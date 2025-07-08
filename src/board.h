#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <set>

class Board {
private:
    std::vector<std::string> board_;
    bool white_to_move_;
    bool white_can_castle_kingside_;
    bool white_can_castle_queenside_;
    bool black_can_castle_kingside_;
    bool black_can_castle_queenside_;
    std::string en_passant_target_;
    int halfmove_clock_;
    int fullmove_number_;

    static const std::set<char> WHITE_PIECES;
    static const std::set<char> BLACK_PIECES;

    bool isValidSquare(int row, int col) const;
    bool isWhitePiece(char piece) const;
    bool isBlackPiece(char piece) const;
    bool isOpponentPiece(char piece, bool white_player) const;
    bool isFriendlyPiece(char piece, bool white_player) const;
    bool canPieceAttackSquare(char piece, int from_row, int from_col, int to_row, int to_col) const;
    bool isPathClear(int from_row, int from_col, int to_row, int to_col) const;

public:
    Board();

    // Board display
    void print() const;

    // Position queries
    char getPiece(int row, int col) const;
    char getPiece(const std::string& square) const;
    std::vector<int> getPosition(const std::string& square) const;
    std::string getSquare(int row, int col) const;

    // Game state
    bool isWhiteToMove() const { return white_to_move_; }
    void setWhiteToMove(bool white) { white_to_move_ = white; }

    // Move operations
    bool makeMove(const std::string& move);
    void unmakeMove(const std::string& move, char captured_piece,
                   bool old_castling_rights[4], const std::string& old_en_passant);
    bool isLegalMove(const std::string& move) const;

    // Check and mate detection
    bool isInCheck(bool white_player) const;
    bool isCheckmate(bool white_player) const;
    bool isStalemate(bool white_player) const;
    bool isGameOver() const;

    // Castling and en passant
    bool canCastleKingside(bool white_player) const;
    bool canCastleQueenside(bool white_player) const;
    std::string getEnPassantTarget() const { return en_passant_target_; }

    // Move generation support
    std::vector<std::string> getAllLegalMoves(bool white_player) const;
    std::vector<std::string> getPseudoLegalMoves(const std::string& square) const;

    // Copy for search
    Board copy() const;

    // Access to board state for move generation
    const std::vector<std::string>& getBoardState() const { return board_; }
};

#endif // BOARD_H