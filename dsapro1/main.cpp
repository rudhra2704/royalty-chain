#include <iostream>
#include <string>

#include "include/btree.h"
#include "include/huffman.h"
#include "include/license_hash.h"
#include "include/options_heap.h"
#include "include/revocation_stack.h"
#include "include/royalty_graph.h"
#include "include/verification_queue.h"

using namespace std;

// Global system components — one per assignment requirement
BTree registry;              // [1] Indexing Registry  — B+ Tree
RevocationLog revLog;        // [2] Revocation Log     — Stack
VerificationQueue transferQueue; // [3] Verification Loop — Queue
LicenseHashTable keyLedger;  // [4] Tracking Ledger    — Hash Table
OptionsHeap tradeRanker;     // [5] Options Sorter     — Max-Heap
RoyaltyGraph royaltyNet;   // [6][7] Channels+Route  — Graph + Dijkstra
HuffmanEncoder compressor;   // [8] Storage Divider    — Huffman Coding

void printHeader() {
    cout << "\n";
    cout << "  ============================================================\n";
    cout << "   ROYALTYCHAIN  |  Intellectual Property Rights Ledger\n";
    cout << "   Digital Copyright & Royalty Management System\n";
    cout << "  ============================================================\n";
    cout << "   Modeled after BMI, ASCAP & SoundExchange\n\n";
}

void printDataSummary() {
    cout << "  Loaded dataset snapshot:\n";
    cout << "  ----------------------------------------------------------\n";
    cout << "   Licenses ........ 16 records   (royalty: 5% - 22%)\n";
    cout << "   Clearances ...... 12 entries   (APPROVED / FLAGGED / REVOKED)\n";
    cout << "   Transfers ....... 12 pending   (FIFO queue)\n";
    cout << "   Accounts ........ 14 keys      (10 active, 4 revoked)\n";
    cout << "   Trade volume .... 47K - 8.4M streams (PLATINUM to CATALOG)\n";
    cout << "   Graph routes .... multiple paths with competing fees\n";
    cout << "  ----------------------------------------------------------\n\n";
}

void printMenu() {
    cout << "\n  ======================== MAIN MENU ========================\n\n";

    cout << "   STORAGE & INDEXING\n";
    cout << "   [1]  License Registry .......... B+ Tree (disk-backed)\n";
    cout << "   [8]  Asset Compression .......... Huffman Coding\n\n";

    cout << "   AUDIT & VERIFICATION\n";
    cout << "   [2]  Revocation Log ............. Stack  (LIFO / rollback)\n";
    cout << "   [3]  Transfer Queue ............. Queue  (FIFO / ordered)\n";
    cout << "   [4]  Key Ledger ................. Hash Table (O(1) lookup)\n\n";

    cout << "   ANALYTICS & ROUTING\n";
    cout << "   [5]  Trade Rankings ............. Max-Heap (by volume)\n";
    cout << "   [6]  Royalty Channels ........... Graph (adjacency list)\n";
    cout << "   [7]  Cheapest Route ............. Dijkstra's Algorithm\n\n";

    cout << "   [9]  Run Full Demo (all 8 data structures)\n";
    cout << "   [0]  Exit\n";
    cout << "  ============================================================\n";
    cout << "   Enter choice: ";
}

