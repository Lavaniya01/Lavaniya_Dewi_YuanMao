// Lavaniya_Dewi_YuanMao.cpp (FULL MAIN - Reviews + Nice Table + CSV Persistence)

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>

#include "GameList.h"
#include "MemberList.h"
#include "MemberNode.h"
#include "TransactionQueue.h"
#include "RatingList.h"
#include "RatingNode.h"
#include "ReviewList.h"  // NEW: Review feature
#include "Utils.h"
#include <limits>
#include "PlayRecordList.h"

// ---------- Date helper ----------
static std::string todayDate() {
    std::time_t t = std::time(nullptr);
    std::tm tmResult{};
    localtime_s(&tmResult, &t);

    int y = 1900 + tmResult.tm_year;
    int m = 1 + tmResult.tm_mon;
    int d = tmResult.tm_mday;

    char buf[11];
    std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d", y, m, d);
    return std::string(buf);
}

// ---------- UI helpers ----------
static void printLine(int width = 95) {
    for (int i = 0; i < width; i++) std::cout << "-";
    std::cout << "\n";
}

static std::string statusText(char s) {
    return (s == 'A') ? "Available" : "Borrowed";
}

static void printBorder(int nameW, int idW, int playersW, int timeW, int yearW, int catW, int statusW, int borrowedW) {
    std::cout << "+"
        << std::string(nameW + 2, '-') << "+"
        << std::string(idW + 2, '-') << "+"
        << std::string(playersW + 2, '-') << "+"
        << std::string(timeW + 2, '-') << "+"
        << std::string(yearW + 2, '-') << "+"
        << std::string(catW + 2, '-') << "+"
        << std::string(statusW + 2, '-') << "+"
        << std::string(borrowedW + 2, '-') << "+\n";
}

static void printCell(const std::string& s, int width) {
    std::cout << " " << std::left << std::setw(width) << s << " ";
}

/*
NO-STL word wrap:
- Splits 'text' into up to maxLines lines
- Each line is at most 'width' characters
- Prefer breaking at spaces
- Stores results into outLines[0..lineCount-1]
*/
static void wrapTextNoSTL(const std::string& text, int width,
    std::string outLines[], int maxLines, int& lineCount) {
    lineCount = 0;
    if (width <= 0 || maxLines <= 0) return;

    int n = static_cast<int>(text.size());
    int start = 0;

    while (start < n&& lineCount < maxLines) {
        int end = start + width;
        if (end > n) end = n;

        // Try to break at last space within the line
        int breakPos = -1;
        for (int i = end - 1; i > start; i--) {
            if (text[i] == ' ') { breakPos = i; break; }
        }

        if (breakPos != -1) {
            outLines[lineCount++] = text.substr(start, breakPos - start);
            start = breakPos + 1; // skip the space
        }
        else {
            outLines[lineCount++] = text.substr(start, end - start);
            start = end;
        }

        // Skip extra spaces
        while (start < n && text[start] == ' ') start++;
    }

    // If text remains but we hit maxLines, mark the last line with "..."
    if (start < n && lineCount > 0) {
        std::string& last = outLines[lineCount - 1];
        if (last.size() >= 3) {
            last[last.size() - 3] = '.';
            last[last.size() - 2] = '.';
            last[last.size() - 1] = '.';
        }
        else {
            last = "...";
        }
    }
}

// ---------- Game field helpers ----------
static std::string getGameName(GameNode* g) { return g->gameName; }
static int getYearPublished(GameNode* g) { return g->yearPublished; }
static int getMinPlaytime(GameNode* g) { return g->minPlaytime; }
static int getMaxPlaytime(GameNode* g) { return g->maxPlaytime; }
PlayRecordList playRecords;


// ---------- Menus ----------
static void showMainMenu() {
    std::cout << "\n============================================\n";
    std::cout << "   NPTTGC Board Game Management System\n";
    std::cout << "============================================\n";
    std::cout << "1. Admin Menu\n";
    std::cout << "2. Member Menu\n";
    std::cout << "3. View Games (Guest)\n";
    std::cout << "0. Exit\n";
    std::cout << "Select: ";
}

