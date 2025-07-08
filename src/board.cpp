#include "board.h"
#include "movegen.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

const std::set<char> Board::WHITE_PIECES = {'P', 'N', 'B', 'R', 'Q', 'K'};
const std::set<char> Board::BLACK_PIECES = {'p', 'n', 'b', 'r', 'q', 'k'};

Board::Board() {
    // Initialize standard starting position
    board_ = {
        "rnbqkbnr",  // rank 8 → row 0
        "pppppppp",  // rank 7 → row 1
        "........",  // rank 6 → row 2
        "........",  // rank 5 → row 3
        "........",  // rank 4 → row 4
        "........",  // rank 3 → row 5
        "PPPPPPPP",  // rank 2 → row 6
        "RNBQKBNR"   // rank 1 → row 7
    };

    white_to_move_ = true;
    white_can_castle_kingside_ = true;
    white_can_castle_queenside_ = true;
    black_can_castle_kingside_ = true;
    black_can_castle_queenside_ = true;
    en_passant_target_ = "-";
    halfmove_clock_ = 0;
    fullmove_number_ = 1;
}

Board::Board(const std::string& fen) {
    // Parse FEN notation
    std::istringstream iss(fen);
    std::string board_part, active_color, castling, en_passant, halfmove, fullmove;

    iss >> board_part >> active_color >> castling >> en_passant >> halfmove >> fullmove;

    // Parse board position
    board_.clear();
    board_.resize(8, std::string(8, '.'));

    int row = 0, col = 0;
    for (char c : board_part) {
        if (c == '/') {
            row++;
            col = 0;
        } else if (std::isdigit(c)) {
            col += (c - '0');
        } else {
            board_[row][col] = c;
            col++;
        }
    }

    // Parse game state
    white_to_move_ = (active_color == "w");
    white_can_castle_kingside_ = castling.find('K') != std::string::npos;
    white_can_castle_queenside_ = castling.find('Q') != std::string::npos;
    black_can_castle_kingside_ = castling.find('k') != std::string::npos;
    black_can_castle_queenside_ = castling.find('q') != std::string::npos;
    en_passant_target_ = en_passant;
    halfmove_clock_ = std::stoi(halfmove);
    fullmove_number_ = std::stoi(fullmove);
}

void Board::print() const {
    std::cout << "\n  a b c d e f g h\n";
    for (int i = 0; i < 8; i++) {
        std::cout << (8 - i) << " ";
        for (int j = 0; j < 8; j++) {
            std::cout << board_[i][j] << " ";
        }
        std::cout << (8 - i) << "\n";
    }
    std::cout << "  a b c d e f g h\n";
    std::cout << "Turn: " << (white_to_move_ ? "White" : "Black") << "\n";
}

bool Board::isValidSquare(int row, int col) const {
    return row >= 0 && row < 8 && col >= 0 && col < 8;
}

bool Board::isWhitePiece(char piece) const {
    return WHITE_PIECES.count(piece) > 0;
}

bool Board::isBlackPiece(char piece) const {
    return BLACK_PIECES.count(piece) > 0;
}

bool Board::isOpponentPiece(char piece, bool white_player) const {
    return white_player ? isBlackPiece(piece) : isWhitePiece(piece);
}

bool Board::isFriendlyPiece(char piece, bool white_player) const {
    return white_player ? isWhitePiece(piece) : isBlackPiece(piece);
}

char Board::getPiece(int row, int col) const {
    if (!isValidSquare(row, col)) return '\0';
    return board_[row][col];
}

char Board::getPiece(const std::string& square) const {
    auto pos = getPosition(square);
    return getPiece(pos[0], pos[1]);
}

std::vector<int> Board::getPosition(const std::string& square) const {
    if (square.length() != 2) return {-1, -1};
    int file = square[0] - 'a';  // column
    int rank = square[1] - '1';  // row (from bottom)
    return {7 - rank, file};     // convert to array indices
}

std::string Board::getSquare(int row, int col) const {
    if (!isValidSquare(row, col)) return "";
    char file = 'a' + col;
    char rank = '1' + (7 - row);
    return std::string{file, rank};
}

