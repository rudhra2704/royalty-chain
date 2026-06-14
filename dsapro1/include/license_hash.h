#ifndef LICENSE_HASH_H
#define LICENSE_HASH_H

#include <iostream>
#include <string>
#include <vector>

// Hash table for O(1) licensing key lookup (Tracking Ledger)
struct LicenseKey {
    std::string accountId;
    std::string publicKey;
    std::string artistName;
    bool active;
};

class LicenseHashTable {
    static const int TABLE_SIZE = 53;
    std::vector<std::vector<LicenseKey>> table;

    int hash(const std::string& key) const {
        unsigned long hashVal = 0;
        for (char c : key) hashVal = hashVal * 31 + c;
        return hashVal % TABLE_SIZE;
    }

public:
    LicenseHashTable() : table(TABLE_SIZE) {}

    void insert(const LicenseKey& key) {
        int idx = hash(key.accountId);
        for (auto& k : table[idx]) {
            if (k.accountId == key.accountId) {
                k = key;
                return;
            }
        }
        table[idx].push_back(key);
    }

    LicenseKey* lookup(const std::string& accountId) {
        int idx = hash(accountId);
        for (auto& k : table[idx])
            if (k.accountId == accountId) return &k;
        return nullptr;
    }

    bool verify(const std::string& accountId) {
        LicenseKey* key = lookup(accountId);
        if (!key) {
            std::cout << "  Account " << accountId << " NOT FOUND.\n";
            return false;
        }
        std::cout << "  Account " << accountId << " (" << key->artistName
                  << ") - Key: " << key->publicKey
                  << " [" << (key->active ? "ACTIVE" : "REVOKED") << "]\n";
        return key->active;
    }

    void displayAll() const {
        std::cout << "\n  +-----------------------------------------------------+\n";
        std::cout << "  |  KEY LEDGER  |  Data Structure: HASH TABLE          |\n";
        std::cout << "  +-----------------------------------------------------+\n";
        std::cout << "  Chaining collision handling | O(1) average lookup\n\n";
        int count = 0, active = 0, revoked = 0;
        for (int i = 0; i < TABLE_SIZE; i++) {
            for (const auto& k : table[i]) {
                const char* status = k.active ? "ACTIVE" : "REVOKED";
                std::cout << "  [" << k.accountId << "] " << k.artistName << "\n"
                          << "       Key: " << k.publicKey << "  |  " << status << "\n";
                count++;
                if (k.active) active++; else revoked++;
            }
        }
        std::cout << "\n  Total: " << count << " accounts  |  "
                  << active << " active, " << revoked << " revoked\n";
    }

    void revoke(const std::string& accountId) {
        LicenseKey* key = lookup(accountId);
        if (key) {
            key->active = false;
            std::cout << "  Revoked key for " << accountId << "\n";
        }
    }
};

#endif
