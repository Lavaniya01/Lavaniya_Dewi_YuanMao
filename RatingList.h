#pragma once
#ifndef RATINGLIST_H
#define RATINGLIST_H

#include "RatingNode.h"
#include <string>

class RatingList {
private:
    RatingNode* head;

public:
    RatingList();
    ~RatingList();

    void clear();

    void addOrUpdate(const std::string& memberID, const std::string& gameID, int rating);

    // returns -1 if no ratings
    double getAverage(const std::string& gameID) const;

    int countRatings(const std::string& gameID) const;
};

#endif
