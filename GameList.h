#ifndef GAMELIST_H
#define GAMELIST_H

#include "GameNode.h"
#include <string>

class GameList {
private:
    GameNode* head;

public:
    GameList();
    ~GameList();

    void clear();
    GameNode* getHead() const;

    void append(GameNode* node);

    // Case-sensitive search
    GameNode* findByName(const std::string& gameName);

    // Case-insensitive search - THIS MUST BE HERE!
    GameNode* findByNameCaseInsensitive(const std::string& gameName);

    bool removeByName(const std::string& gameName);

    void printAll(int limit = 30) const;

    bool loadFromCSV(const std::string& filename);
    bool appendGameToCSV(const std::string& filename, const GameNode& g);

    void printGameSummary() const;
};

#endif