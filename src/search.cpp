#include "search.h"
#include "board.h"
#include "movegen.h"
#include "eval.h"
#include <algorithm>
#include <limits>

const int INFINITY_SCORE = 100000;
const int MATE_SCORE = 50000;

SearchEngine::SearchEngine(int max_depth, std::chrono::milliseconds max_time)
    : max_depth_(max_depth), max_time_(max_time), time_up_(false), nodes_searched_(0) {
}

SearchResult SearchEngine::findBestMove(Board& board, bool white_player) {
    return iterativeDeepening(board, white_player);
}

SearchResult SearchEngine::iterativeDeepening(Board& board, bool white_player) {
    start_time_ = std::chrono::steady_clock::now();
    time_up_ = false;
    nodes_searched_ = 0;

    SearchResult best_result;
    best_result.best_move = "";
    best_result.score = white_player ? -INFINITY_SCORE : INFINITY_SCORE;
    best_result.depth = 0;

    auto legal_moves = MoveGenerator::generateLegalMoves(board, white_player);
    if (legal_moves.empty()) {
        best_result.nodes_searched = nodes_searched_;
        best_result.time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_time_);
        return best_result;
    }

    // If only one legal move, return it immediately
    if (legal_moves.size() == 1) {
        best_result.best_move = legal_moves[0];
        best_result.score = 0;
        best_result.depth = 1;
        best_result.nodes_searched = 1;
        best_result.time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_time_);
        return best_result;
    }

    // Iterative deepening
    for (int depth = 1; depth <= max_depth_ && !isTimeUp(); depth++) {
        int alpha = -INFINITY_SCORE;
        int beta = INFINITY_SCORE;

        std::string best_move_this_depth;
        int best_score_this_depth = white_player ? -INFINITY_SCORE : INFINITY_SCORE;

        auto ordered_moves = orderMoves(board, legal_moves, white_player);

        for (const auto& move : ordered_moves) {
            if (isTimeUp()) break;

            Board temp_board = board.copy();
            if (!temp_board.makeMove(move)) continue;

            int score = alphaBeta(temp_board, depth - 1, alpha, beta, !white_player);

            if (white_player) {
                if (score > best_score_this_depth) {
                    best_score_this_depth = score;
                    best_move_this_depth = move;
                }
                alpha = std::max(alpha, score);
            } else {
                if (score < best_score_this_depth) {
                    best_score_this_depth = score;
                    best_move_this_depth = move;
                }
                beta = std::min(beta, score);
            }

            if (beta <= alpha) break; // Alpha-beta cutoff
        }

        if (!isTimeUp() && !best_move_this_depth.empty()) {
            best_result.best_move = best_move_this_depth;
            best_result.score = best_score_this_depth;
            best_result.depth = depth;
        }
    }

    best_result.nodes_searched = nodes_searched_;
    best_result.time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start_time_);

    return best_result;
}

int SearchEngine::alphaBeta(Board& board, int depth, int alpha, int beta, bool maximizing_player) {
    nodes_searched_++;

    if (isTimeUp() || depth < 0) {
        return Evaluator::evaluate(board, maximizing_player);
    }

    // Check for terminal positions
    if (board.isCheckmate(maximizing_player)) {
        return maximizing_player ? -MATE_SCORE - depth : MATE_SCORE + depth;
    }

    if (board.isStalemate(maximizing_player) || depth == 0) {
        if (depth == 0) {
            return quiescenceSearch(board, alpha, beta, maximizing_player);
        }
        return 0; // Stalemate
    }

    // Probe transposition table
    std::string position_key = getPositionKey(board);
    int tt_score;
    if (probeTransposition(position_key, depth, alpha, beta, tt_score)) {
        return tt_score;
    }

    auto legal_moves = MoveGenerator::generateLegalMoves(board, maximizing_player);
    if (legal_moves.empty()) {
        return board.isInCheck(maximizing_player) ?
               (maximizing_player ? -MATE_SCORE - depth : MATE_SCORE + depth) : 0;
    }

    auto ordered_moves = orderMoves(board, legal_moves, maximizing_player);
    std::string best_move;

    if (maximizing_player) {
        int max_eval = -INFINITY_SCORE;

        for (const auto& move : ordered_moves) {
            if (isTimeUp()) break;

            Board temp_board = board.copy();
            if (!temp_board.makeMove(move)) continue;

            int eval = alphaBeta(temp_board, depth - 1, alpha, beta, !maximizing_player);

            if (eval > max_eval) {
                max_eval = eval;
                best_move = move;
            }

            alpha = std::max(alpha, eval);
            if (beta <= alpha) break; // Beta cutoff
        }

        // Store in transposition table
        TranspositionEntry::NodeType type = (max_eval <= alpha) ? TranspositionEntry::UPPER_BOUND :
                                           (max_eval >= beta) ? TranspositionEntry::LOWER_BOUND :
                                           TranspositionEntry::EXACT;
        storeTransposition(position_key, max_eval, depth, type, best_move);

        return max_eval;
    } else {
        int min_eval = INFINITY_SCORE;

        for (const auto& move : ordered_moves) {
            if (isTimeUp()) break;

            Board temp_board = board.copy();
            if (!temp_board.makeMove(move)) continue;

            int eval = alphaBeta(temp_board, depth - 1, alpha, beta, !maximizing_player);

            if (eval < min_eval) {
                min_eval = eval;
                best_move = move;
            }

            beta = std::min(beta, eval);
            if (beta <= alpha) break; // Alpha cutoff
        }

        // Store in transposition table
        TranspositionEntry::NodeType type = (min_eval <= alpha) ? TranspositionEntry::UPPER_BOUND :
                                           (min_eval >= beta) ? TranspositionEntry::LOWER_BOUND :
                                           TranspositionEntry::EXACT;
        storeTransposition(position_key, min_eval, depth, type, best_move);

        return min_eval;
    }
}