static void showAdminMenu() {
    std::cout << "\n==================== ADMIN MENU ====================\n";
    std::cout << "1. Add Game Copy (Add New Game)\n";
    std::cout << "2. Remove Game Copy\n";
    std::cout << "3. Add New Member\n";
    std::cout << "4. View Borrow/Return Summary (ALL)\n";
    std::cout << "0. Back\n";
    std::cout << "Select: ";
}

static void showMemberMenu() {
    std::cout << "\n==================== MEMBER MENU ====================\n";
    std::cout << "1. Borrow a Game\n";
    std::cout << "2. Return a Game\n";
    std::cout << "3. My Borrow/Return Summary\n";
    std::cout << "4. Rate a Game (1-10)\n";
    std::cout << "5. Write a Review for a Game\n";      // NEW
    std::cout << "6. Read Reviews for a Game\n";        // NEW
    std::cout << "7. View Game Details (Average Rating)\n";
    std::cout << "8. Games Playable by N Players (Sort)\n";
    std::cout << "9. Record Game Play (Players and winners)\n";
    std::cout << "10.View my gameplay history\n";
    std::cout << "11. Recommend Games (Based on Ratings)\n";
    std::cout << "0. Back\n";
    std::cout << "Select: ";
}

// ---------- Printing games nicely (table + wrapping names) ----------
static void printGamesHeader() {
    const int NAME_W = 38;
    const int ID_W = 8;
    const int PLAYERS_W = 9;
    const int TIME_W = 11;
    const int YEAR_W = 6;
    const int CAT_W = 12;
    const int STATUS_W = 10;
    const int BORROWED_W = 10;

    printBorder(NAME_W, ID_W, PLAYERS_W, TIME_W, YEAR_W, CAT_W, STATUS_W, BORROWED_W);

    std::cout << "|";
    printCell("Game Name", NAME_W);       std::cout << "|";
    printCell("Game ID", ID_W);           std::cout << "|";
    printCell("Players", PLAYERS_W);      std::cout << "|";
    printCell("Playtime", TIME_W);        std::cout << "|";
    printCell("Year", YEAR_W);            std::cout << "|";
    printCell("Category", CAT_W);         std::cout << "|";
    printCell("Status", STATUS_W);        std::cout << "|";
    printCell("BorrowedBy", BORROWED_W);  std::cout << "|\n";

    printBorder(NAME_W, ID_W, PLAYERS_W, TIME_W, YEAR_W, CAT_W, STATUS_W, BORROWED_W);
}

static void printGamesBottomBorder() {
    const int NAME_W = 38;
    const int ID_W = 8;
    const int PLAYERS_W = 9;
    const int TIME_W = 11;
    const int YEAR_W = 6;
    const int CAT_W = 12;
    const int STATUS_W = 10;
    const int BORROWED_W = 10;
    printBorder(NAME_W, ID_W, PLAYERS_W, TIME_W, YEAR_W, CAT_W, STATUS_W, BORROWED_W);
}

static void printGameRow(GameNode* g) {
    const int NAME_W = 38;
    const int ID_W = 8;
    const int PLAYERS_W = 9;
    const int TIME_W = 11;
    const int YEAR_W = 6;
    const int CAT_W = 12;
    const int STATUS_W = 10;
    const int BORROWED_W = 10;

    std::string name = getGameName(g);
    std::string players = std::to_string(g->minPlayers) + "-" + std::to_string(g->maxPlayers);
    std::string time = std::to_string(getMinPlaytime(g)) + "-" + std::to_string(getMaxPlaytime(g));
    std::string year = std::to_string(getYearPublished(g));
    std::string status = statusText(g->status);
    std::string borrowed = g->borrowedBy.empty() ? "-" : g->borrowedBy;

    const int MAX_LINES = 4;
    std::string lines[MAX_LINES];
    int lineCount = 0;
    wrapTextNoSTL(name, NAME_W, lines, MAX_LINES, lineCount);
    if (lineCount == 0) { lines[0] = ""; lineCount = 1; }

    for (int i = 0; i < lineCount; i++) {
        std::cout << "|";
        printCell(lines[i], NAME_W); std::cout << "|";

        if (i == 0) {
            printCell(g->gameId, ID_W);      std::cout << "|";
            printCell(players, PLAYERS_W);   std::cout << "|";
            printCell(time, TIME_W);         std::cout << "|";
            printCell(year, YEAR_W);         std::cout << "|";
            printCell(g->category, CAT_W);   std::cout << "|";
            printCell(status, STATUS_W);     std::cout << "|";
            printCell(borrowed, BORROWED_W); std::cout << "|\n";
        }
        else {
            printCell("", ID_W);        std::cout << "|";
            printCell("", PLAYERS_W);   std::cout << "|";       
            printCell("", TIME_W);      std::cout << "|";
            printCell("", YEAR_W);      std::cout << "|";
            printCell("", CAT_W);       std::cout << "|";
            printCell("", STATUS_W);    std::cout << "|";
            printCell("", BORROWED_W);  std::cout << "|\n";
        }
    }

    printGamesBottomBorder();
}

