#ifndef GAMENODE_H
#define GAMENODE_H

#include <string>

struct GameNode {
    // CSV: name
    std::string gameName;

    // CSV: gameId
    std::string gameId;

    // CSV: minplayers, maxplayers
    int minPlayers;
    int maxPlayers;

    // CSV: minplaytime, maxplaytime
    int minPlaytime;
    int maxPlaytime;

    // CSV: yearpublished
    int yearPublished;

    // CSV: category
    std::string category;

    // System fields
    // 'A' = Available, 'B' = Borrowed
    char status;
    std::string borrowedBy;

    GameNode* next;

    GameNode()
        : minPlayers(0),
        maxPlayers(0),
        minPlaytime(0),
        maxPlaytime(0),
        yearPublished(0),
        status('A'),
        next(nullptr) {}
};

#endif
