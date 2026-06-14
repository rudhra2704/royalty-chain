#ifndef OPTIONS_HEAP_H
#define OPTIONS_HEAP_H

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// Max-heap to rank media trades by transaction volume (Options Sorter)
struct TradeOption {
    std::string trackTitle;
    std::string artist;
    int transactionVolume;
    double avgPrice;

    bool operator<(const TradeOption& other) const {
        return transactionVolume < other.transactionVolume;
    }

    bool operator>(const TradeOption& other) const {
        return transactionVolume > other.transactionVolume;
    }
};

class OptionsHeap {
    std::vector<TradeOption> heap;

    void heapifyUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (heap[idx] < heap[parent]) break;
            std::swap(heap[idx], heap[parent]);
            idx = parent;
        }
    }

    void heapifyDown(int idx) {
        int size = (int)heap.size();
        while (true) {
            int largest = idx;
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;
            if (left < size && heap[left] > heap[largest]) largest = left;
            if (right < size && heap[right] > heap[largest]) largest = right;
            if (largest == idx) break;
            std::swap(heap[idx], heap[largest]);
            idx = largest;
        }
    }

public:
    void insert(const TradeOption& opt) {
        heap.push_back(opt);
        heapifyUp((int)heap.size() - 1);
    }

    TradeOption extractMax() {
        if (heap.empty()) return {"", "", 0, 0.0};
        TradeOption maxVal = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) heapifyDown(0);
        return maxVal;
    }

    static std::string formatVolume(int vol) {
        std::string s = std::to_string(vol);
        std::string out;
        int count = 0;
        for (int i = (int)s.size() - 1; i >= 0; i--) {
            if (count && count % 3 == 0) out = ',' + out;
            out = s[i] + out;
            count++;
        }
        return out;
    }

    static std::string volumeTier(int vol) {
        if (vol >= 5000000) return "PLATINUM";
        if (vol >= 2000000) return "GOLD";
        if (vol >= 500000)  return "SILVER";
        return "CATALOG";
    }

    void displayRanked() const {
        std::cout << "\n  +-----------------------------------------------------+\n";
        std::cout << "  |  TRADE RANKINGS  |  Data Structure: MAX-HEAP       |\n";
        std::cout << "  +-----------------------------------------------------+\n";
        std::cout << "  Sorted by transaction volume (highest = root)\n\n";
        std::vector<TradeOption> temp = heap;
        std::vector<TradeOption> sorted;
        OptionsHeap copy;
        copy.heap = temp;
        while (!copy.heap.empty())
            sorted.push_back(copy.extractMax());

        for (size_t i = 0; i < sorted.size(); i++) {
            std::cout << "  #" << std::setw(2) << (i + 1) << " ["
                      << volumeTier(sorted[i].transactionVolume) << "] "
                      << sorted[i].trackTitle << " - " << sorted[i].artist << "\n"
                      << "       Vol: " << formatVolume(sorted[i].transactionVolume)
                      << " streams  |  Avg: $" << std::fixed
                      << std::setprecision(2) << sorted[i].avgPrice << "\n";
        }
        if (!sorted.empty()) {
            std::cout << "\n  Range: " << formatVolume(sorted.back().transactionVolume)
                      << " (low) -> " << formatVolume(sorted.front().transactionVolume)
                      << " (high)\n";
        }
    }

    size_t size() const { return heap.size(); }
};

#endif
