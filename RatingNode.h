#pragma once

#ifndef RATINGNODE_H
#define RATINGNODE_H

#include <string>

struct RatingNode {
    std::string memberID;
    std::string gameID;
    int rating; // 1..10
    RatingNode* next;

    RatingNode() : rating(0), next(nullptr) {}
};

#endif
