#include "movegen.h"
#include "board.h"
#include <algorithm>

std::string MoveGenerator::positionToSquare(int row, int col) {
    if (row < 0 || row >= 8 || col < 0 || col >= 8) return "";
    char file = 'a' + col;
    char rank = '1' + (7 - row);
    return std::string{file, rank};
}

std::vector<int> MoveGenerator::squareToPosition(const std::string& square) {
    if (square.length() != 2) return {-1, -1};
    int file = square[0] - 'a';
    int rank = square[1] - '1';
    return {7 - rank, file};
}

std::vector<std::string> MoveGenerator::generateLegalMoves(const Board& board, bool white_player) {
    std::vector<std::string> legal_moves;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            char piece = board.getPiece(row, col);
            if (piece == '.') continue;

            bool is_white_piece = (piece >= 'A' && piece <= 'Z');
            if (is_white_piece != white_player) continue;

            auto pseudo_moves = generatePseudoLegalMoves(board, row, col);
            std::string from_square = positionToSquare(row, col);

            // Check each pseudo-legal move to see if it's actually legal
            for (const auto& to_square : pseudo_moves) {
                std::string full_move = from_square + to_square;

                // Test the move by making it on a copy of the board
                Board temp_board = board.copy();
                if (temp_board.makeMove(full_move)) {
                    // Check if this move leaves our king in check
                    if (!temp_board.isInCheck(white_player)) {
                        legal_moves.push_back(full_move);
                    }
                }
            }
        }
    }

    return legal_moves;
}

std::vector<std::string> MoveGenerator::generatePseudoLegalMoves(const Board& board, int row, int col) {
    char piece = board.getPiece(row, col);
    if (piece == '.') return {};

    bool white_piece = (piece >= 'A' && piece <= 'Z');

    switch (std::tolower(piece)) {
        case 'p': return generatePawnMoves(board, row, col, white_piece);
        case 'r': return generateRookMoves(board, row, col, white_piece);
        case 'n': return generateKnightMoves(board, row, col, white_piece);
        case 'b': return generateBishopMoves(board, row, col, white_piece);
        case 'q': return generateQueenMoves(board, row, col, white_piece);
        case 'k': return generateKingMoves(board, row, col, white_piece);
        default: return {};
    }
}

std::vector<std::string> MoveGenerator::generatePawnMoves(const Board& board, int row, int col, bool white_piece) {
    std::vector<std::string> moves;
    int direction = white_piece ? -1 : 1;
    int start_row = white_piece ? 6 : 1;

    // Forward moves
    int new_row = row + direction;
    if (new_row >= 0 && new_row < 8 && board.getPiece(new_row, col) == '.') {
        moves.push_back(positionToSquare(new_row, col));

        // Double move from starting position
        if (row == start_row) {
            new_row = row + 2 * direction;
            if (new_row >= 0 && new_row < 8 && board.getPiece(new_row, col) == '.') {
                moves.push_back(positionToSquare(new_row, col));
            }
        }
    }

    // Captures
    for (int dc = -1; dc <= 1; dc += 2) {
        int new_col = col + dc;
        new_row = row + direction;
        if (new_row >= 0 && new_row < 8 && new_col >= 0 && new_col < 8) {
            char target = board.getPiece(new_row, new_col);
            if (target != '.') {
                bool target_white = (target >= 'A' && target <= 'Z');
                if (target_white != white_piece) {
                    moves.push_back(positionToSquare(new_row, new_col));
                }
            }
            // En passant
            else if (positionToSquare(new_row, new_col) == board.getEnPassantTarget()) {
                moves.push_back(positionToSquare(new_row, new_col));
            }
        }
    }

    return moves;
}

std::vector<std::string> MoveGenerator::generateRookMoves(const Board& board, int row, int col, bool white_piece) {
    std::vector<std::string> moves;

    // Four directions: up, down, left, right
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (auto& dir : directions) {
        int dr = dir[0], dc = dir[1];
        int new_row = row + dr, new_col = col + dc;

        while (new_row >= 0 && new_row < 8 && new_col >= 0 && new_col < 8) {
            char target = board.getPiece(new_row, new_col);

            if (target == '.') {
                moves.push_back(positionToSquare(new_row, new_col));
            } else {
                bool target_white = (target >= 'A' && target <= 'Z');
                if (target_white != white_piece) {
                    moves.push_back(positionToSquare(new_row, new_col));
                }
                break; // Can't move further in this direction
            }

            new_row += dr;
            new_col += dc;
        }
    }

    return moves;
}

