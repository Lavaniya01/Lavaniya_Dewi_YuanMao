#include "MemberList.h"
#include "MemberNode.h"   
#include <fstream>
#include <iostream>
#include <cctype>

// ---------- Helper Function ----------
static std::string toLower(const std::string& s) {
    std::string out = s;
    for (size_t i = 0; i < out.size(); i++) {
        out[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(out[i])));
    }
    return out;
}

// ---------- MemberList Implementation ----------

bool MemberList::appendMemberToCSV(const std::string& filename,
    const std::string& memberID,
    const std::string& name) {
    std::ofstream out(filename, std::ios::app);
    if (!out.is_open()) return false;

    // Simple CSV: memberid,name (names should not include commas in our test data)
    out << memberID << "," << name << "\n";
    return true;
}

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

// Case-SENSITIVE search (exact match)
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

// NEW: Case-INSENSITIVE search (flexible match)
// "m001" will match "M001", "M001" will match "m001"
MemberNode* MemberList::findByIDCaseInsensitive(const std::string& memberID) {
    std::string searchLower = toLower(memberID);

    MemberNode* curr = head;
    while (curr != nullptr) {
        std::string currIdLower = toLower(curr->memberID);
        if (currIdLower == searchLower) {
            return curr;
        }
        curr = curr->next;
    }
    return nullptr;
}

// Load members from CSV file
bool MemberList::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "WARNING: Cannot open " << filename << "\n";
        return false;
    }

    // Optional: clear existing members
    clear();

    std::string line;

    // Skip header line
    if (!std::getline(file, line)) {
        std::cout << "WARNING: members.csv is empty\n";
        return false;
    }

    int loaded = 0;
    int skipped = 0;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Simple split: memberid,name
        size_t commaPos = line.find(',');
        if (commaPos == std::string::npos) {
            skipped++;
            continue;
        }

        std::string id = line.substr(0, commaPos);
        std::string name = line.substr(commaPos + 1);

        if (id.empty() || name.empty()) {
            skipped++;
            continue;
        }

        // Avoid duplicate members (case-insensitive check)
        if (existsCaseInsensitive(id)) {
            skipped++;
            continue;
        }

        MemberNode* m = new MemberNode();
        m->memberID = id;
        m->name = name;
        m->next = nullptr;

        append(m);
        loaded++;
    }

    std::cout << "Loaded " << loaded << " members from CSV. "
        << "(Skipped " << skipped << " rows)\n";

    return true;
}

// Case-SENSITIVE exists check
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

// NEW: Case-INSENSITIVE exists check
bool MemberList::existsCaseInsensitive(const std::string& memberID) const {
    std::string searchLower = toLower(memberID);

    MemberNode* curr = head;
    while (curr != nullptr) {
        std::string currIdLower = toLower(curr->memberID);
        if (currIdLower == searchLower) {
            return true;
        }
        curr = curr->next;
    }
    return false;
}