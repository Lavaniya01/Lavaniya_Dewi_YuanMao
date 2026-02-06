// GameList.cpp (FULL VERSION - supports quoted CSV fields like "Artifacts, Inc.")

#include "GameList.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

// ---------- helpers ----------
static std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) start++;
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) end--;
    return s.substr(start, end - start);
}

static std::string toLower(const std::string& s) {
    std::string out = s;
    for (size_t i = 0; i < out.size(); i++) {
        out[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(out[i])));
    }
    return out;
}

static bool toIntSafe(const std::string& s, int& out) {
    try {
        std::string t = trim(s);
        if (t.empty()) return false;
        size_t idx = 0;
        int v = std::stoi(t, &idx);
        if (idx != t.size()) return false;
        out = v;
        return true;
    }
    catch (...) {
        return false;
    }
}

static int findColumnIndex(const std::string headers[], int headerCount, const std::string& colName) {
    std::string target = toLower(colName);
    for (int i = 0; i < headerCount; i++) {
        if (toLower(headers[i]) == target) return i;
    }
    return -1;
}

static int splitCSVQuoted(const std::string& line, std::string out[], int maxOut) {
    std::string field;
    bool inQuotes = false;
    int count = 0;

    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];

        if (c == '"') {
            inQuotes = !inQuotes;
        }
        else if (c == ',' && !inQuotes) {
            if (count < maxOut) out[count++] = trim(field);
            field.clear();
        }
        else {
            field.push_back(c);
        }
    }

    if (count < maxOut) out[count++] = trim(field);

    // Remove surrounding quotes if any
    for (int i = 0; i < count; i++) {
        if (out[i].size() >= 2 && out[i].front() == '"' && out[i].back() == '"') {
            out[i] = out[i].substr(1, out[i].size() - 2);
        }
    }
    return count;
}

// Quote a CSV field if needed (handles commas and quotes)
static std::string csvEscape(const std::string& s) {
    bool needsQuotes = false;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == ',' || s[i] == '"' || s[i] == '\n' || s[i] == '\r') {
            needsQuotes = true;
            break;
        }
    }
    if (!needsQuotes) return s;

    std::string out = "\"";
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '"') out += "\"\"";
        else out += s[i];
    }
    out += "\"";
    return out;
}

// ---------- GameList Implementation ----------

GameList::GameList() : head(nullptr) {}

GameList::~GameList() {
    clear();
}

void GameList::clear() {
    GameNode* curr = head;
    while (curr) {
        GameNode* tmp = curr;
        curr = curr->next;
        delete tmp;
    }
    head = nullptr;
}

GameNode* GameList::getHead() const {
    return head;
}

void GameList::append(GameNode* node) {
    if (!node) return;
    node->next = nullptr;

    if (!head) {
        head = node;
        return;
    }

    GameNode* curr = head;
    while (curr->next) curr = curr->next;
    curr->next = node;
}

// Case-SENSITIVE search (exact match)
GameNode* GameList::findByName(const std::string& gameName) {
    GameNode* curr = head;
    while (curr) {
        if (curr->gameName == gameName) return curr;
        curr = curr->next;
    }
    return nullptr;
}

// Case-INSENSITIVE search (flexible match)
// "catan" will find "Catan", "CATAN", etc.
GameNode* GameList::findByNameCaseInsensitive(const std::string& gameName) {
    std::string searchLower = toLower(gameName);

    GameNode* curr = head;
    while (curr) {
        std::string currNameLower = toLower(curr->gameName);
        if (currNameLower == searchLower) {
            return curr;
        }
        curr = curr->next;
    }
    return nullptr;
}

