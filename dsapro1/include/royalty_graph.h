#ifndef ROYALTY_GRAPH_H
#define ROYALTY_GRAPH_H

#include <algorithm>
#include <cctype>
#include <climits>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

// Graph for royalty distribution channels + Dijkstra routing (Royalty Channels & Routing)
struct Edge {
    std::string to;
    double cost;
};

class RoyaltyGraph {
    std::map<std::string, std::vector<Edge>> adj;
    std::vector<std::string> nodes;

    static std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }

    static std::string toLower(const std::string& s) {
        std::string out = s;
        for (char& c : out) c = (char)std::tolower((unsigned char)c);
        return out;
    }

    int nodeIndex(const std::string& name) const {
        for (size_t i = 0; i < nodes.size(); i++)
            if (nodes[i] == name) return (int)i;
        return -1;
    }

    std::string resolveNode(const std::string& input) const {
        std::string cleaned = trim(input);
        if (cleaned.empty()) return "";

        if (nodeIndex(cleaned) != -1) return cleaned;

        std::string lower = toLower(cleaned);
        for (const auto& n : nodes) {
            if (toLower(n) == lower) return n;
        }

        // Allow first-name shorthand: "drake" -> "Drake", "travis" -> "Travis Scott"
        for (const auto& n : nodes) {
            std::string nLower = toLower(n);
            if (nLower.find(lower) == 0)
                return n;
        }

        return "";
    }

    void printKnownNodes() const {
        std::cout << "  Known nodes:\n";
        std::cout << "    Platforms: Spotify, Apple Music, YouTube Music, Amazon Music\n";
        std::cout << "    Artists:   Drake, Kendrick Lamar, J. Cole, JID, ASAP Rocky,\n";
        std::cout << "               Travis Scott, Frank Ocean, Eazy-E\n";
    }

public:
    void addNode(const std::string& name) {
        if (nodeIndex(name) == -1) nodes.push_back(name);
    }

    void addEdge(const std::string& from, const std::string& to, double cost) {
        addNode(from);
        addNode(to);
        adj[from].push_back({to, cost});
    }

    void displayChannels() const {
        std::cout << "\n  +-----------------------------------------------------+\n";
        std::cout << "  |  ROYALTY CHANNELS  |  Data Structure: GRAPH        |\n";
        std::cout << "  +-----------------------------------------------------+\n";
        std::cout << "  Weighted directed edges = distribution fee per hop\n\n";
        for (const auto& [from, edges] : adj) {
            std::cout << "  " << from << "\n";
            for (const auto& e : edges)
                std::cout << "    --> " << e.to << "  (fee: $" << e.cost << ")\n";
        }
    }

    // Dijkstra's algorithm for minimum-cost royalty routing
    std::vector<std::string> findCheapestRoute(const std::string& source,
                                                const std::string& dest) {
        std::string srcName = resolveNode(source);
        std::string dstName = resolveNode(dest);

        if (srcName.empty() || dstName.empty()) {
            std::cout << "\n  Could not match input to a graph node.\n";
            if (srcName.empty())
                std::cout << "  >> Unknown source: \"" << trim(source) << "\"\n";
            if (dstName.empty())
                std::cout << "  >> Unknown destination: \"" << trim(dest) << "\"\n";
            printKnownNodes();
            std::cout << "  Tip: names are case-insensitive (spotify / drake works)\n";
            return {};
        }

        int n = (int)nodes.size();
        int src = nodeIndex(srcName);
        int dst = nodeIndex(dstName);

        std::vector<double> dist(n, INT_MAX);
        std::vector<int> parent(n, -1);
        dist[src] = 0;

        using Pair = std::pair<double, int>;
        std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> pq;
        pq.push({0, src});

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            if (d > dist[u]) continue;

            for (const auto& e : adj[nodes[u]]) {
                int v = nodeIndex(e.to);
                if (dist[u] + e.cost < dist[v]) {
                    dist[v] = dist[u] + e.cost;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        if (dist[dst] == INT_MAX) {
            std::cout << "  No route found.\n";
            return {};
        }

        std::vector<std::string> path;
        for (int v = dst; v != -1; v = parent[v])
            path.push_back(nodes[v]);
        std::reverse(path.begin(), path.end());

        std::cout << "\n  +-----------------------------------------------------+\n";
        std::cout << "  |  ROUTING  |  Algorithm: DIJKSTRA (Min-Cost Path)   |\n";
        std::cout << "  +-----------------------------------------------------+\n";
        std::cout << "  " << srcName << " --> " << dstName
                  << "  |  Cheapest total fee: $" << dist[dst] << "\n";
        std::cout << "  Route: ";
        for (size_t i = 0; i < path.size(); i++) {
            if (i > 0) std::cout << " -> ";
            std::cout << path[i];
        }
        std::cout << "\n";
        return path;
    }
};

#endif
