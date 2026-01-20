#include "MemberList.h"
#include "MemberNode.h"   // ✅ MUST be included here for full definition

MemberList::MemberList() : head(nullptr) {}

MemberList::~MemberList() {
    clear();
}

void MemberList::clear() {
    MemberNode* curr = head;
    while (curr != nullptr) {
        MemberNode* temp = curr;
        curr = curr->next;
        delete temp;
    }
    head = nullptr;
}

void MemberList::append(MemberNode* node) {
    if (node == nullptr) return;

    node->next = nullptr;

    if (head == nullptr) {
        head = node;
        return;
    }

    MemberNode* curr = head;
    while (curr->next != nullptr) {
        curr = curr->next;
    }
    curr->next = node;
}

MemberNode* MemberList::findByID(const std::string& memberID) {
    MemberNode* curr = head;
    while (curr != nullptr) {
        if (curr->memberID == memberID) {
            return curr;
        }
        curr = curr->next;
    }
    return nullptr;
}

bool MemberList::exists(const std::string& memberID) const {
    MemberNode* curr = head;
    while (curr != nullptr) {
        if (curr->memberID == memberID) {
            return true;
        }
        curr = curr->next;
    }
    return false;
}
