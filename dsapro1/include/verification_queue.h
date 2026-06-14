#ifndef VERIFICATION_QUEUE_H
#define VERIFICATION_QUEUE_H

#include <iostream>
#include <queue>
#include <string>

// FIFO queue for processing copyright transfers in submission order (Verification Loop)
struct TransferRequest {
    std::string requestId;
    std::string fromAccount;
    std::string toAccount;
    std::string licenseId;
    std::string trackTitle;
    std::string status;

    TransferRequest(const std::string& id, const std::string& from,
                    const std::string& to, const std::string& lic,
                    const std::string& title)
        : requestId(id), fromAccount(from), toAccount(to),
          licenseId(lic), trackTitle(title), status("PENDING") {}
};

class VerificationQueue {
    std::queue<TransferRequest> pending;
    int processedCount = 0;

public:
    void enqueue(const TransferRequest& req, bool quiet = false) {
        pending.push(req);
        if (!quiet)
            std::cout << "  Queued: " << req.requestId << " ("
                      << req.trackTitle << ")\n";
    }

    bool processNext() {
        if (pending.empty()) {
            std::cout << "  No pending transfers.\n";
            return false;
        }
        TransferRequest req = pending.front();
        pending.pop();
        req.status = "VERIFIED";
        processedCount++;
        std::cout << "  Processed: " << req.requestId << " - "
                  << req.trackTitle << " transferred from "
                  << req.fromAccount << " to " << req.toAccount << "\n";
        return true;
    }

    void processAll() {
        std::cout << "\n--- Processing Transfer Queue ---\n";
        while (!pending.empty()) processNext();
        std::cout << "  Total processed: " << processedCount << "\n";
    }

    void display() const {
        std::cout << "\n  +-----------------------------------------------------+\n";
        std::cout << "  |  TRANSFER QUEUE  |  Data Structure: QUEUE (FIFO)   |\n";
        std::cout << "  +-----------------------------------------------------+\n";
        std::cout << "  Operations: enqueue() = add back | processNext() = serve front\n\n";
        if (pending.empty()) {
            std::cout << "  (empty)\n";
            return;
        }
        std::queue<TransferRequest> temp = pending;
        int i = 1;
        while (!temp.empty()) {
            const auto& r = temp.front();
            std::cout << "  " << i++ << ". [" << r.requestId << "] "
                      << r.trackTitle << ": " << r.fromAccount
                      << " -> " << r.toAccount << " (" << r.status << ")\n";
            temp.pop();
        }
    }

    size_t size() const { return pending.size(); }
};

#endif
