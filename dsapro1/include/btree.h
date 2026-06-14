#ifndef BTREE_H
#define BTREE_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// B+ Tree for disk-efficient license indexing (Indexing Registry)
// All records live in leaf nodes; internal nodes only guide search.
// Ideal for disk storage — leaves map 1:1 to file pages.
struct LicenseRecord {
    std::string licenseId;
    std::string title;
    std::string artist;
    std::string publisher;
    double royaltyRate;

    std::string serialize() const {
        return licenseId + "|" + title + "|" + artist + "|" + publisher + "|" +
               std::to_string(royaltyRate);
    }

    static LicenseRecord deserialize(const std::string& line) {
        LicenseRecord rec;
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        rec.licenseId = line.substr(0, p1);
        rec.title = line.substr(p1 + 1, p2 - p1 - 1);
        rec.artist = line.substr(p2 + 1, p3 - p2 - 1);
        rec.publisher = line.substr(p3 + 1, p4 - p3 - 1);
        rec.royaltyRate = std::stod(line.substr(p4 + 1));
        return rec;
    }
};

struct BPlusNode {
    bool leaf;
    std::vector<std::string> keys;
    std::vector<LicenseRecord> records;   // only used in leaf nodes
    std::vector<BPlusNode*> children;     // only used in internal nodes
    BPlusNode* next = nullptr;            // leaf chain for sequential scan

    explicit BPlusNode(bool isLeaf) : leaf(isLeaf) {}
};

class BTree {
    static const int MAX_KEYS = 3;
    BPlusNode* root;
    std::string diskFile;

    BPlusNode* findLeaf(BPlusNode* node, const std::string& key) {
        while (!node->leaf) {
            int i = 0;
            while (i < (int)node->keys.size() && key >= node->keys[i]) i++;
            node = node->children[i];
        }
        return node;
    }

    void destroy(BPlusNode* node) {
        if (!node) return;
        if (!node->leaf)
            for (auto* c : node->children) destroy(c);
        delete node;
    }

    void traverseLeaves(BPlusNode* node, bool saving, std::ofstream* out) const {
        if (!node) return;
        if (node->leaf) {
            for (const auto& r : node->records) {
                if (saving && out)
                    *out << r.serialize() << "\n";
                else {
                    std::cout << "  [" << r.licenseId << "] " << r.title
                              << " - " << r.artist << "\n"
                              << "       Publisher: " << r.publisher
                              << "  |  Royalty: " << (int)(r.royaltyRate * 100) << "%\n";
                }
            }
            return;
        }
        for (auto* child : node->children)
            traverseLeaves(child, saving, out);
    }

    void insertIntoLeaf(BPlusNode* leaf, const LicenseRecord& rec) {
        int i = 0;
        while (i < (int)leaf->records.size() && rec.licenseId > leaf->records[i].licenseId)
            i++;
        leaf->records.insert(leaf->records.begin() + i, rec);
        leaf->keys.insert(leaf->keys.begin() + i, rec.licenseId);
    }

    void splitLeafNode(BPlusNode* leaf, BPlusNode* newLeaf) {
        int mid = (int)leaf->records.size() / 2;
        for (int j = mid; j < (int)leaf->records.size(); j++) {
            newLeaf->records.push_back(leaf->records[j]);
            newLeaf->keys.push_back(leaf->records[j].licenseId);
        }
        leaf->records.resize(mid);
        leaf->keys.resize(mid);
        newLeaf->next = leaf->next;
        leaf->next = newLeaf;
    }

    void insertKeyIntoParent(std::vector<BPlusNode*>& path, BPlusNode* left,
                             const std::string& key, BPlusNode* right) {
        if (path.empty()) {
            auto* newRoot = new BPlusNode(false);
            newRoot->keys.push_back(key);
            newRoot->children.push_back(left);
            newRoot->children.push_back(right);
            root = newRoot;
            return;
        }

        BPlusNode* parent = path.back();
        path.pop_back();
        int idx = -1;
        for (int i = 0; i < (int)parent->children.size(); i++) {
            if (parent->children[i] == left) { idx = i; break; }
        }
        if (idx == -1) return;

        parent->keys.insert(parent->keys.begin() + idx, key);
        parent->children.insert(parent->children.begin() + idx + 1, right);

        if ((int)parent->keys.size() <= MAX_KEYS) return;

        // Split internal node and propagate upward
        auto* newInternal = new BPlusNode(false);
        int mid = (int)parent->keys.size() / 2;
        std::string promote = parent->keys[mid];

        for (int j = mid + 1; j < (int)parent->keys.size(); j++)
            newInternal->keys.push_back(parent->keys[j]);
        for (int j = mid + 1; j < (int)parent->children.size(); j++)
            newInternal->children.push_back(parent->children[j]);

        parent->keys.resize(mid);
        parent->children.resize(mid + 1);

        insertKeyIntoParent(path, parent, promote, newInternal);
    }

public:
    BTree(const std::string& file = "data/licenses.dat")
        : root(new BPlusNode(true)), diskFile(file) {}

    ~BTree() { destroy(root); }

    void insert(const LicenseRecord& rec) {
        std::vector<BPlusNode*> path;
        BPlusNode* node = root;

        while (!node->leaf) {
            path.push_back(node);
            int i = 0;
            while (i < (int)node->keys.size() && rec.licenseId >= node->keys[i]) i++;
            node = node->children[i];
        }

        for (const auto& r : node->records)
            if (r.licenseId == rec.licenseId) return;

        insertIntoLeaf(node, rec);
        if ((int)node->records.size() <= MAX_KEYS) return;

        auto* newLeaf = new BPlusNode(true);
        splitLeafNode(node, newLeaf);
        insertKeyIntoParent(path, node, newLeaf->keys[0], newLeaf);
    }

    LicenseRecord* search(const std::string& licenseId) {
        BPlusNode* leaf = findLeaf(root, licenseId);
        for (auto& r : leaf->records)
            if (r.licenseId == licenseId) return &r;
        return nullptr;
    }

    void displayAll() const {
        std::cout << "\n  +-----------------------------------------------------+\n";
        std::cout << "  |  LICENSE REGISTRY  |  Data Structure: B+ TREE      |\n";
        std::cout << "  +-----------------------------------------------------+\n";
        std::cout << "  Disk-backed index | Operations: insert, search, scan\n\n";
        traverseLeaves(root, false, nullptr);
    }

    void saveToDisk() {
        std::ofstream out(diskFile);
        traverseLeaves(root, true, &out);
        out.close();
        std::cout << "  Registry saved to " << diskFile << "\n";
    }

    void loadFromDisk() {
        std::ifstream in(diskFile);
        if (!in) return;
        destroy(root);
        root = new BPlusNode(true);
        std::string line;
        while (std::getline(in, line)) {
            if (!line.empty()) insert(LicenseRecord::deserialize(line));
        }
        in.close();
        std::cout << "  Registry loaded from " << diskFile << "\n";
    }
};

#endif