std::vector<std::string> MoveGenerator::generateBishopMoves(const Board& board, int row, int col, bool white_piece) {
    std::vector<std::string> moves;

    // Four diagonal directions
    int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

    for (auto& dir : directions) {
        int dr = dir[0], dc = dir[1];
        int new_row = row + dr, new_col = col + dc;

        while (new_row >= 0 && new_row < 8 && new_col >= 0 && new_col < 8) {
            char target = board.getPiece(new_row, new_col);

            if (target == '.') {
                moves.push_back(positionToSquare(new_row, new_col));
            } else {
                bool target_white = (target >= 'A' && target <= 'Z');
                if (target_white != white_piece) {
                    moves.push_back(positionToSquare(new_row, new_col));
                }
                break; // Can't move further in this direction
            }

            new_row += dr;
            new_col += dc;
        }
    }

    return moves;
}

std::vector<std::string> MoveGenerator::generateKnightMoves(const Board& board, int row, int col, bool white_piece) {
    std::vector<std::string> moves;

    // Eight possible knight moves
    int knight_moves[8][2] = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2}, {1, 2}, {2, -1}, {2, 1}
    };

    for (auto& move : knight_moves) {
        int new_row = row + move[0];
        int new_col = col + move[1];

        if (new_row >= 0 && new_row < 8 && new_col >= 0 && new_col < 8) {
            char target = board.getPiece(new_row, new_col);

            if (target == '.') {
                moves.push_back(positionToSquare(new_row, new_col));
            } else {
                bool target_white = (target >= 'A' && target <= 'Z');
                if (target_white != white_piece) {
                    moves.push_back(positionToSquare(new_row, new_col));
                }
            }
        }
    }

    return moves;
}

std::vector<std::string> MoveGenerator::generateQueenMoves(const Board& board, int row, int col, bool white_piece) {
    auto rook_moves = generateRookMoves(board, row, col, white_piece);
    auto bishop_moves = generateBishopMoves(board, row, col, white_piece);

    rook_moves.insert(rook_moves.end(), bishop_moves.begin(), bishop_moves.end());
    return rook_moves;
}

std::vector<std::string> MoveGenerator::generateKingMoves(const Board& board, int row, int col, bool white_piece) {
    std::vector<std::string> moves;

    // Eight directions around the king
    int directions[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };

    for (auto& dir : directions) {
        int new_row = row + dir[0];
        int new_col = col + dir[1];

        if (new_row >= 0 && new_row < 8 && new_col >= 0 && new_col < 8) {
            char target = board.getPiece(new_row, new_col);

            if (target == '.') {
                moves.push_back(positionToSquare(new_row, new_col));
            } else {
                bool target_white = (target >= 'A' && target <= 'Z');
                if (target_white != white_piece) {
                    moves.push_back(positionToSquare(new_row, new_col));
                }
            }
        }
    }

    // Castling moves
    if (!board.isInCheck(white_piece)) {
        // Kingside castling
        if (board.canCastleKingside(white_piece)) {
            if (board.getPiece(row, col + 1) == '.' && board.getPiece(row, col + 2) == '.') {
                // Check if squares are not under attack
                Board temp_board = board.copy();
                temp_board.makeMove(positionToSquare(row, col) + positionToSquare(row, col + 1));
                if (!temp_board.isInCheck(white_piece)) {
                    moves.push_back(positionToSquare(row, col + 2));
                }
            }
        }

        // Queenside castling
        if (board.canCastleQueenside(white_piece)) {
            if (board.getPiece(row, col - 1) == '.' && board.getPiece(row, col - 2) == '.' &&
                board.getPiece(row, col - 3) == '.') {
                // Check if squares are not under attack
                Board temp_board = board.copy();
                temp_board.makeMove(positionToSquare(row, col) + positionToSquare(row, col - 1));
                if (!temp_board.isInCheck(white_piece)) {
                    moves.push_back(positionToSquare(row, col - 2));
                }
            }
        }
    }

    return moves;
}

// Legal move validation is now done directly in generateLegalMoves()
// by testing each move on a copy of the board and checking if it leaves the king in check
