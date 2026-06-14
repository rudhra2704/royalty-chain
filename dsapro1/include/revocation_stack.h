#ifndef REVOCATION_STACK_H
#define REVOCATION_STACK_H

#include <iostream>
#include <stack>
#include <string>
#include <vector>

// Stack-based revocation log with rollback support (Revocation Log)
struct RevocationEntry {
    std::string clearanceId;
    std::string authority;
    std::string action;
    std::string timestamp;
    std::string licenseId;
};

class RevocationLog {
    std::stack<RevocationEntry> log;
    std::vector<RevocationEntry> history;

public:
    void push(const RevocationEntry& entry, bool quiet = false) {
        log.push(entry);
        history.push_back(entry);
        if (!quiet)
            std::cout << "  Logged: " << entry.action << " by " << entry.authority
                      << " on " << entry.licenseId << "\n";
    }

    bool rollback() {
        if (log.empty()) {
            std::cout << "  Nothing to rollback.\n";
            return false;
        }
        RevocationEntry top = log.top();
        log.pop();
        std::cout << "  Rolled back: " << top.action << " (" << top.clearanceId << ")\n";
        return true;
    }

    void display() const {
        std::cout << "\n  +-----------------------------------------------------+\n";
        std::cout << "  |  REVOCATION LOG  |  Data Structure: STACK (LIFO)  |\n";
        std::cout << "  +-----------------------------------------------------+\n";
        std::cout << "  Operations: push() = log clearance | rollback() = pop()\n\n";
        if (log.empty()) {
            std::cout << "  (empty)\n";
            return;
        }
        std::stack<RevocationEntry> temp = log;
        int i = 1;
        while (!temp.empty()) {
            const auto& e = temp.top();
            std::cout << "  " << i++ << ". [" << e.clearanceId << "] "
                      << e.action << " - " << e.authority
                      << " -> " << e.licenseId << " (" << e.timestamp << ")\n";
            temp.pop();
        }
    }

    void displayFullHistory() const {
        std::cout << "\n--- Full Audit Trail ---\n";
        for (size_t i = 0; i < history.size(); i++) {
            const auto& e = history[i];
            std::cout << "  " << (i + 1) << ". " << e.action << " by "
                      << e.authority << " on " << e.licenseId << "\n";
        }
    }

    size_t size() const { return log.size(); }
};

#endif
