#include "eval.h"
#include "board.h"
#include "movegen.h"
#include <algorithm>
#include <cmath>

// Piece values in centipawns
const int Evaluator::PIECE_VALUES[6] = {100, 320, 330, 500, 900, 20000}; // P, N, B, R, Q, K

// Piece-square tables (from white's perspective)
const int Evaluator::PAWN_TABLE[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    { 5,  5, 10, 25, 25, 10,  5,  5},
    { 0,  0,  0, 20, 20,  0,  0,  0},
    { 5, -5,-10,  0,  0,-10, -5,  5},
    { 5, 10, 10,-20,-20, 10, 10,  5},
    { 0,  0,  0,  0,  0,  0,  0,  0}
};

const int Evaluator::KNIGHT_TABLE[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

const int Evaluator::BISHOP_TABLE[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20}
};

const int Evaluator::ROOK_TABLE[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 5, 10, 10, 10, 10, 10, 10,  5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    { 0,  0,  0,  5,  5,  0,  0,  0}
};

const int Evaluator::QUEEN_TABLE[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5,  5,  5,  5,  0,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {  0,  0,  5,  5,  5,  5,  0, -5},
    {-10,  5,  5,  5,  5,  5,  0,-10},
    {-10,  0,  5,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20}
};

const int Evaluator::KING_MIDDLE_GAME_TABLE[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-20,-30,-30,-40,-40,-30,-30,-20},
    {-10,-20,-20,-20,-20,-20,-20,-10},
    { 20, 20,  0,  0,  0,  0, 20, 20},
    { 20, 30, 10,  0,  0, 10, 30, 20}
};

const int Evaluator::KING_END_GAME_TABLE[8][8] = {
    {-50,-40,-30,-20,-20,-30,-40,-50},
    {-30,-20,-10,  0,  0,-10,-20,-30},
    {-30,-10, 20, 30, 30, 20,-10,-30},
    {-30,-10, 30, 40, 40, 30,-10,-30},
    {-30,-10, 30, 40, 40, 30,-10,-30},
    {-30,-10, 20, 30, 30, 20,-10,-30},
    {-30,-30,  0,  0,  0,  0,-30,-30},
    {-50,-30,-30,-30,-30,-30,-30,-50}
};

int Evaluator::evaluate(const Board& board, bool white_perspective) {
    int score = 0;

    // Material evaluation
    score += getMaterialValue(board, true) - getMaterialValue(board, false);

    // Positional evaluation
    score += getPositionalValue(board, true) - getPositionalValue(board, false);

    // Pawn structure
    score += evaluatePawnStructure(board, true) - evaluatePawnStructure(board, false);

    // King safety
    if (!isEndgame(board)) {
        score += evaluateKingSafety(board, true) - evaluateKingSafety(board, false);
    }

    // Piece mobility
    score += evaluateMobility(board, true) - evaluateMobility(board, false);

    // Endgame evaluation
    if (isEndgame(board)) {
        score += evaluateEndgame(board, true) - evaluateEndgame(board, false);
    }

    return white_perspective ? score : -score;
}

int Evaluator::getPieceValue(char piece) {
    switch (std::tolower(piece)) {
        case 'p': return PIECE_VALUES[0];
        case 'n': return PIECE_VALUES[1];
        case 'b': return PIECE_VALUES[2];
        case 'r': return PIECE_VALUES[3];
        case 'q': return PIECE_VALUES[4];
        case 'k': return PIECE_VALUES[5];
        default: return 0;
    }
}

int Evaluator::getMaterialValue(const Board& board, bool white_player) {
    int material = 0;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            char piece = board.getPiece(row, col);
            if (piece == '.') continue;

            bool is_white = (piece >= 'A' && piece <= 'Z');
            if (is_white == white_player) {
                material += getPieceValue(piece);
            }
        }
    }

    return material;
}

int Evaluator::getPositionalValue(const Board& board, bool white_player) {
    int positional = 0;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            char piece = board.getPiece(row, col);
            if (piece == '.') continue;

            bool is_white = (piece >= 'A' && piece <= 'Z');
            if (is_white == white_player) {
                positional += getPieceSquareValue(piece, row, col);
            }
        }
    }

    return positional;
}