bool GameList::removeByName(const std::string& gameName) {
    GameNode* curr = head;
    GameNode* prev = nullptr;

    while (curr) {
        if (curr->gameName == gameName) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            delete curr;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

bool GameList::appendGameToCSV(const std::string& filename, const GameNode& g) {
    std::ofstream out(filename, std::ios::app);
    if (!out.is_open()) return false;

    out << csvEscape(g.gameName) << ","
        << csvEscape(g.gameId) << ","
        << g.minPlayers << ","
        << g.maxPlayers << ","
        << g.maxPlaytime << ","
        << g.minPlaytime << ","
        << g.yearPublished << ","
        << csvEscape(g.category)
        << "\n";

    return true;
}

void GameList::printAll(int limit) const {
    std::cout << "Game Name\t\tID\tPlayers\tPlaytime\tYear\tCategory\tStatus\tBorrowedBy\n";
    std::cout << "--------------------------------------------------------------------------\n";

    int shown = 0;
    GameNode* curr = head;
    while (curr && shown < limit) {
        std::cout << curr->gameName << "\t\t"
            << curr->gameId << "\t"
            << curr->minPlayers << "-" << curr->maxPlayers << "\t"
            << curr->minPlaytime << "-" << curr->maxPlaytime << "\t\t"
            << curr->yearPublished << "\t"
            << curr->category << "\t"
            << (curr->status == 'A' ? "Avail" : "Borrow") << "\t"
            << (curr->borrowedBy.empty() ? "-" : curr->borrowedBy)
            << "\n";
        curr = curr->next;
        shown++;
    }

    if (shown == 0) std::cout << "(No games loaded)\n";
    std::cout << "Showing " << shown << " games.\n";
}

bool GameList::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "ERROR: Cannot open: " << filename << "\n";
        return false;
    }

    clear();

    std::string headerLine;
    if (!std::getline(file, headerLine)) {
        std::cout << "ERROR: CSV is empty.\n";
        return false;
    }

    const int MAX_COLS = 20;
    std::string headers[MAX_COLS];
    int headerCount = splitCSVQuoted(headerLine, headers, MAX_COLS);

    int idxName = findColumnIndex(headers, headerCount, "name");
    int idxId = findColumnIndex(headers, headerCount, "gameId");
    int idxMinP = findColumnIndex(headers, headerCount, "minplayers");
    int idxMaxP = findColumnIndex(headers, headerCount, "maxplayers");
    int idxMaxT = findColumnIndex(headers, headerCount, "maxplaytime");
    int idxMinT = findColumnIndex(headers, headerCount, "minplaytime");
    int idxYear = findColumnIndex(headers, headerCount, "yearpublished");
    int idxCat = findColumnIndex(headers, headerCount, "category");

    if (idxName < 0 || idxId < 0 || idxMinP < 0 || idxMaxP < 0 ||
        idxMinT < 0 || idxMaxT < 0 || idxYear < 0 || idxCat < 0) {
        std::cout << "ERROR: Missing required columns. Need:\n";
        std::cout << "name, gameId, minplayers, maxplayers, minplaytime, maxplaytime, yearpublished, category\n";
        return false;
    }

    std::string line;
    int loaded = 0;
    int skipped = 0;

    while (std::getline(file, line)) {
        if (trim(line).empty()) continue;

        std::string cols[MAX_COLS];
        int colCount = splitCSVQuoted(line, cols, MAX_COLS);

        int maxIdx = idxName;
        if (idxId > maxIdx) maxIdx = idxId;
        if (idxMinP > maxIdx) maxIdx = idxMinP;
        if (idxMaxP > maxIdx) maxIdx = idxMaxP;
        if (idxMinT > maxIdx) maxIdx = idxMinT;
        if (idxMaxT > maxIdx) maxIdx = idxMaxT;
        if (idxYear > maxIdx) maxIdx = idxYear;
        if (idxCat > maxIdx) maxIdx = idxCat;

        if (colCount <= maxIdx) { skipped++; continue; }

        std::string id = cols[idxId];
        std::string cat = cols[idxCat];
        std::string name = cols[idxName];
        if (name.empty()) { skipped++; continue; }
        if (id.empty()) { skipped++; continue; }
        if (cat.empty()) cat = "Unknown";

        int minP = 0, maxP = 0, minT = 0, maxT = 0, year = 0;
        if (!toIntSafe(cols[idxMinP], minP)) { skipped++; continue; }
        if (!toIntSafe(cols[idxMaxP], maxP)) { skipped++; continue; }
        if (!toIntSafe(cols[idxMinT], minT)) { skipped++; continue; }
        if (!toIntSafe(cols[idxMaxT], maxT)) { skipped++; continue; }
        if (!toIntSafe(cols[idxYear], year)) { skipped++; continue; }

        GameNode* node = new GameNode();
        node->gameName = name;
        node->gameId = id;
        node->minPlayers = minP;
        node->maxPlayers = maxP;
        node->minPlaytime = minT;
        node->maxPlaytime = maxT;
        node->yearPublished = year;
        node->category = cat;
        node->status = 'A';
        node->borrowedBy = "";

        append(node);
        loaded++;
    }

    std::cout << "Loaded " << loaded << " game entries from CSV. ";
    std::cout << "(Skipped " << skipped << " invalid rows)\n";
    return true;
}