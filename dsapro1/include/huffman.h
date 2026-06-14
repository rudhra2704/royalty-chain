#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <iomanip>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

// Huffman coding for compressing asset descriptions (Storage Divider)
struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode *left, *right;

    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

class HuffmanEncoder {
    std::unordered_map<char, std::string> codeMap;

    void buildCodes(HuffmanNode* root, const std::string& code) {
        if (!root) return;
        if (!root->left && !root->right) {
            codeMap[root->ch] = code.empty() ? "0" : code;
            return;
        }
        buildCodes(root->left, code + "0");
        buildCodes(root->right, code + "1");
    }

    void freeTree(HuffmanNode* node) {
        if (!node) return;
        freeTree(node->left);
        freeTree(node->right);
        delete node;
    }

public:
    std::string encode(const std::string& text) {
        if (text.empty()) return "";

        std::unordered_map<char, int> freq;
        for (char c : text) freq[c]++;

        std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> pq;
        for (auto& [ch, f] : freq)
            pq.push(new HuffmanNode(ch, f));

        while (pq.size() > 1) {
            HuffmanNode* left = pq.top(); pq.pop();
            HuffmanNode* right = pq.top(); pq.pop();
            auto* parent = new HuffmanNode('\0', left->freq + right->freq);
            parent->left = left;
            parent->right = right;
            pq.push(parent);
        }

        HuffmanNode* root = pq.top();
        codeMap.clear();
        buildCodes(root, "");

        std::string encoded;
        for (char c : text) encoded += codeMap[c];

        freeTree(root);
        return encoded;
    }

    void displayCodes(const std::string& text) {
        encode(text);
        std::cout << "\n  +-----------------------------------------------------+\n";
        std::cout << "  |  COMPRESSION  |  Algorithm: HUFFMAN CODING        |\n";
        std::cout << "  +-----------------------------------------------------+\n";
        std::cout << "  Input: \"" << text << "\"\n";
        std::cout << "  Original size: " << text.size() * 8 << " bits (8 bits/char)\n\n";
        std::cout << "  Character codes:\n";
        for (auto& [ch, code] : codeMap)
            std::cout << "    '" << ch << "' -> " << code << "\n";

        std::string compressed = "";
        for (char c : text) compressed += codeMap[c];
        std::cout << "\n  Compressed: " << compressed.size() << " bits\n";
        double ratio = 100.0 * (1.0 - (double)compressed.size() / (text.size() * 8));
        std::cout << "  Space saved: " << std::fixed << std::setprecision(1) << ratio << "%\n";
    }
};

#endif
