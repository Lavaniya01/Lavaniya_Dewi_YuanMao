#pragma once
#ifndef REVIEWNODE_H
#define REVIEWNODE_H

#include <string>

struct ReviewNode {
    std::string memberID;
    std::string gameName;
    int rating;              // 1-10
    std::string reviewText;  // The actual review content
    std::string date;        // Date when review was written (YYYY-MM-DD)
    
    ReviewNode* next;

    ReviewNode() : rating(0), next(nullptr) {}
};

#endif
