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

    // Find/remove by Game Name (because CSV has no numeric ID)
    GameNode* findByName(const std::string& gameName);
    bool removeByName(const std::string& gameName);

    void printAll(int limit = 30) const; // nicer: limit output

    bool loadFromCSV(const std::string& filename);

    void printGameSummary() const;
};

#endif
