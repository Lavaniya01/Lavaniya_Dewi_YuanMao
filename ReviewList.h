#pragma once
#ifndef REVIEWLIST_H
#define REVIEWLIST_H

#include "ReviewNode.h"
#include <string>

class ReviewList {
private:
    ReviewNode* head;

public:
    ReviewList();
    ~ReviewList();

    void clear();

    // Add or update a review (if member already reviewed the game, update it)
    void addOrUpdate(const std::string& memberID, 
                     const std::string& gameName, 
                     int rating,
                     const std::string& reviewText,
                     const std::string& date);

    // Get all reviews for a specific game
    void printReviewsForGame(const std::string& gameName) const;

    // Count how many reviews a game has
    int countReviews(const std::string& gameName) const;

    // Calculate average rating for a game (returns -1 if no reviews)
    double getAverage(const std::string& gameName) const;

    // Check if a member has already reviewed a game
    bool hasReviewed(const std::string& memberID, const std::string& gameName) const;
};

#endif
