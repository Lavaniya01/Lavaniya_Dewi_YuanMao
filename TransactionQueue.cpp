#include "TransactionQueue.h"
#include <iostream>

TransactionQueue::TransactionQueue() : front(nullptr), rear(nullptr) {}
TransactionQueue::~TransactionQueue() { clear(); }

void TransactionQueue::clear() {
    TransactionNode* curr = front;
    while (curr) {
        TransactionNode* tmp = curr;
        curr = curr->next;
        delete tmp;
    }
    front = rear = nullptr;
}

void TransactionQueue::enqueue(const std::string& memberID,
    const std::string& gameID,
    const std::string& title,
    const std::string& action,
    const std::string& date) {
    TransactionNode* node = new TransactionNode();
    node->memberID = memberID;
    node->gameID = gameID;
    node->title = title;
    node->action = action;
    node->date = date;
    node->next = nullptr;

    if (!rear) {
        front = rear = node;
    }
    else {
        rear->next = node;
        rear = node;
    }
}

void TransactionQueue::printAll() const {
    std::cout << "No\tDate\t\tMemberID\tAction\tGameID\tTitle\n";
    std::cout << "---------------------------------------------------------------\n";
    int i = 1;

    TransactionNode* curr = front;
    if (!curr) {
        std::cout << "(No transactions)\n";
        return;
    }

    while (curr) {
        std::cout << i << "\t" << curr->date << "\t"
            << curr->memberID << "\t\t"
            << curr->action << "\t"
            << curr->gameID << "\t"
            << curr->title << "\n";
        curr = curr->next;
        i++;
    }
}

void TransactionQueue::printByMember(const std::string& memberID) const {
    std::cout << "No\tDate\t\tAction\tGameID\tTitle\n";
    std::cout << "-------------------------------------------------\n";

    TransactionNode* curr = front;
    int i = 1;
    bool any = false;

    while (curr) {
        if (curr->memberID == memberID) {
            any = true;
            std::cout << i << "\t" << curr->date << "\t"
                << curr->action << "\t"
                << curr->gameID << "\t"
                << curr->title << "\n";
            i++;
        }
        curr = curr->next;
    }

    if (!any) {
        std::cout << "(No records for this member)\n";
    }
}
