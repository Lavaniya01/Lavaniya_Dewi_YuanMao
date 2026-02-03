#include "PlayRecordList.h"
#include <iostream>
#include <fstream>

// Very simple CSV quote escape (no STL containers)
static std::string escapeCSV(const std::string& s) {
    bool needQuotes = false;
    for (int i = 0; i < (int)s.size(); i++)
        if (s[i] == ',' || s[i] == '"' || s[i] == '\n') needQuotes = true;
    if (!needQuotes) return s;

    std::string out = "\"";
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == '"') out += "\"\"";
        else out += s[i];
    }
    out += "\"";
    return out;
}

// Join players with semicolon
static std::string joinPlayers(const std::string players[], int count) {
    std::string out;
    for (int i = 0; i < count; i++) {
        out += players[i];
        if (i < count - 1) out += ";";
    }
    return out;
}

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
    if (!head) { head = node; return; }
    PlayRecordNode* curr = head;
    while (curr->next) curr = curr->next;
    curr->next = node;
}

bool PlayRecordList::appendToCSV(const std::string& filename, const PlayRecordNode& r) {
    std::ofstream out(filename, std::ios::app);
    if (!out.is_open()) return false;

    out << escapeCSV(r.date) << ","
        << escapeCSV(r.gameName) << ","
        << r.playerCount << ","
        << escapeCSV(joinPlayers(r.players, r.playerCount)) << ","
        << escapeCSV(r.winnerID)
        << "\n";

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
