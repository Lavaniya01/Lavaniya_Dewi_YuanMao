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

    // Case-sensitive (exact match)
    MemberNode* findByID(const std::string& memberID);

    // NEW: Case-insensitive (flexible match)
    MemberNode* findByIDCaseInsensitive(const std::string& memberID);

    // Case-sensitive exists check
    bool exists(const std::string& memberID) const;

    // NEW: Case-insensitive exists check
    bool existsCaseInsensitive(const std::string& memberID) const;

    bool loadFromCSV(const std::string& filename);
    bool appendMemberToCSV(const std::string& filename,
        const std::string& memberID,
        const std::string& name);
};

#endif