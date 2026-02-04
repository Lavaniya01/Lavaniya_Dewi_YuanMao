#include "RatingList.h"
#include <fstream>
#include <sstream>
#include <iostream>

RatingList::RatingList() : head(nullptr) {}
RatingList::~RatingList() { clear(); }

void RatingList::clear() {
    RatingNode* curr = head;
    while (curr) {
        RatingNode* tmp = curr;
        curr = curr->next;
        delete tmp;
    }
    head = nullptr;
}

RatingNode* RatingList::getHead() const {
    return head;
}

void RatingList::addOrUpdate(const std::string& memberID,
    const std::string& gameID,
    int rating,
    const std::string& date) {

    RatingNode* curr = head;
    while (curr) {
        if (curr->memberID == memberID && curr->gameID == gameID) {
            curr->rating = rating;
            curr->date = date;
            return;
        }
        curr = curr->next;
    }

    RatingNode* node = new RatingNode();
    node->memberID = memberID;
    node->gameID = gameID;
    node->rating = rating;
    node->date = date;

    node->next = head;
    head = node;
}

double RatingList::getAverage(const std::string& gameID) const {
    int sum = 0;
    int count = 0;

    RatingNode* curr = head;
    while (curr) {
        if (curr->gameID == gameID) {
            sum += curr->rating;
            count++;
        }
        curr = curr->next;
    }

    if (count == 0) return -1.0;
    return static_cast<double>(sum) / static_cast<double>(count);
}

int RatingList::countRatings(const std::string& gameID) const {
    int count = 0;
    RatingNode* curr = head;
    while (curr) {
        if (curr->gameID == gameID) count++;
        curr = curr->next;
    }
    return count;
}

// ---------- CSV LOAD ----------
bool RatingList::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "No ratings file found. Starting fresh.\n";
        return false;
    }

    clear();

    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string memberID, gameID, ratingStr, date;

        std::getline(ss, memberID, ',');
        std::getline(ss, gameID, ',');
        std::getline(ss, ratingStr, ',');
        std::getline(ss, date);

        if (memberID.empty() || gameID.empty() || ratingStr.empty() || date.empty()) continue;

        int rating = 0;
        try {
            rating = std::stoi(ratingStr);
        }
        catch (...) {
            continue;
        }

        addOrUpdate(memberID, gameID, rating, date);
    }

    return true;
}

// ---------- CSV APPEND ----------
bool RatingList::saveOrUpdateCSV(const std::string& filename,
    const std::string& memberID,
    const std::string& gameID,
    int rating,
    const std::string& date) {

    std::ifstream in(filename);
    if (!in.is_open()) {
        // file doesn't exist yet -> create and write header + row
        std::ofstream out(filename);
        if (!out.is_open()) return false;

        out << "memberid,gameid,rating,date\n";
        out << memberID << "," << gameID << "," << rating << "," << date << "\n";
        return true;
    }

    const int MAX_LINES = 20000;
    std::string lines[MAX_LINES];
    int lineCount = 0;

    std::string line;
    bool updated = false;

    // read header (keep it)
    if (std::getline(in, line)) {
        if (line.empty()) line = "memberid,gameid,rating,date";
        lines[lineCount++] = line;
    }
    else {
        // empty file: treat as missing header
        lines[lineCount++] = "memberid,gameid,rating,date";
    }

    while (std::getline(in, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string mid, gid, rStr, dStr;

        std::getline(ss, mid, ',');
        std::getline(ss, gid, ',');
        std::getline(ss, rStr, ',');
        std::getline(ss, dStr);

        if (mid == memberID && gid == gameID) {
            // replace this row with new rating/date
            std::ostringstream os;
            os << memberID << "," << gameID << "," << rating << "," << date;
            lines[lineCount++] = os.str();
            updated = true;
        }
        else {
            lines[lineCount++] = line;
        }

        if (lineCount >= MAX_LINES) break; // safety
    }
    in.close();

    // if not found -> append new line
    if (!updated && lineCount < MAX_LINES) {
        std::ostringstream os;
        os << memberID << "," << gameID << "," << rating << "," << date;
        lines[lineCount++] = os.str();
    }

    // rewrite full file
    std::ofstream out(filename);
    if (!out.is_open()) return false;

    for (int i = 0; i < lineCount; i++) {
        out << lines[i] << "\n";
    }

    return true;
}