bool Board::makeMove(const std::string& move) {
    if (move.length() < 4) return false;

    std::string from = move.substr(0, 2);
    std::string to = move.substr(2, 2);

    auto from_pos = getPosition(from);
    auto to_pos = getPosition(to);

    if (from_pos[0] == -1 || to_pos[0] == -1) return false;

    char piece = getPiece(from_pos[0], from_pos[1]);
    if (piece == '.') return false;

    // Check if it's the correct player's turn
    if (white_to_move_ != isWhitePiece(piece)) return false;

    // Make the move
    board_[to_pos[0]][to_pos[1]] = piece;
    board_[from_pos[0]][from_pos[1]] = '.';

    // Handle special moves
    // En passant capture
    if (piece == 'P' || piece == 'p') {
        if (to == en_passant_target_) {
            // Remove captured pawn
            int captured_row = white_to_move_ ? to_pos[0] + 1 : to_pos[0] - 1;
            board_[captured_row][to_pos[1]] = '.';
        }

        // Set en passant target for double pawn move
        if (abs(from_pos[0] - to_pos[0]) == 2) {
            int ep_row = (from_pos[0] + to_pos[0]) / 2;
            en_passant_target_ = getSquare(ep_row, to_pos[1]);
        } else {
            en_passant_target_ = "-";
        }

        // Pawn promotion (simplified - always promote to queen)
        if ((piece == 'P' && to_pos[0] == 0) || (piece == 'p' && to_pos[0] == 7)) {
            board_[to_pos[0]][to_pos[1]] = white_to_move_ ? 'Q' : 'q';
        }
    } else {
        en_passant_target_ = "-";
    }

    // Handle castling
    if (piece == 'K' || piece == 'k') {
        if (abs(to_pos[1] - from_pos[1]) == 2) {
            // Castling move
            if (to_pos[1] == 6) { // Kingside
                int rook_row = piece == 'K' ? 7 : 0;
                board_[rook_row][5] = board_[rook_row][7];
                board_[rook_row][7] = '.';
            } else if (to_pos[1] == 2) { // Queenside
                int rook_row = piece == 'K' ? 7 : 0;
                board_[rook_row][3] = board_[rook_row][0];
                board_[rook_row][0] = '.';
            }
        }

        // Update castling rights
        if (piece == 'K') {
            white_can_castle_kingside_ = false;
            white_can_castle_queenside_ = false;
        } else {
            black_can_castle_kingside_ = false;
            black_can_castle_queenside_ = false;
        }
    }

    // Update castling rights for rook moves
    if (piece == 'R' || piece == 'r') {
        if (from == "a1") white_can_castle_queenside_ = false;
        else if (from == "h1") white_can_castle_kingside_ = false;
        else if (from == "a8") black_can_castle_queenside_ = false;
        else if (from == "h8") black_can_castle_kingside_ = false;
    }

    // Update move counters
    if (piece == 'P' || piece == 'p' || getPiece(to_pos[0], to_pos[1]) != '.') {
        halfmove_clock_ = 0;
    } else {
        halfmove_clock_++;
    }

    if (!white_to_move_) {
        fullmove_number_++;
    }

    white_to_move_ = !white_to_move_;
    return true;
}

Board Board::copy() const {
    Board new_board;
    new_board.board_ = board_;
    new_board.white_to_move_ = white_to_move_;
    new_board.white_can_castle_kingside_ = white_can_castle_kingside_;
    new_board.white_can_castle_queenside_ = white_can_castle_queenside_;
    new_board.black_can_castle_kingside_ = black_can_castle_kingside_;
    new_board.black_can_castle_queenside_ = black_can_castle_queenside_;
    new_board.en_passant_target_ = en_passant_target_;
    new_board.halfmove_clock_ = halfmove_clock_;
    new_board.fullmove_number_ = fullmove_number_;
    return new_board;
}

bool Board::isInCheck(bool white_player) const {
    // Find the king
    char king = white_player ? 'K' : 'k';
    int king_row = -1, king_col = -1;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board_[i][j] == king) {
                king_row = i;
                king_col = j;
                break;
            }
        }
        if (king_row != -1) break;
    }

    if (king_row == -1) return false; // King not found

    // Check if any opponent piece can attack the king
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            char piece = board_[i][j];
            if (piece == '.' || isFriendlyPiece(piece, white_player)) continue;

            // Check if this piece can attack the king
            if (canPieceAttackSquare(piece, i, j, king_row, king_col)) {
                return true;
            }
        }
    }

    return false;
}

