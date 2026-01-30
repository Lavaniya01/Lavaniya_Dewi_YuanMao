// Lavaniya_Dewi_YuanMao.cpp (FULL MAIN - NO STL containers, nice table + wraps long names)

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
#include "Utils.h"
#include <limits>

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

static void printBorder(int nameW, int playersW, int timeW, int yearW, int statusW, int borrowedW) {
    std::cout << "+"
        << std::string(nameW + 2, '-') << "+"
        << std::string(playersW + 2, '-') << "+"
        << std::string(timeW + 2, '-') << "+"
        << std::string(yearW + 2, '-') << "+"
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
    std::cout << "5. View Game Details (Average Rating)\n";
    std::cout << "6. Games Playable by N Players (Sort)\n";
    std::cout << "0. Back\n";
    std::cout << "Select: ";
}

// ---------- Printing games nicely (table + wrapping names) ----------
static void printGamesHeader() {
    const int NAME_W = 38;
    const int PLAYERS_W = 9;
    const int TIME_W = 11;
    const int YEAR_W = 6;
    const int STATUS_W = 10;
    const int BORROWED_W = 10;

    printBorder(NAME_W, PLAYERS_W, TIME_W, YEAR_W, STATUS_W, BORROWED_W);

    std::cout << "|";
    printCell("Game Name", NAME_W);       std::cout << "|";
    printCell("Players", PLAYERS_W);      std::cout << "|";
    printCell("Playtime", TIME_W);        std::cout << "|";
    printCell("Year", YEAR_W);            std::cout << "|";
    printCell("Status", STATUS_W);        std::cout << "|";
    printCell("BorrowedBy", BORROWED_W);  std::cout << "|\n";

    printBorder(NAME_W, PLAYERS_W, TIME_W, YEAR_W, STATUS_W, BORROWED_W);
}

static void printGamesBottomBorder() {
    const int NAME_W = 38;
    const int PLAYERS_W = 9;
    const int TIME_W = 11;
    const int YEAR_W = 6;
    const int STATUS_W = 10;
    const int BORROWED_W = 10;
    printBorder(NAME_W, PLAYERS_W, TIME_W, YEAR_W, STATUS_W, BORROWED_W);
}

static void printGameRow(GameNode* g) {
    const int NAME_W = 38;
    const int PLAYERS_W = 9;
    const int TIME_W = 11;
    const int YEAR_W = 6;
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
            printCell(players, PLAYERS_W);   std::cout << "|";
            printCell(time, TIME_W);         std::cout << "|";
            printCell(year, YEAR_W);         std::cout << "|";
            printCell(status, STATUS_W);     std::cout << "|";
            printCell(borrowed, BORROWED_W); std::cout << "|\n";
        }
        else {
            printCell("", PLAYERS_W);   std::cout << "|";
            printCell("", TIME_W);      std::cout << "|";
            printCell("", YEAR_W);      std::cout << "|";
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
}


// ---------- Admin actions ----------
static void adminAddGame(GameList& games) {
    std::cout << "\n--- Add Game Copy ---\n";
    std::string name = readLine("Game Name: ");
    if (name.empty()) { std::cout << "Game Name cannot be empty.\n"; return; }

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
    node->minPlayers = minP;
    node->maxPlayers = maxP;
    node->minPlaytime = minT;
    node->maxPlaytime = maxT;
    node->yearPublished = year;

    node->status = 'A';
    node->borrowedBy = "";

    games.append(node);
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

    ratings.addOrUpdate(memberID, gameName, r);
    std::cout << "Rating saved.\n";
}

static void viewGameDetails(GameList& games, RatingList& ratings) {
    std::cout << "\n--- Game Details ---\n";
    std::string gameName = readLine("Enter Game Name: ");
    GameNode* g = games.findByName(gameName);
    if (!g) { std::cout << "Game not found.\n"; return; }

    printGamesHeader();
    printGameRow(g);

    double avg = ratings.getAverage(gameName);
    int count = ratings.countRatings(gameName);
    if (avg < 0) std::cout << "Average Rating: No ratings yet\n";
    else std::cout << "Average Rating: " << std::fixed << std::setprecision(2) << avg
        << " (based on " << count << " ratings)\n";
}

// ---------- Sorting helper ----------
static double avgRatingForName(RatingList& ratings, const std::string& gameName) {
    double avg = ratings.getAverage(gameName);
    return (avg < 0) ? 0.0 : avg;
}

static void gamesPlayableByN(GameList& games, RatingList& ratings) {
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
                    double a = avgRatingForName(ratings, arr[j]->gameName);
                    double b = avgRatingForName(ratings, arr[j + 1]->gameName);
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

    games.loadFromCSV("data/games.csv");
    members.loadFromCSV("data/members.csv");


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
                else if (m == 5) viewGameDetails(games, ratings);
                else if (m == 6) gamesPlayableByN(games, ratings);
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