void loadSampleData() {
    const bool quiet = true;

    // ── [1] B+ Tree — royalty rates span 5% (archival) to 22% (indie-owned) ──
    registry.insert({"LIC-001", "God's Plan",         "Drake",          "OVO Sound",        0.19});
    registry.insert({"LIC-002", "HUMBLE.",            "Kendrick Lamar", "TDE",              0.16});
    registry.insert({"LIC-003", "No Role Modelz",     "J. Cole",        "Dreamville",       0.12});
    registry.insert({"LIC-004", "151 Rum",            "JID",            "Dreamville",       0.08});
    registry.insert({"LIC-005", "Surround Sound",     "JID",            "Dreamville",       0.09});
    registry.insert({"LIC-006", "Middle Child",       "J. Cole",        "Dreamville",       0.13});
    registry.insert({"LIC-007", "Alright",            "Kendrick Lamar", "TDE",              0.15});
    registry.insert({"LIC-008", "Hotline Bling",      "Drake",          "OVO Sound",        0.18});
    registry.insert({"LIC-009", "Praise The Lord",    "ASAP Rocky",     "AWGE",             0.11});
    registry.insert({"LIC-010", "L$D",                "ASAP Rocky",     "AWGE",             0.07});
    registry.insert({"LIC-011", "SICKO MODE",         "Travis Scott",   "Cactus Jack",      0.17});
    registry.insert({"LIC-012", "goosebumps",         "Travis Scott",   "Cactus Jack",      0.14});
    registry.insert({"LIC-013", "Thinkin Bout You",   "Frank Ocean",    "Blonded",          0.22});
    registry.insert({"LIC-014", "Nights",             "Frank Ocean",    "Blonded",          0.21});
    registry.insert({"LIC-015", "Boyz-N-The-Hood",    "Eazy-E",         "Ruthless Records", 0.06});
    registry.insert({"LIC-016", "Real Muthaphuckkin G's","Eazy-E",       "Ruthless Records", 0.05});

    // ── [2] Stack — mixed clearance states, newest on top ──
    revLog.push({"CLR-101", "ASCAP",         "APPROVED",  "2025-01-15", "LIC-001"}, quiet);
    revLog.push({"CLR-102", "BMI",           "VERIFIED",  "2025-02-20", "LIC-002"}, quiet);
    revLog.push({"CLR-103", "SoundExchange", "APPROVED",  "2025-03-10", "LIC-003"}, quiet);
    revLog.push({"CLR-104", "ASCAP",         "FLAGGED",   "2025-04-05", "LIC-004"}, quiet);
    revLog.push({"CLR-105", "BMI",           "REVOKED",   "2025-05-01", "LIC-005"}, quiet);
    revLog.push({"CLR-106", "ASCAP",         "APPROVED",  "2025-05-12", "LIC-009"}, quiet);
    revLog.push({"CLR-107", "BMI",           "VERIFIED",  "2025-05-18", "LIC-011"}, quiet);
    revLog.push({"CLR-108", "SoundExchange", "APPROVED",  "2025-06-01", "LIC-013"}, quiet);
    revLog.push({"CLR-109", "ASCAP",         "FLAGGED",   "2025-06-08", "LIC-015"}, quiet);
    revLog.push({"CLR-110", "BMI",           "REVOKED",   "2025-06-10", "LIC-010"}, quiet);
    revLog.push({"CLR-111", "SoundExchange", "VERIFIED",  "2025-06-12", "LIC-007"}, quiet);
    revLog.push({"CLR-112", "BMI",           "APPROVED",  "2025-06-14", "LIC-012"}, quiet);

    // ── [3] Queue — transfers arrive out of order, processed FIFO ──
    transferQueue.enqueue({"TXN-001", "OVO Sound",        "Spotify",       "LIC-001", "God's Plan"},         quiet);
    transferQueue.enqueue({"TXN-002", "TDE",              "Apple Music",   "LIC-002", "HUMBLE."},            quiet);
    transferQueue.enqueue({"TXN-003", "Dreamville",       "YouTube Music", "LIC-003", "No Role Modelz"},     quiet);
    transferQueue.enqueue({"TXN-004", "Dreamville",       "Amazon Music",  "LIC-004", "151 Rum"},              quiet);
    transferQueue.enqueue({"TXN-005", "OVO Sound",        "Spotify",       "LIC-008", "Hotline Bling"},      quiet);
    transferQueue.enqueue({"TXN-006", "AWGE",             "Apple Music",   "LIC-009", "Praise The Lord"},    quiet);
    transferQueue.enqueue({"TXN-007", "Cactus Jack",      "Spotify",       "LIC-011", "SICKO MODE"},         quiet);
    transferQueue.enqueue({"TXN-008", "Blonded",          "YouTube Music", "LIC-013", "Thinkin Bout You"},   quiet);
    transferQueue.enqueue({"TXN-009", "Ruthless Records", "Amazon Music",  "LIC-015", "Boyz-N-The-Hood"},    quiet);
    transferQueue.enqueue({"TXN-010", "Cactus Jack",      "Spotify",       "LIC-012", "goosebumps"},         quiet);
    transferQueue.enqueue({"TXN-011", "Blonded",          "Apple Music",   "LIC-014", "Nights"},             quiet);
    transferQueue.enqueue({"TXN-012", "Ruthless Records", "Amazon Music",  "LIC-016", "Real Muthaphuckkin G's"}, quiet);

    // ── [4] Hash Table — 4 revoked accounts mixed in ──
    keyLedger.insert({"ACC-DRAKE",    "PK-DRK-7X9F", "Drake",                  true});
    keyLedger.insert({"ACC-KENDRICK", "PK-KDL-3M2A", "Kendrick Lamar",         true});
    keyLedger.insert({"ACC-JCOLE",    "PK-JCL-8B4E", "J. Cole",                true});
    keyLedger.insert({"ACC-JID",      "PK-JID-1C6D", "JID",                    true});
    keyLedger.insert({"ACC-ROCKY",    "PK-ROK-4A7B", "ASAP Rocky",             false});  // licensing dispute
    keyLedger.insert({"ACC-TRAVIS",   "PK-TRV-6E2C", "Travis Scott",           true});
    keyLedger.insert({"ACC-FRANK",    "PK-FRK-9D5H", "Frank Ocean",            true});
    keyLedger.insert({"ACC-EAZYE",    "PK-EZE-2F8K", "Eazy-E (Estate)",        false});  // estate frozen
    keyLedger.insert({"ACC-OVO",      "PK-OVO-5H8G", "OVO Sound",              true});
    keyLedger.insert({"ACC-TDE",      "PK-TDE-9K3J", "TDE",                    false});  // audit hold
    keyLedger.insert({"ACC-AWGE",     "PK-AWG-3N1P", "AWGE",                   true});
    keyLedger.insert({"ACC-CACTUS",   "PK-CJK-7R4M", "Cactus Jack",            true});
    keyLedger.insert({"ACC-BLONDED",  "PK-BLD-2Q7N", "Blonded",                false});  // label suspended
    keyLedger.insert({"ACC-RUTHLESS", "PK-RTH-8W1X", "Ruthless Records",       true});

    // ── [5] Max-Heap — volume spread: 47K (catalog) to 8.4M (platinum) ──
    tradeRanker.insert({"God's Plan",              "Drake",          8420000, 2.49});
    tradeRanker.insert({"SICKO MODE",              "Travis Scott",   6150000, 2.29});
    tradeRanker.insert({"Hotline Bling",           "Drake",          4890000, 1.99});
    tradeRanker.insert({"HUMBLE.",                 "Kendrick Lamar", 3720000, 1.89});
    tradeRanker.insert({"goosebumps",              "Travis Scott",   2940000, 2.19});
    tradeRanker.insert({"Praise The Lord",         "ASAP Rocky",     1680000, 1.49});
    tradeRanker.insert({"No Role Modelz",          "J. Cole",        1420000, 1.29});
    tradeRanker.insert({"Thinkin Bout You",        "Frank Ocean",    1210000, 1.79});
    tradeRanker.insert({"Middle Child",            "J. Cole",         980000, 1.29});
    tradeRanker.insert({"L$D",                     "ASAP Rocky",      740000, 0.99});
    tradeRanker.insert({"Surround Sound",          "JID",             620000, 1.29});
    tradeRanker.insert({"Alright",                 "Kendrick Lamar",  510000, 1.29});
    tradeRanker.insert({"Nights",                  "Frank Ocean",     385000, 1.59});
    tradeRanker.insert({"151 Rum",                 "JID",             290000, 0.99});
    tradeRanker.insert({"Boyz-N-The-Hood",         "Eazy-E",          156000, 0.79});
    tradeRanker.insert({"Real Muthaphuckkin G's",  "Eazy-E",           47000, 0.69});

    // ── [6][7] Graph — direct cheap routes + expensive aggregator detours ──
    // Streaming platforms -> publishers (competing fees)
    royaltyNet.addEdge("Spotify",       "OVO Sound",           2.50);
    royaltyNet.addEdge("Spotify",       "TDE",                 3.20);
    royaltyNet.addEdge("Spotify",       "Dreamville",          2.90);
    royaltyNet.addEdge("Spotify",       "AWGE",                2.70);
    royaltyNet.addEdge("Spotify",       "Cactus Jack",         2.40);
    royaltyNet.addEdge("Spotify",       "Blonded",             4.10);  // expensive direct
    royaltyNet.addEdge("Spotify",       "Ruthless Records",    3.40);
    royaltyNet.addEdge("Spotify",       "Global Rights Hub",   1.80);  // cheap entry, costly chain

    royaltyNet.addEdge("Apple Music",   "OVO Sound",           2.20);
    royaltyNet.addEdge("Apple Music",   "TDE",                 2.80);
    royaltyNet.addEdge("Apple Music",   "Dreamville",          2.50);
    royaltyNet.addEdge("Apple Music",   "AWGE",                2.30);
    royaltyNet.addEdge("Apple Music",   "Cactus Jack",         2.10);
    royaltyNet.addEdge("Apple Music",   "Blonded",             3.60);
    royaltyNet.addEdge("Apple Music",   "Ruthless Records",    2.90);
    royaltyNet.addEdge("Apple Music",   "Global Rights Hub",   4.00);

    royaltyNet.addEdge("YouTube Music", "Dreamville",          1.80);
    royaltyNet.addEdge("YouTube Music", "TDE",                 2.30);
    royaltyNet.addEdge("YouTube Music", "AWGE",                2.10);
    royaltyNet.addEdge("YouTube Music", "Blonded",             1.70);  // cheapest to Frank
    royaltyNet.addEdge("YouTube Music", "Cactus Jack",         2.20);
    royaltyNet.addEdge("YouTube Music", "Global Rights Hub",   2.50);

    royaltyNet.addEdge("Amazon Music",  "Dreamville",          2.00);
    royaltyNet.addEdge("Amazon Music",  "OVO Sound",           2.60);
    royaltyNet.addEdge("Amazon Music",  "Ruthless Records",    1.60);  // best for Eazy-E
    royaltyNet.addEdge("Amazon Music",  "Blonded",             2.80);
    royaltyNet.addEdge("Amazon Music",  "Global Rights Hub",   3.10);

    // Aggregator middleman — high downstream fees make detour costly
    royaltyNet.addEdge("Global Rights Hub", "OVO Sound",        2.80);
    royaltyNet.addEdge("Global Rights Hub", "Cactus Jack",      2.50);
    royaltyNet.addEdge("Global Rights Hub", "AWGE",             2.20);
    royaltyNet.addEdge("Global Rights Hub", "Blonded",          3.00);
    royaltyNet.addEdge("Global Rights Hub", "Ruthless Records", 2.60);

    // Publishers -> artists (varied artist fees)
    royaltyNet.addEdge("OVO Sound",        "Drake",          1.00);
    royaltyNet.addEdge("TDE",              "Kendrick Lamar", 1.20);
    royaltyNet.addEdge("Dreamville",       "J. Cole",        0.90);
    royaltyNet.addEdge("Dreamville",       "JID",            0.80);
    royaltyNet.addEdge("AWGE",             "ASAP Rocky",     0.85);
    royaltyNet.addEdge("Cactus Jack",      "Travis Scott",   0.95);
    royaltyNet.addEdge("Blonded",          "Frank Ocean",    0.75);
    royaltyNet.addEdge("Ruthless Records", "Eazy-E",         0.70);
}

