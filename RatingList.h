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

    RatingNode* getHead() const;

    void addOrUpdate(const std::string& memberID,
        const std::string& gameID,
        int rating,
        const std::string& date);

    bool loadFromCSV(const std::string& filename);
    bool appendToCSV(const std::string& filename,
        const std::string& memberID,
        const std::string& gameID,
        int rating,
        const std::string& date);

    double getAverage(const std::string& gameID) const;
    int countRatings(const std::string& gameID) const;
};

#endif
