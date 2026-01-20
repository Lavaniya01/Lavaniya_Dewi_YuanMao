#pragma once
#ifndef TRANSACTIONNODE_H
#define TRANSACTIONNODE_H

#include <string>

struct TransactionNode {
    std::string memberID;
    std::string gameID;
    std::string title;
    std::string action; // "BORROW" or "RETURN"
    std::string date;   // YYYY-MM-DD

    TransactionNode* next;

    TransactionNode() : next(nullptr) {}
};

#endif
