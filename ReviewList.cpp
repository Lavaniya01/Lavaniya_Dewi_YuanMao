#include "ReviewList.h"
#include <iostream>
#include <iomanip>

ReviewList::ReviewList() : head(nullptr) {}

ReviewList::~ReviewList() { 
    clear(); 
}

void ReviewList::clear() {
    ReviewNode* curr = head;
    while (curr) {
        ReviewNode* tmp = curr;
        curr = curr->next;
        delete tmp;
    }
    head = nullptr;
}

void ReviewList::addOrUpdate(const std::string& memberID, 
                             const std::string& gameName, 
                             int rating,
                             const std::string& reviewText,
                             const std::string& date) {
    // Check if this member already reviewed this game
    ReviewNode* curr = head;
    while (curr) {
        if (curr->memberID == memberID && curr->gameName == gameName) {
            // Update existing review
            curr->rating = rating;
            curr->reviewText = reviewText;
            curr->date = date;
            return;
        }
        curr = curr->next;
    }

    // Add new review
    ReviewNode* node = new ReviewNode();
    node->memberID = memberID;
    node->gameName = gameName;
    node->rating = rating;
    node->reviewText = reviewText;
    node->date = date;

    node->next = head;
    head = node;
}

void ReviewList::printReviewsForGame(const std::string& gameName) const {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "  Reviews for: " << gameName << "\n";
    std::cout << "========================================\n";

    ReviewNode* curr = head;
    int count = 0;
    double totalRating = 0.0;

    // First pass: count and calculate average
    ReviewNode* temp = head;
    while (temp) {
        if (temp->gameName == gameName) {
            count++;
            totalRating += temp->rating;
        }
        temp = temp->next;
    }

    if (count == 0) {
        std::cout << "(No reviews yet for this game)\n";
        std::cout << "========================================\n";
        return;
    }

    double avgRating = totalRating / count;
    std::cout << "Average Rating: " << std::fixed << std::setprecision(1) 
              << avgRating << "/10 (based on " << count << " review" 
              << (count > 1 ? "s" : "") << ")\n";
    std::cout << "========================================\n\n";

    // Second pass: display all reviews
    int reviewNum = 1;
    while (curr) {
        if (curr->gameName == gameName) {
            std::cout << "Review #" << reviewNum << "\n";
            std::cout << "----------------------------------------\n";
            std::cout << "Member ID: " << curr->memberID << "\n";
            std::cout << "Rating: " << curr->rating << "/10\n";
            std::cout << "Date: " << curr->date << "\n";
            std::cout << "Review:\n";
            std::cout << curr->reviewText << "\n";
            std::cout << "----------------------------------------\n\n";
            reviewNum++;
        }
        curr = curr->next;
    }
}

int ReviewList::countReviews(const std::string& gameName) const {
    int count = 0;
    ReviewNode* curr = head;
    while (curr) {
        if (curr->gameName == gameName) {
            count++;
        }
        curr = curr->next;
    }
    return count;
}

double ReviewList::getAverage(const std::string& gameName) const {
    int sum = 0;
    int count = 0;

    ReviewNode* curr = head;
    while (curr) {
        if (curr->gameName == gameName) {
            sum += curr->rating;
            count++;
        }
        curr = curr->next;
    }

    if (count == 0) return -1.0;
    return static_cast<double>(sum) / static_cast<double>(count);
}

bool ReviewList::hasReviewed(const std::string& memberID, const std::string& gameName) const {
    ReviewNode* curr = head;
    while (curr) {
        if (curr->memberID == memberID && curr->gameName == gameName) {
            return true;
        }
        curr = curr->next;
    }
    return false;
}
