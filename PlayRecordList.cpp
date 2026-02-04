// PlayRecordList.cpp  (FULL FILE)
// - No STL containers (no vector/map)
// - Supports:
//   1) append() in-memory (linked list)
//   2) appendToCSV() persist one record (append-only)
//   3) loadFromCSV() load records back after restart
//   4) printAll() / printByMember()

#include "PlayRecordList.h"
#include <iostream>
#include <fstream>

// ----------------- CSV helpers -----------------

// Escape field for CSV (quotes commas/quotes/newlines)
static std::string escapeCSV(const std::string& s) {
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

// Remove trailing \r from Windows line endings
static void trimCR(std::string& s) {
    if (!s.empty() && s[s.size() - 1] == '\r') s.pop_back();
}

// Parse 5-column CSV line with quotes support
// expected: date,gameName,playerCount,players,winnerID
static bool parseCSV5(const std::string& line, std::string out[5]) {
    int col = 0;
    std::string cur;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];

        if (inQuotes) {
            if (c == '"') {
                // escaped quote ""
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    cur += '"';
                    i++;
                }
                else {
                    inQuotes = false;
                }
            }
            else {
                cur += c;
            }
        }
        else {
            if (c == '"') {
                inQuotes = true;
            }
            else if (c == ',') {
                if (col >= 5) return false;
                out[col++] = cur;
                cur.clear();
            }
            else {
                cur += c;
            }
        }
    }

    if (col != 4) return false;
    out[col] = cur;
    return true;
}

// Join players with ';'
static std::string joinPlayers(const std::string players[], int count) {
    std::string out = "";
    for (int i = 0; i < count; i++) {
        out += players[i];
        if (i != count - 1) out += ";";
    }
    return out;
}

// Split "M001;M002;M003" into fixed array
static int splitPlayers(const std::string& joined, std::string playersOut[], int maxPlayers) {
    int count = 0;
    std::string cur;

    for (size_t i = 0; i < joined.size(); i++) {
        char c = joined[i];
        if (c == ';') {
            if (!cur.empty() && count < maxPlayers) {
                playersOut[count++] = cur;
            }
            cur.clear();
        }
        else {
            cur += c;
        }
    }

    if (!cur.empty() && count < maxPlayers) {
        playersOut[count++] = cur;
    }

    return count;
}

// ----------------- PlayRecordList -----------------

PlayRecordList::PlayRecordList() : head(nullptr) {}
PlayRecordList::~PlayRecordList() { clear(); }

void PlayRecordList::clear() {
    PlayRecordNode* curr = head;
    while (curr) {
        PlayRecordNode* tmp = curr;
        curr = curr->next;
        delete tmp;
    }
    head = nullptr;
}

void PlayRecordList::append(PlayRecordNode* node) {
    if (!node) return;
    node->next = nullptr;

    if (!head) {
        head = node;
        return;
    }

    PlayRecordNode* curr = head;
    while (curr->next) curr = curr->next;
    curr->next = node;
}

// Save one record to CSV (append-only)
bool PlayRecordList::appendToCSV(const std::string& filename, const PlayRecordNode& r) {
    std::ofstream out(filename, std::ios::app);
    if (!out.is_open()) return false;

    // date,gameName,playerCount,players,winnerID
    out << escapeCSV(r.date) << ","
        << escapeCSV(r.gameName) << ","
        << r.playerCount << ","
        << escapeCSV(joinPlayers(r.players, r.playerCount)) << ","
        << escapeCSV(r.winnerID)
        << "\n";

    return true;
}

// Load records from CSV on startup (so you can view after reopening terminal)
bool PlayRecordList::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "WARNING: Cannot open " << filename << "\n";
        return false;
    }

    clear();

    std::string line;

    // Skip header
    if (!std::getline(file, line)) {
        std::cout << "WARNING: plays.csv is empty\n";
        return false;
    }

    int loaded = 0;
    int skipped = 0;

    while (std::getline(file, line)) {
        trimCR(line);
        if (line.empty()) continue;

        std::string cols[5];
        if (!parseCSV5(line, cols)) {
            skipped++;
            continue;
        }

        // cols: 0=date, 1=gameName, 2=playerCount, 3=players, 4=winnerID
        int pCount = 0;
        try {
            pCount = std::stoi(cols[2]);
        }
        catch (...) {
            skipped++;
            continue;
        }

        // We'll trust the split result more than the number in CSV
        PlayRecordNode* r = new PlayRecordNode();
        r->date = cols[0];
        r->gameName = cols[1];
        r->winnerID = cols[4];

        int actualPlayers = splitPlayers(cols[3], r->players, MAX_PLAYERS_PER_PLAY);
        if (actualPlayers <= 0) {
            delete r;
            skipped++;
            continue;
        }

        // If CSV number is invalid, still accept if split is ok
        if (pCount <= 0 || pCount > MAX_PLAYERS_PER_PLAY) {
            r->playerCount = actualPlayers;
        }
        else {
            // If mismatch, use actual
            r->playerCount = actualPlayers;
        }

        append(r);
        loaded++;
    }

    std::cout << "Loaded " << loaded << " play records from CSV. (Skipped " << skipped << " rows)\n";
    return true;
}

void PlayRecordList::printAll() const {
    std::cout << "\nPlay Records:\n";
    std::cout << "Date       | Game Name | Players | Winner\n";
    std::cout << "------------------------------------------------\n";

    PlayRecordNode* curr = head;
    while (curr) {
        std::cout << curr->date << " | "
            << curr->gameName << " | "
            << curr->playerCount << " players | "
            << curr->winnerID << "\n";
        curr = curr->next;
    }
}

void PlayRecordList::printByMember(const std::string& memberID) const {
    std::cout << "\nPlay History for Member " << memberID << ":\n";
    std::cout << "Date       | Game Name | Winner\n";
    std::cout << "-----------------------------------\n";

    bool found = false;

    PlayRecordNode* curr = head;
    while (curr) {
        bool played = false;
        for (int i = 0; i < curr->playerCount; i++) {
            if (curr->players[i] == memberID) {
                played = true;
                break;
            }
        }

        if (played) {
            found = true;
            std::cout << curr->date << " | "
                << curr->gameName << " | "
                << curr->winnerID << "\n";
        }

        curr = curr->next;
    }

    if (!found) std::cout << "(No records found)\n";
}