void runFullDemo() {
    cout << "\n  ============================================================\n";
    cout << "   FULL SYSTEM DEMO  —  All 8 Data Structures\n";
    cout << "  ============================================================\n";

    cout << "\n  [1/8] INDEXING REGISTRY  |  B+ Tree  |  Disk-backed index\n";
    registry.displayAll();
    registry.saveToDisk();
    cout << "\n  Reloading from disk to verify persistence...\n";
    registry.loadFromDisk();
    cout << "  Disk reload complete.\n";

    cout << "\n  [2/8] REVOCATION LOG  |  Stack (LIFO)  |  Rollback support\n";
    revLog.display();
    cout << "\n  >> Rolling back most recent clearance (pop)...\n";
    revLog.rollback();
    revLog.display();

    cout << "\n  [3/8] VERIFICATION LOOP  |  Queue (FIFO)  |  Ordered processing\n";
    transferQueue.display();
    transferQueue.processAll();

    cout << "\n  [4/8] TRACKING LEDGER  |  Hash Table  |  O(1) key lookup\n";
    keyLedger.verify("ACC-DRAKE");
    keyLedger.verify("ACC-ROCKY");    // revoked — shows failure case
    keyLedger.verify("ACC-TRAVIS");
    keyLedger.verify("ACC-EAZYE");    // revoked estate

    cout << "\n  [5/8] OPTIONS SORTER  |  Max-Heap  |  Volume-ranked trades\n";
    tradeRanker.displayRanked();

    cout << "\n  [6/8] ROYALTY CHANNELS  |  Graph  |  Distribution network\n";
    royaltyNet.displayChannels();

    cout << "\n  [7/8] ROUTING CHANNEL  |  Dijkstra  |  Minimum-cost paths\n";
    cout << "  (Notice: Dijkstra skips expensive aggregator detours)\n";
    royaltyNet.findCheapestRoute("Spotify", "Drake");
    royaltyNet.findCheapestRoute("Spotify", "Travis Scott");
    royaltyNet.findCheapestRoute("YouTube Music", "Frank Ocean");
    royaltyNet.findCheapestRoute("Amazon Music", "Eazy-E");

    cout << "\n  [8/8] STORAGE DIVIDER  |  Huffman Coding  |  Asset compression\n";
    compressor.displayCodes("Drake Gods Plan OVO Sound digital license asset");
    compressor.displayCodes("Eazy-E Ruthless Records archival catalog license metadata");

    cout << "\n  ============================================================\n";
    cout << "   Demo complete — all 8 assignment requirements demonstrated.\n";
    cout << "  ============================================================\n";
}

