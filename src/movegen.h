#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <vector>
#include <string>

class Board; // Forward declaration

class MoveGenerator {
public:
    // Generate all legal moves for a player
    static std::vector<std::string> generateLegalMoves(const Board& board, bool white_player);

    // Generate pseudo-legal moves for a specific piece
    static std::vector<std::string> generatePseudoLegalMoves(const Board& board, int row, int col);

    // Individual piece move generators
    static std::vector<std::string> generatePawnMoves(const Board& board, int row, int col, bool white_piece);
    static std::vector<std::string> generateRookMoves(const Board& board, int row, int col, bool white_piece);
    static std::vector<std::string> generateBishopMoves(const Board& board, int row, int col, bool white_piece);
    static std::vector<std::string> generateKnightMoves(const Board& board, int row, int col, bool white_piece);
    static std::vector<std::string> generateKingMoves(const Board& board, int row, int col, bool white_piece);
    static std::vector<std::string> generateQueenMoves(const Board& board, int row, int col, bool white_piece);

    // Utility functions
    static std::string positionToSquare(int row, int col);
    static std::vector<int> squareToPosition(const std::string& square);

private:
    // Legal move validation is now done directly in generateLegalMoves()
};

#endif // MOVE_GEN_H
