#ifndef PLAYRECORDLIST_H
#define PLAYRECORDLIST_H

#include "PlayRecordNode.h"
#include <string>

class PlayRecordList {
private:
    PlayRecordNode* head;

public:
    PlayRecordList();
    ~PlayRecordList();

    void clear();
    void append(PlayRecordNode* node);

    bool appendToCSV(const std::string& filename, const PlayRecordNode& r);
    void printAll() const;
    void printByMember(const std::string& memberID) const;
    bool loadFromCSV(const std::string& filename);

};

#endif
#pragma once
