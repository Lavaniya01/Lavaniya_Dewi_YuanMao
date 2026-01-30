#ifndef MEMBERLIST_H
#define MEMBERLIST_H

#include <string>

struct MemberNode;

class MemberList {
private:
    MemberNode* head;

public:
    MemberList();
    ~MemberList();

    void clear();
    void append(MemberNode* node);

    MemberNode* findByID(const std::string& memberID);
    bool exists(const std::string& memberID) const;
    bool loadFromCSV(const std::string& filename);

};

#endif