int main() {
    loadSampleData();
    printHeader();
    printDataSummary();

    int choice;
    do {
        printMenu();
        cin >> choice;

        switch (choice) {
        case 1:
            registry.displayAll();
            cout << "\n  Search by license ID (or type 'skip'): ";
            { string id; cin >> id;
              if (id != "skip") {
                  LicenseRecord* r = registry.search(id);
                  if (r) cout << "  >> Found: " << r->title << " by " << r->artist
                              << " | Royalty: " << (int)(r->royaltyRate * 100) << "%\n";
                  else cout << "  >> License not found.\n";
              }
            }
            registry.saveToDisk();
            break;

        case 2:
            revLog.display();
            cout << "\n  Rollback top entry? (1=Yes, 0=No): ";
            { int r; cin >> r; if (r) revLog.rollback(); }
            break;

        case 3:
            transferQueue.display();
            cout << "\n  Process all pending? (1=Yes, 0=No): ";
            { int p; cin >> p; if (p) transferQueue.processAll(); }
            break;

        case 4:
            keyLedger.displayAll();
            cout << "\n  Verify account ID: ";
            { string id; cin >> id; keyLedger.verify(id); }
            break;

        case 5:
            tradeRanker.displayRanked();
            break;

        case 6:
            royaltyNet.displayChannels();
            break;

        case 7:
            cout << "\n  Type platform then artist (case doesn't matter).\n";
            cout << "  Examples:  spotify  ->  drake\n";
            cout << "              youtube music  ->  frank ocean\n";
            { string src, dst;
              if (cin.peek() == '\n') cin.ignore();
              cout << "  Source platform: ";
              getline(cin, src);
              cout << "  Destination artist: ";
              getline(cin, dst);
              if (src.empty() || dst.empty())
                  cout << "  Both source and destination are required.\n";
              else
                  royaltyNet.findCheapestRoute(src, dst);
            }
            break;

        case 8:
            cout << "\n  Enter asset description to compress: ";
            { string desc; cin.ignore(); getline(cin, desc);
              if (desc.empty()) desc = "Frank Ocean Blonded independent release license";
              compressor.displayCodes(desc);
            }
            break;

        case 9:
            runFullDemo();
            break;

        case 0:
            cout << "\n  Shutting down RoyaltyChain. Goodbye!\n\n";
            break;

        default:
            cout << "\n  Invalid choice. Enter a number 0-9.\n";
        }
    } while (choice != 0);

    return 0;
}
