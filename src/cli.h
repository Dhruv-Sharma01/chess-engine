#ifndef CLI_H
#define CLI_H

#include <string>
#include <vector>

class Board;
class SearchEngine;

class ChessInterface {
private:
    Board* board_;
    SearchEngine* search_engine_;
    bool human_is_white_;
    bool game_over_;
    bool color_set_by_args_;

    // Input parsing
    std::string parseMove(const std::string& input);
    bool isValidMoveFormat(const std::string& move);
    std::string algebraicToLongNotation(const std::string& algebraic);

    // Game flow
    void displayWelcome();
    void displayHelp();
    void displayGameState();
    void displayMoveHistory();

    // Move handling
    bool handleHumanMove();
    bool handleComputerMove();
    void displayLegalMoves();

    // Utility
    void clearScreen();
    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);

public:
    ChessInterface();
    ~ChessInterface();

    // Main game loop
    void run();

    // Configuration
    void setHumanColor(bool white) { human_is_white_ = white; color_set_by_args_ = true; }
    void setSearchDepth(int depth);
    void setSearchTime(int milliseconds);

    // Game control
    void newGame();
    void loadPosition(const std::string& fen);
    void savePosition();
    void undoMove();
    void quit();
};

#endif // CLI_H