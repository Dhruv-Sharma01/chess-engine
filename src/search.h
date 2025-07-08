#ifndef SEARCH_H
#define SEARCH_H

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

class Board;

struct SearchResult {
    std::string best_move;
    int score;
    int depth;
    int nodes_searched;
    std::chrono::milliseconds time_taken;
};

struct TranspositionEntry {
    int score;
    int depth;
    enum NodeType { EXACT, LOWER_BOUND, UPPER_BOUND } type;
    std::string best_move;
};

class SearchEngine {
private:
    std::unordered_map<std::string, TranspositionEntry> transposition_table_;
    int max_depth_;
    std::chrono::milliseconds max_time_;
    bool time_up_;
    int nodes_searched_;
    std::chrono::steady_clock::time_point start_time_;

    // Search methods
    int alphaBeta(Board& board, int depth, int alpha, int beta, bool maximizing_player);
    int quiescenceSearch(Board& board, int alpha, int beta, bool maximizing_player);

    // Move ordering
    std::vector<std::string> orderMoves(const Board& board, const std::vector<std::string>& moves, bool white_player);
    int getMoveScore(const Board& board, const std::string& move, bool white_player);

    // Utility methods
    bool isTimeUp() const;
    void storeTransposition(const std::string& position_key, int score, int depth,
                           TranspositionEntry::NodeType type, const std::string& best_move);
    bool probeTransposition(const std::string& position_key, int depth, int alpha, int beta, int& score);
    std::string getPositionKey(const Board& board) const;

public:
    SearchEngine(int max_depth = 6, std::chrono::milliseconds max_time = std::chrono::milliseconds(5000));

    // Main search interface
    SearchResult findBestMove(Board& board, bool white_player);
    SearchResult iterativeDeepening(Board& board, bool white_player);

    // Configuration
    void setMaxDepth(int depth) { max_depth_ = depth; }
    void setMaxTime(std::chrono::milliseconds time) { max_time_ = time; }
    void clearTranspositionTable() { transposition_table_.clear(); }

    // Statistics
    int getNodesSearched() const { return nodes_searched_; }
    size_t getTranspositionTableSize() const { return transposition_table_.size(); }
};

#endif // SEARCH_H