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

void RatingList::addOrUpdate(const std::string& memberID,
    const std::string& gameID,
    int rating,
    const std::string& date) {
    RatingNode* curr = head;
    while (curr) {
        if (curr->memberID == memberID &&
            curr->gameID == gameID) {
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
    return static_cast<double>(sum) / count;
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

        int rating = std::stoi(ratingStr);
        addOrUpdate(memberID, gameID, rating, date);
    }

    return true;
}

// ---------- CSV APPEND ----------
bool RatingList::appendToCSV(const std::string& filename,
    const std::string& memberID,
    const std::string& gameID,
    int rating,
    const std::string& date) {
    std::ofstream out(filename, std::ios::app);
    if (!out.is_open()) return false;

    out << memberID << ","
        << gameID << ","
        << rating << ","
        << date << "\n";

    return true;
}
