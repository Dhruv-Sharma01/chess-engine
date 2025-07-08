#ifndef EVAL_H
#define EVAL_H

class Board;

class Evaluator {
public:
    // Main evaluation function
    static int evaluate(const Board& board, bool white_perspective);

    // Piece value functions
    static int getPieceValue(char piece);
    static int getMaterialValue(const Board& board, bool white_player);

    // Positional evaluation
    static int getPositionalValue(const Board& board, bool white_player);
    static int getPieceSquareValue(char piece, int row, int col);

    // Pawn structure evaluation
    static int evaluatePawnStructure(const Board& board, bool white_player);
    static bool isDoubledPawn(const Board& board, int col, bool white_player);
    static bool isIsolatedPawn(const Board& board, int row, int col, bool white_player);
    static bool isPassedPawn(const Board& board, int row, int col, bool white_player);

    // King safety evaluation
    static int evaluateKingSafety(const Board& board, bool white_player);

    // Piece mobility evaluation
    static int evaluateMobility(const Board& board, bool white_player);

    // Endgame evaluation
    static bool isEndgame(const Board& board);
    static int evaluateEndgame(const Board& board, bool white_player);

private:
    // Piece-square tables
    static const int PAWN_TABLE[8][8];
    static const int KNIGHT_TABLE[8][8];
    static const int BISHOP_TABLE[8][8];
    static const int ROOK_TABLE[8][8];
    static const int QUEEN_TABLE[8][8];
    static const int KING_MIDDLE_GAME_TABLE[8][8];
    static const int KING_END_GAME_TABLE[8][8];

    // Piece values
    static const int PIECE_VALUES[6];
};

#endif // EVAL_H