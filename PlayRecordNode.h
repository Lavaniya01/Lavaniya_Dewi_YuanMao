#ifndef PLAYRECORDNODE_H
#define PLAYRECORDNODE_H

#include <string>

const int MAX_PLAYERS_PER_PLAY = 10;

struct PlayRecordNode {
    std::string date;        // YYYY-MM-DD
    std::string gameName;    // name matching your games
    int playerCount;
    std::string players[MAX_PLAYERS_PER_PLAY];   // MemberIDs
    std::string winnerID;

    PlayRecordNode* next;

    PlayRecordNode() : playerCount(0), next(nullptr) {}
};

#endif
#pragma once
