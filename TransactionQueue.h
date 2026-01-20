#pragma once
#ifndef TRANSACTIONQUEUE_H
#define TRANSACTIONQUEUE_H

#include "TransactionNode.h"
#include <string>

class TransactionQueue {
private:
    TransactionNode* front;
    TransactionNode* rear;

public:
    TransactionQueue();
    ~TransactionQueue();

    void clear();

    void enqueue(const std::string& memberID,
        const std::string& gameID,
        const std::string& title,
        const std::string& action,
        const std::string& date);

    void printAll() const;
    void printByMember(const std::string& memberID) const;
};

#endif