int Evaluator::getPieceSquareValue(char piece, int row, int col) {
    bool is_white = (piece >= 'A' && piece <= 'Z');
    int actual_row = is_white ? row : (7 - row); // Flip for black pieces

    switch (std::tolower(piece)) {
        case 'p': return PAWN_TABLE[actual_row][col];
        case 'n': return KNIGHT_TABLE[actual_row][col];
        case 'b': return BISHOP_TABLE[actual_row][col];
        case 'r': return ROOK_TABLE[actual_row][col];
        case 'q': return QUEEN_TABLE[actual_row][col];
        case 'k':
            return isEndgame(Board()) ? // Simplified endgame check
                   KING_END_GAME_TABLE[actual_row][col] :
                   KING_MIDDLE_GAME_TABLE[actual_row][col];
        default: return 0;
    }
}

int Evaluator::evaluatePawnStructure(const Board& board, bool white_player) {
    int score = 0;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            char piece = board.getPiece(row, col);
            if (piece != (white_player ? 'P' : 'p')) continue;

            // Doubled pawns penalty
            if (isDoubledPawn(board, col, white_player)) {
                score -= 10;
            }

            // Isolated pawns penalty
            if (isIsolatedPawn(board, row, col, white_player)) {
                score -= 20;
            }

            // Passed pawns bonus
            if (isPassedPawn(board, row, col, white_player)) {
                int rank = white_player ? (7 - row) : row;
                score += 20 + rank * 10; // More valuable closer to promotion
            }
        }
    }

    return score;
}

bool Evaluator::isDoubledPawn(const Board& board, int col, bool white_player) {
    char pawn = white_player ? 'P' : 'p';
    int count = 0;

    for (int row = 0; row < 8; row++) {
        if (board.getPiece(row, col) == pawn) {
            count++;
        }
    }

    return count > 1;
}

bool Evaluator::isIsolatedPawn(const Board& board, int row, int col, bool white_player) {
    char pawn = white_player ? 'P' : 'p';

    // Check adjacent files for friendly pawns
    for (int dc = -1; dc <= 1; dc += 2) {
        int adj_col = col + dc;
        if (adj_col >= 0 && adj_col < 8) {
            for (int r = 0; r < 8; r++) {
                if (board.getPiece(r, adj_col) == pawn) {
                    return false; // Found a friendly pawn on adjacent file
                }
            }
        }
    }

    return true; // No friendly pawns on adjacent files
}

bool Evaluator::isPassedPawn(const Board& board, int row, int col, bool white_player) {
    char enemy_pawn = white_player ? 'p' : 'P';
    int direction = white_player ? -1 : 1;

    // Check if any enemy pawns can block or capture this pawn
    for (int dc = -1; dc <= 1; dc++) {
        int check_col = col + dc;
        if (check_col >= 0 && check_col < 8) {
            for (int r = row + direction; r >= 0 && r < 8; r += direction) {
                if (board.getPiece(r, check_col) == enemy_pawn) {
                    return false; // Enemy pawn found in path
                }
            }
        }
    }

    return true; // No enemy pawns blocking the path
}

int Evaluator::evaluateKingSafety(const Board& board, bool white_player) {
    // Simplified king safety evaluation
    // In a full implementation, this would check for:
    // - Pawn shield in front of king
    // - Open files near king
    // - Enemy pieces attacking king zone

    return 0; // Placeholder
}

int Evaluator::evaluateMobility(const Board& board, bool white_player) {
    auto legal_moves = MoveGenerator::generateLegalMoves(board, white_player);
    return static_cast<int>(legal_moves.size()) * 2; // 2 points per legal move
}

bool Evaluator::isEndgame(const Board& board) {
    int total_material = 0;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            char piece = board.getPiece(row, col);
            if (piece != '.' && std::tolower(piece) != 'k') {
                total_material += getPieceValue(piece);
            }
        }
    }

    // Consider it endgame if total material is less than 2 rooks + 2 bishops + 2 knights
    return total_material < (2 * PIECE_VALUES[3] + 2 * PIECE_VALUES[2] + 2 * PIECE_VALUES[1]);
}

int Evaluator::evaluateEndgame(const Board& board, bool white_player) {
    int score = 0;

    // In endgame, centralize the king
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            char piece = board.getPiece(row, col);
            if (piece == (white_player ? 'K' : 'k')) {
                // Distance from center
                int center_distance = std::abs(row - 3.5) + std::abs(col - 3.5);
                score += (7 - center_distance) * 10; // Bonus for being closer to center
                break;
            }
        }
    }

    return score;
}
