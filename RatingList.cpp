#include "RatingList.h"

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

void RatingList::addOrUpdate(const std::string& memberID, const std::string& gameID, int rating) {
    RatingNode* curr = head;
    while (curr) {
        if (curr->memberID == memberID && curr->gameID == gameID) {
            curr->rating = rating;
            return;
        }
        curr = curr->next;
    }

    RatingNode* node = new RatingNode();
    node->memberID = memberID;
    node->gameID = gameID;
    node->rating = rating;

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
