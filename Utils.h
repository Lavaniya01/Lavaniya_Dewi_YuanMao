#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>
#include <limits>

inline void clearInputLine() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

inline bool readInt(const std::string& prompt, int& out) {
    std::cout << prompt;
    if (!(std::cin >> out)) {
        std::cin.clear();
        clearInputLine();
        return false;
    }
    clearInputLine();
    return true;
}

inline std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

#endif