static void viewGamesAll(GameList& games) {
    GameNode* curr = games.getHead();
    if (!curr) {
        std::cout << "(No games loaded)\n";
        return;
    }

    const int PAGE_SIZE = 10;
    int shown = 0;
    int pageCount = 0;

    printGamesHeader();

    while (curr) {
        int printedThisPage = 0;

        // Print up to PAGE_SIZE games
        while (curr && printedThisPage < PAGE_SIZE) {
            printGameRow(curr);
            curr = curr->next;
            printedThisPage++;
            shown++;
        }

        pageCount++;

        // If more games remain, pause
        if (curr) {
            std::cout << "---- Showing " << shown
                << " games so far. Press ENTER to load more ----";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::cout << "\nEnd of list. Total games shown: " << shown << "\n";
    std::cout << "Tip: Copy the Game Name exactly when borrowing/returning/rating/reviewing.\n";
}

// ---------- Admin actions ----------
static void adminAddGame(GameList& games) {
    std::cout << "\n--- Add Game Copy ---\n";
    std::string name = readLine("Game Name: ");
    if (name.empty()) { std::cout << "Game Name cannot be empty.\n"; return; }

    std::string id = readLine("Game ID: ");
    if (id.empty()) { std::cout << "Game ID cannot be empty.\n"; return; }

    std::string category = readLine("Category: ");
    if (category.empty()) category = "Unknown";

    int minP, maxP, minT, maxT, year;
    if (!readInt("Min Players: ", minP)) { std::cout << "Invalid.\n"; return; }
    if (!readInt("Max Players: ", maxP)) { std::cout << "Invalid.\n"; return; }
    if (!readInt("Min Playtime (mins): ", minT)) { std::cout << "Invalid.\n"; return; }
    if (!readInt("Max Playtime (mins): ", maxT)) { std::cout << "Invalid.\n"; return; }
    if (!readInt("Year Published: ", year)) { std::cout << "Invalid.\n"; return; }

    if (minP > maxP) { std::cout << "Min players cannot be > max players.\n"; return; }
    if (minT > maxT) { std::cout << "Min playtime cannot be > max playtime.\n"; return; }

    GameNode* node = new GameNode();
    node->gameName = name;
    node->gameId = id;
    node->category = category;
    node->minPlayers = minP;
    node->maxPlayers = maxP;
    node->minPlaytime = minT;
    node->maxPlaytime = maxT;
    node->yearPublished = year;
    node->category = category;
    node->status = 'A';
    node->borrowedBy = "";

    games.append(node);
    if (!games.appendGameToCSV("data/games.csv", *node)) {
        std::cout << "WARNING: Game saved in memory but failed to write to CSV.\n";
    }

    std::cout << "Game copy added.\n";
}

static void adminRemoveGame(GameList& games) {
    std::cout << "\n--- Remove Game Copy ---\n";
    std::string name = readLine("Enter Game Name to remove: ");
    if (name.empty()) { std::cout << "Game Name cannot be empty.\n"; return; }

    GameNode* g = games.findByName(name);
    if (!g) { std::cout << "Not found.\n"; return; }
    if (g->status == 'B') { std::cout << "Cannot remove: this copy is currently borrowed.\n"; return; }

    if (games.removeByName(name)) std::cout << "Removed.\n";
    else std::cout << "Remove failed.\n";
}

static void adminAddMember(MemberList& members) {
    std::cout << "\n--- Add Member ---\n";
    std::string id = readLine("MemberID: ");
    if (id.empty()) { std::cout << "MemberID cannot be empty.\n"; return; }
    if (members.exists(id)) { std::cout << "MemberID already exists.\n"; return; }

    std::string name = readLine("Name: ");

    MemberNode* m = new MemberNode();
    m->memberID = id;
    m->name = name;
    members.append(m);
    if (!members.appendMemberToCSV("data/members.csv", id, name)) {
        std::cout << "WARNING: Member saved in memory but failed to write to CSV.\n";
    }

    std::cout << "Member added.\n";
}

// ---------- Member actions ----------
static void memberBorrow(GameList& games, MemberList& members, TransactionQueue& tx) {
    std::cout << "\n--- Borrow a Game ---\n";
    std::string memberID = readLine("MemberID: ");
    if (!members.exists(memberID)) { std::cout << "Member not found.\n"; return; }

    std::string gameName = readLine("Game Name: ");
    GameNode* g = games.findByName(gameName);
    if (!g) { std::cout << "Game not found. (Tip: copy the name exactly from View Games)\n"; return; }
    if (g->status == 'B') { std::cout << "Already borrowed.\n"; return; }

    g->status = 'B';
    g->borrowedBy = memberID;

    tx.enqueue(memberID, gameName, gameName, "BORROW", todayDate());
    std::cout << "Borrow successful.\n";
}

static void memberReturn(GameList& games, MemberList& members, TransactionQueue& tx) {
    std::cout << "\n--- Return a Game ---\n";
    std::string memberID = readLine("MemberID: ");
    if (!members.exists(memberID)) { std::cout << "Member not found.\n"; return; }

    std::string gameName = readLine("Game Name: ");
    GameNode* g = games.findByName(gameName);
    if (!g) { std::cout << "Game not found.\n"; return; }
    if (g->status == 'A') { std::cout << "This copy is not borrowed.\n"; return; }
    if (g->borrowedBy != memberID) { std::cout << "Return denied: not borrowed by you.\n"; return; }

    g->status = 'A';
    g->borrowedBy = "";

    tx.enqueue(memberID, gameName, gameName, "RETURN", todayDate());
    std::cout << "Return successful.\n";
}

static void memberRate(GameList& games, MemberList& members, RatingList& ratings) {
    std::cout << "\n--- Rate a Game (1-10) ---\n";
    std::string memberID = readLine("MemberID: ");
    if (!members.exists(memberID)) { std::cout << "Member not found.\n"; return; }

    std::string gameName = readLine("Game Name: ");
    GameNode* g = games.findByName(gameName);
    if (!g) { std::cout << "Game not found.\n"; return; }

    int r;
    if (!readInt("Rating (1-10): ", r)) { std::cout << "Invalid rating.\n"; return; }
    if (r < 1 || r > 10) { std::cout << "Rating must be 1 to 10.\n"; return; }

    ratings.addOrUpdate(memberID,
        g->gameId,
        r,
        todayDate());

    ratings.appendToCSV("data/ratings.csv",
        memberID,
        g->gameId,
        r,
        todayDate());

    std::cout << "Rating saved.\n";
}

static GameNode* findGameByIdOrName(GameList& games, const std::string& input) {
    GameNode* curr = games.getHead();
    while (curr) {
        if (curr->gameId == input || curr->gameName == input) return curr;
        curr = curr->next;
    }
    return nullptr;
}

static bool memberInList(const std::string members[], int count, const std::string& id) {
    for (int i = 0; i < count; i++) {
        if (members[i] == id) return true;
    }
    return false;
}

static int findGameIndex(const std::string gameIDs[], int count, const std::string& gid) {
    for (int i = 0; i < count; i++) {
        if (gameIDs[i] == gid) return i;
    }
    return -1;
}

static void recommendGames(GameList& games, RatingList& ratings) {
    std::cout << "\n--- Recommend Games (Based on Ratings) ---\n";
    std::string input = readLine("Enter Game Name or Game ID: ");

    GameNode* target = findGameByIdOrName(games, input);
    if (!target) {
        std::cout << "Game not found. Please copy Game Name / Game ID from View Games.\n";
        return;
    }

    const int LIKE_THRESHOLD = 7;
    const int MAX_MEMBERS = 5000;
    const int MAX_GAMES = 5000;

    std::string likedMembers[MAX_MEMBERS];
    int likedCount = 0;

    // 1) Find members who "liked" the selected game (rating >= 7)
    RatingNode* r = ratings.getHead();
    while (r) {
        if (r->gameID == target->gameId && r->rating >= LIKE_THRESHOLD) {
            if (!memberInList(likedMembers, likedCount, r->memberID) && likedCount < MAX_MEMBERS) {
                likedMembers[likedCount++] = r->memberID;
            }
        }
        r = r->next;
    }

    if (likedCount == 0) {
        std::cout << "No recommendations found.\n";
        std::cout << "Reason: No members rated this game >= " << LIKE_THRESHOLD << " yet.\n";
        return;
    }

    // 2) For those members, find other games they liked, count score per gameID
    std::string recGameIDs[MAX_GAMES];
    int recScores[MAX_GAMES];
    int recCount = 0;

    r = ratings.getHead();
    while (r) {
        if (r->rating >= LIKE_THRESHOLD && memberInList(likedMembers, likedCount, r->memberID)) {
            if (r->gameID != target->gameId) {
                int idx = findGameIndex(recGameIDs, recCount, r->gameID);
                if (idx == -1) {
                    if (recCount < MAX_GAMES) {
                        recGameIDs[recCount] = r->gameID;
                        recScores[recCount] = 1;
                        recCount++;
                    }
                }
                else {
                    recScores[idx]++;
                }
            }
        }
        r = r->next;
    }

    if (recCount == 0) {
        std::cout << "No recommendations found.\n";
        std::cout << "Reason: Similar members didn't rate other games >= " << LIKE_THRESHOLD << ".\n";
        return;
    }

    // 3) Sort by score desc (bubble sort)
    for (int i = 0; i < recCount - 1; i++) {
        for (int j = 0; j < recCount - 1 - i; j++) {
            if (recScores[j] < recScores[j + 1]) {
                int tmpS = recScores[j];
                recScores[j] = recScores[j + 1];
                recScores[j + 1] = tmpS;

                std::string tmpID = recGameIDs[j];
                recGameIDs[j] = recGameIDs[j + 1];
                recGameIDs[j + 1] = tmpID;
            }
        }
    }

    // 4) Print top results
    std::cout << "\nRecommendations based on: " << target->gameName
        << " (" << target->gameId << ") | Category: " << target->category << "\n";
    std::cout << "Because members who rated this game >= " << LIKE_THRESHOLD
        << " also liked these games:\n\n";

    std::cout << "#  "
        << std::left
        << std::setw(9) << "GameID"
        << std::setw(35) << "Game Name"
        << std::setw(15) << "Category"
        << "Score\n";
    std::cout << "--------------------------------------------------------------------------\n";

    int topN = 5;
    if (recCount < topN) topN = recCount;

    for (int i = 0; i < topN; i++) {
        GameNode* g = findGameByIdOrName(games, recGameIDs[i]); // finds by ID
        if (!g) continue;

        std::cout << std::left
            << std::setw(3) << (i + 1)
            << std::setw(9) << g->gameId
            << std::setw(35) << g->gameName
            << std::setw(15) << g->category
            << recScores[i] << "\n";
    }

    std::cout << "\nScore = number of similar members who rated that game >= " << LIKE_THRESHOLD << "\n";
}

static void recordGamePlay(GameList& games, MemberList& members, PlayRecordList& plays) {
    std::cout << "\n--- Record Game Play ---\n";

    std::string gameName = readLine("Enter Game Name: ");
    GameNode* g = games.findByName(gameName);
    if (!g) {
        std::cout << "Game not found.\n";
        return;
    }

    int pCount;
    if (!readInt("How many players? ", pCount)) {
        std::cout << "Invalid number.\n";
        return;
    }
    if (pCount < 1 || pCount > MAX_PLAYERS_PER_PLAY) {
        std::cout << "Player count must be 1 to " << MAX_PLAYERS_PER_PLAY << ".\n";
        return;
    }

    PlayRecordNode* r = new PlayRecordNode();
    r->gameName = gameName;
    r->date = todayDate();
    r->playerCount = pCount;

    for (int i = 0; i < pCount; i++) {
        std::string pid = readLine("Player " + std::to_string(i + 1) + " MemberID: ");
        if (!members.exists(pid)) {
            std::cout << "Member not found: " << pid << "\n";
            delete r;
            return;
        }
        r->players[i] = pid;
    }

    std::string winner = readLine("Winner MemberID (or DRAW): ");
    if (winner != "DRAW" && !members.exists(winner)) {
        std::cout << "Winner unknown.\n";
        delete r;
        return;
    }
    r->winnerID = winner;

    // store in memory
    plays.append(r);

    // save to CSV
    if (!plays.appendToCSV("data/plays.csv", *r)) {
        std::cout << "WARNING: Could not append to plays.csv\n";
    }
    else {
        std::cout << "Play recorded!\n";
    }
}


// ---------- NEW: Review actions ----------
static void memberWriteReview(GameList& games, MemberList& members, ReviewList& reviews) {
    std::cout << "\n--- Write a Review ---\n";
    std::string memberID = readLine("MemberID: ");
    if (!members.exists(memberID)) { std::cout << "Member not found.\n"; return; }

    std::string gameName = readLine("Game Name: ");
    GameNode* g = games.findByName(gameName);
    if (!g) { std::cout << "Game not found. (Tip: copy the name exactly from View Games)\n"; return; }

    // Check if already reviewed
    if (reviews.hasReviewed(memberID, gameName)) {
        std::cout << "You have already reviewed this game. This will update your previous review.\n";
    }

    int rating;
    if (!readInt("Rating (1-10): ", rating)) { std::cout << "Invalid rating.\n"; return; }
    if (rating < 1 || rating > 10) { std::cout << "Rating must be between 1 and 10.\n"; return; }

    std::cout << "Write your review (press Enter twice when done):\n";
    std::cout << "> ";

    std::string reviewText;
    std::string line;

    // Read multiple lines until empty line
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        if (!reviewText.empty()) reviewText += "\n";
        reviewText += line;
        std::cout << "> ";
    }

    if (reviewText.empty()) {
        std::cout << "Review cannot be empty.\n";
        return;
    }

    reviews.addOrUpdate(memberID, gameName, rating, reviewText, todayDate());
    std::cout << "\nReview saved successfully!\n";
}

static void memberReadReviews(GameList& games, ReviewList& reviews) {
    std::cout << "\n--- Read Reviews ---\n";
    std::string gameName = readLine("Enter Game Name: ");
    GameNode* g = games.findByName(gameName);
    if (!g) { std::cout << "Game not found. (Tip: copy the name exactly from View Games)\n"; return; }

    reviews.printReviewsForGame(gameName);
}

// ---------- View Game Details (uses ReviewList for average) ----------
static void viewGameDetails(GameList& games, ReviewList& reviews) {
    std::cout << "\n--- Game Details ---\n";
    std::string gameName = readLine("Enter Game Name: ");
    GameNode* g = games.findByName(gameName);
    if (!g) { std::cout << "Game not found.\n"; return; }

    printGamesHeader();
    printGameRow(g);

    double avg = reviews.getAverage(gameName);
    int count = reviews.countReviews(gameName);
    if (avg < 0) std::cout << "Average Rating: No reviews yet\n";
    else std::cout << "Average Rating: " << std::fixed << std::setprecision(2) << avg
        << " (based on " << count << " review" << (count > 1 ? "s" : "") << ")\n";

    std::cout << "\nTip: Use 'Read Reviews for a Game' to see detailed reviews.\n";
}

// ---------- Sorting helper (uses ReviewList) ----------
static double avgRatingForName(ReviewList& reviews, const std::string& gameName) {
    double avg = reviews.getAverage(gameName);
    return (avg < 0) ? 0.0 : avg;
}

static void gamesPlayableByN(GameList& games, ReviewList& reviews) {
    std::cout << "\n--- Games Playable by N Players ---\n";
    int N;
    if (!readInt("Enter number of players N: ", N)) { std::cout << "Invalid N.\n"; return; }
    if (N <= 0) { std::cout << "N must be > 0.\n"; return; }

    std::cout << "Sort by:\n";
    std::cout << "1. Year Asc\n";
    std::cout << "2. Year Desc\n";
    std::cout << "3. Avg Rating Desc\n";
    std::cout << "0. No Sort\n";
    std::cout << "Select: ";

    int sortChoice;
    if (!(std::cin >> sortChoice)) { std::cin.clear(); clearInputLine(); return; }
    clearInputLine();

    const int MAX = 5000;
    GameNode** arr = new GameNode * [MAX];
    int count = 0;

    GameNode* curr = games.getHead();
    while (curr && count < MAX) {
        if (curr->minPlayers <= N && N <= curr->maxPlayers) {
            arr[count++] = curr;
        }
        curr = curr->next;
    }

    if (sortChoice != 0) {
        for (int i = 0; i < count - 1; i++) {
            for (int j = 0; j < count - 1 - i; j++) {
                bool swapNeeded = false;

                if (sortChoice == 1) {
                    if (arr[j]->yearPublished > arr[j + 1]->yearPublished) swapNeeded = true;
                }
                else if (sortChoice == 2) {
                    if (arr[j]->yearPublished < arr[j + 1]->yearPublished) swapNeeded = true;
                }
                else if (sortChoice == 3) {
                    double a = avgRatingForName(reviews, arr[j]->gameName);
                    double b = avgRatingForName(reviews, arr[j + 1]->gameName);
                    if (a < b) swapNeeded = true;
                }

                if (swapNeeded) {
                    GameNode* tmp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = tmp;
                }
            }
        }
    }

    printGamesHeader();
    for (int i = 0; i < count; i++) {
        printGameRow(arr[i]);
    }
    std::cout << "Matches for N=" << N << ": " << count << "\n";

    delete[] arr;
}

// ---------- MAIN ----------
int main() {
    GameList games;
    MemberList members;
    TransactionQueue tx;
    RatingList ratings;
    ReviewList reviews;  // NEW: Review list

    games.loadFromCSV("data/games.csv");
    members.loadFromCSV("data/members.csv");
    ratings.loadFromCSV("data/ratings.csv");

    int choice = -1;
    while (choice != 0) {
        showMainMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            clearInputLine();
            continue;
        }
        clearInputLine();

        if (choice == 1) {
            int a = -1;
            while (a != 0) {
                showAdminMenu();
                if (!(std::cin >> a)) { std::cin.clear(); clearInputLine(); continue; }
                clearInputLine();

                if (a == 1) adminAddGame(games);
                else if (a == 2) adminRemoveGame(games);
                else if (a == 3) adminAddMember(members);
                else if (a == 4) tx.printAll();
                else if (a == 0) {}
                else std::cout << "Invalid option.\n";
            }
        }
        else if (choice == 2) {
            int m = -1;
            while (m != 0) {
                showMemberMenu();
                if (!(std::cin >> m)) { std::cin.clear(); clearInputLine(); continue; }
                clearInputLine();

                if (m == 1) memberBorrow(games, members, tx);
                else if (m == 2) memberReturn(games, members, tx);
                else if (m == 3) {
                    std::string memberID = readLine("MemberID: ");
                    tx.printByMember(memberID);
                }
                else if (m == 4) memberRate(games, members, ratings);
                else if (m == 5) memberWriteReview(games, members, reviews);  // NEW
                else if (m == 6) memberReadReviews(games, reviews);           // NEW
                else if (m == 7) viewGameDetails(games, reviews);             // Updated to use reviews
                else if (m == 8) gamesPlayableByN(games, reviews);            // Updated to use reviews
                else if (m == 9) {
                    recordGamePlay(games, members, playRecords);
                }
                else if (m == 10) {
                    std::string memberID = readLine("MemberID: ");
                    playRecords.printByMember(memberID);
                }
                else if (m == 11) recommendGames(games, ratings);
                else if (m == 0) {}
                else std::cout << "Invalid option.\n";
            }
        }
        else if (choice == 3) {
            viewGamesAll(games);
        }
        else if (choice == 0) {
            std::cout << "Goodbye!\n";
        }
        else {
            std::cout << "Invalid option.\n";
        }
    }

    return 0;
}