int SearchEngine::quiescenceSearch(Board& board, int alpha, int beta, bool maximizing_player) {
    nodes_searched_++;

    if (isTimeUp()) {
        return 0;
    }

    int stand_pat = Evaluator::evaluate(board, maximizing_player);

    if (maximizing_player) {
        if (stand_pat >= beta) return beta;
        alpha = std::max(alpha, stand_pat);
    } else {
        if (stand_pat <= alpha) return alpha;
        beta = std::min(beta, stand_pat);
    }

    // Generate only capture moves for quiescence search
    auto legal_moves = MoveGenerator::generateLegalMoves(board, maximizing_player);
    std::vector<std::string> capture_moves;

    for (const auto& move : legal_moves) {
        if (move.length() >= 4) {
            auto to_pos = MoveGenerator::squareToPosition(move.substr(2, 2));
            if (to_pos[0] >= 0 && to_pos[1] >= 0) {
                char target = board.getPiece(to_pos[0], to_pos[1]);
                if (target != '.') {
                    capture_moves.push_back(move);
                }
            }
        }
    }

    if (capture_moves.empty()) {
        return stand_pat;
    }

    auto ordered_captures = orderMoves(board, capture_moves, maximizing_player);

    if (maximizing_player) {
        for (const auto& move : ordered_captures) {
            if (isTimeUp()) break;

            Board temp_board = board.copy();
            if (!temp_board.makeMove(move)) continue;

            int eval = quiescenceSearch(temp_board, alpha, beta, !maximizing_player);
            alpha = std::max(alpha, eval);

            if (beta <= alpha) break;
        }
        return alpha;
    } else {
        for (const auto& move : ordered_captures) {
            if (isTimeUp()) break;

            Board temp_board = board.copy();
            if (!temp_board.makeMove(move)) continue;

            int eval = quiescenceSearch(temp_board, alpha, beta, !maximizing_player);
            beta = std::min(beta, eval);

            if (beta <= alpha) break;
        }
        return beta;
    }
}

std::vector<std::string> SearchEngine::orderMoves(const Board& board, const std::vector<std::string>& moves, bool white_player) {
    std::vector<std::pair<std::string, int>> scored_moves;

    for (const auto& move : moves) {
        int score = getMoveScore(board, move, white_player);
        scored_moves.emplace_back(move, score);
    }

    // Sort moves by score (highest first for better move ordering)
    std::sort(scored_moves.begin(), scored_moves.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::vector<std::string> ordered_moves;
    for (const auto& scored_move : scored_moves) {
        ordered_moves.push_back(scored_move.first);
    }

    return ordered_moves;
}

int SearchEngine::getMoveScore(const Board& board, const std::string& move, bool white_player) {
    int score = 0;

    if (move.length() >= 4) {
        auto from_pos = MoveGenerator::squareToPosition(move.substr(0, 2));
        auto to_pos = MoveGenerator::squareToPosition(move.substr(2, 2));

        if (from_pos[0] >= 0 && to_pos[0] >= 0) {
            char moving_piece = board.getPiece(from_pos[0], from_pos[1]);
            char target_piece = board.getPiece(to_pos[0], to_pos[1]);

            // Prioritize captures
            if (target_piece != '.') {
                score += Evaluator::getPieceValue(target_piece) - Evaluator::getPieceValue(moving_piece);
            }

            // Prioritize center control
            int center_bonus = 0;
            if (to_pos[0] >= 3 && to_pos[0] <= 4 && to_pos[1] >= 3 && to_pos[1] <= 4) {
                center_bonus = 10;
            }
            score += center_bonus;

            // Prioritize piece development
            if (std::tolower(moving_piece) == 'n' || std::tolower(moving_piece) == 'b') {
                if ((white_player && from_pos[0] == 7) || (!white_player && from_pos[0] == 0)) {
                    score += 20; // Development bonus
                }
            }
        }
    }

    return score;
}

bool SearchEngine::isTimeUp() const {
    if (time_up_) return true;

    auto current_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time_);

    return elapsed >= max_time_;
}

void SearchEngine::storeTransposition(const std::string& position_key, int score, int depth,
                                     TranspositionEntry::NodeType type, const std::string& best_move) {
    TranspositionEntry entry;
    entry.score = score;
    entry.depth = depth;
    entry.type = type;
    entry.best_move = best_move;

    transposition_table_[position_key] = entry;
}

bool SearchEngine::probeTransposition(const std::string& position_key, int depth, int alpha, int beta, int& score) {
    auto it = transposition_table_.find(position_key);
    if (it == transposition_table_.end()) {
        return false;
    }

    const TranspositionEntry& entry = it->second;
    if (entry.depth < depth) {
        return false;
    }

    switch (entry.type) {
        case TranspositionEntry::EXACT:
            score = entry.score;
            return true;
        case TranspositionEntry::LOWER_BOUND:
            if (entry.score >= beta) {
                score = entry.score;
                return true;
            }
            break;
        case TranspositionEntry::UPPER_BOUND:
            if (entry.score <= alpha) {
                score = entry.score;
                return true;
            }
            break;
    }

    return false;
}

std::string SearchEngine::getPositionKey(const Board& board) const {
    // Simple position key based on board state
    std::string key;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            key += board.getPiece(row, col);
        }
    }
    key += board.isWhiteToMove() ? "w" : "b";
    return key;
}