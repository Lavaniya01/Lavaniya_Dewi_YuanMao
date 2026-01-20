#ifndef MEMBERNODE_H
#define MEMBERNODE_H

#include <string>

struct MemberNode {
    std::string memberID;
    std::string name;
    MemberNode* next;

    MemberNode() : next(nullptr) {}
};

#endif