bool Board::canPieceAttackSquare(char piece, int from_row, int from_col, int to_row, int to_col) const {
    int dr = to_row - from_row;
    int dc = to_col - from_col;

    switch (std::tolower(piece)) {
        case 'p': {
            int direction = (piece == 'P') ? -1 : 1;
            return (dr == direction && abs(dc) == 1);
        }
        case 'r':
            return (dr == 0 || dc == 0) && isPathClear(from_row, from_col, to_row, to_col);
        case 'n':
            return (abs(dr) == 2 && abs(dc) == 1) || (abs(dr) == 1 && abs(dc) == 2);
        case 'b':
            return (abs(dr) == abs(dc)) && isPathClear(from_row, from_col, to_row, to_col);
        case 'q':
            return ((dr == 0 || dc == 0) || (abs(dr) == abs(dc))) &&
                   isPathClear(from_row, from_col, to_row, to_col);
        case 'k':
            return abs(dr) <= 1 && abs(dc) <= 1;
    }
    return false;
}

bool Board::isPathClear(int from_row, int from_col, int to_row, int to_col) const {
    int dr = (to_row > from_row) ? 1 : (to_row < from_row) ? -1 : 0;
    int dc = (to_col > from_col) ? 1 : (to_col < from_col) ? -1 : 0;

    int r = from_row + dr;
    int c = from_col + dc;

    while (r != to_row || c != to_col) {
        if (board_[r][c] != '.') return false;
        r += dr;
        c += dc;
    }

    return true;
}

std::string Board::toFEN() const {
    std::string fen;

    // Board position
    for (int i = 0; i < 8; i++) {
        int empty_count = 0;
        for (int j = 0; j < 8; j++) {
            if (board_[i][j] == '.') {
                empty_count++;
            } else {
                if (empty_count > 0) {
                    fen += std::to_string(empty_count);
                    empty_count = 0;
                }
                fen += board_[i][j];
            }
        }
        if (empty_count > 0) {
            fen += std::to_string(empty_count);
        }
        if (i < 7) fen += '/';
    }

    // Active color
    fen += white_to_move_ ? " w " : " b ";

    // Castling rights
    std::string castling;
    if (white_can_castle_kingside_) castling += 'K';
    if (white_can_castle_queenside_) castling += 'Q';
    if (black_can_castle_kingside_) castling += 'k';
    if (black_can_castle_queenside_) castling += 'q';
    if (castling.empty()) castling = "-";
    fen += castling + " ";

    // En passant target
    fen += en_passant_target_ + " ";

    // Halfmove clock and fullmove number
    fen += std::to_string(halfmove_clock_) + " " + std::to_string(fullmove_number_);

    return fen;
}

bool Board::isCheckmate(bool white_player) const {
    if (!isInCheck(white_player)) return false;

    auto legal_moves = MoveGenerator::generateLegalMoves(*this, white_player);
    return legal_moves.empty();
}

bool Board::isStalemate(bool white_player) const {
    if (isInCheck(white_player)) return false;

    auto legal_moves = MoveGenerator::generateLegalMoves(*this, white_player);
    return legal_moves.empty();
}

bool Board::isGameOver() const {
    return isCheckmate(white_to_move_) || isStalemate(white_to_move_);
}

bool Board::canCastleKingside(bool white_player) const {
    return white_player ? white_can_castle_kingside_ : black_can_castle_kingside_;
}

bool Board::canCastleQueenside(bool white_player) const {
    return white_player ? white_can_castle_queenside_ : black_can_castle_queenside_;
}

std::vector<std::string> Board::getAllLegalMoves(bool white_player) const {
    return MoveGenerator::generateLegalMoves(*this, white_player);
}

std::vector<std::string> Board::getPseudoLegalMoves(const std::string& square) const {
    auto pos = getPosition(square);
    if (pos[0] == -1) return {};
    return MoveGenerator::generatePseudoLegalMoves(*this, pos[0], pos[1]);
}

bool Board::isLegalMove(const std::string& move) const {
    auto legal_moves = getAllLegalMoves(white_to_move_);
    return std::find(legal_moves.begin(), legal_moves.end(), move) != legal_moves.end();
}

void Board::unmakeMove(const std::string& move, char captured_piece,
                      bool old_castling_rights[4], const std::string& old_en_passant) {
    // Simplified undo - in a full implementation this would restore the exact previous state
    // For now, this is a placeholder
}