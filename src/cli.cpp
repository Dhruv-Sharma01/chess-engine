#include "cli.h"
#include "board.h"
#include "search.h"
#include "movegen.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

ChessInterface::ChessInterface()
    : board_(new Board()), search_engine_(new SearchEngine()),
      human_is_white_(true), game_over_(false), color_set_by_args_(false) {
}

ChessInterface::~ChessInterface() {
    delete board_;
    delete search_engine_;
}

void ChessInterface::run() {
    displayWelcome();

    // Only ask for color if not already set via command line
    if (color_set_by_args_) {
        if (human_is_white_) {
            std::cout << "You are playing as White.\n";
        } else {
            std::cout << "You are playing as Black.\n";
        }
    } else {
        std::cout << "Choose your color (w/b): ";
        std::string color_choice;
        std::getline(std::cin, color_choice);
        color_choice = trim(color_choice);

        if (!color_choice.empty() && std::tolower(color_choice[0]) == 'b') {
            human_is_white_ = false;
            std::cout << "You are playing as Black.\n";
        } else {
            human_is_white_ = true;
            std::cout << "You are playing as White.\n";
        }
    }

    std::cout << "\nStarting new game...\n";
    newGame();

    // Main game loop
    while (!game_over_) {
        displayGameState();

        if (board_->isWhiteToMove() == human_is_white_) {
            if (!handleHumanMove()) {
                break; // User quit or error
            }
        } else {
            if (!handleComputerMove()) {
                break; // Game over or error
            }
        }

        // Check for game over conditions
        if (board_->isCheckmate(board_->isWhiteToMove())) {
            displayGameState();
            std::cout << "\nCheckmate! "
                      << (board_->isWhiteToMove() ? "Black" : "White")
                      << " wins!\n";
            game_over_ = true;
        } else if (board_->isStalemate(board_->isWhiteToMove())) {
            displayGameState();
            std::cout << "\nStalemate! The game is a draw.\n";
            game_over_ = true;
        }
    }

    std::cout << "Thanks for playing!\n";
}

void ChessInterface::displayWelcome() {
    clearScreen();
    std::cout << "========================================\n";
    std::cout << "         CHESS ENGINE v1.0\n";
    std::cout << "========================================\n";
    std::cout << "\nWelcome to the Chess Engine!\n";
    std::cout << "Type 'help' during the game for commands.\n";
    std::cout << "Use algebraic notation for moves (e.g., e2e4, Nf3, O-O)\n\n";
}

void ChessInterface::displayHelp() {
    std::cout << "\n=== COMMANDS ===\n";
    std::cout << "help       - Show this help\n";
    std::cout << "moves      - Show all legal moves\n";
    std::cout << "new        - Start a new game\n";
    std::cout << "undo       - Undo last move\n";
    std::cout << "quit       - Exit the game\n";
    std::cout << "\n=== MOVE FORMAT ===\n";
    std::cout << "Long notation: e2e4, g1f3, e1g1 (castling)\n";
    std::cout << "Standard notation: e4, Nf3, O-O, O-O-O\n\n";
}

void ChessInterface::displayGameState() {
    clearScreen();
    board_->print();

    if (board_->isInCheck(board_->isWhiteToMove())) {
        std::cout << "\n*** CHECK! ***\n";
    }
}

bool ChessInterface::handleHumanMove() {
    std::cout << "\nYour move: ";
    std::string input;
    std::getline(std::cin, input);
    input = trim(input);

    if (input.empty()) {
        return true;
    }

    if (input == "help") {
        displayHelp();
        return true;
    } else if (input == "moves") {
        displayLegalMoves();
        return true;
    } else if (input == "new") {
        newGame();
        return true;
    } else if (input == "undo") {
        undoMove();
        return true;
    // save and load commands removed
    } else if (input == "quit" || input == "exit") {
        return false; // Exit game
    }

    // Try to parse as a move
    std::string move = parseMove(input);
    if (move.empty()) {
        std::cout << "Invalid move format. Type 'help' for assistance.\n";
        return true;
    }

    // Validate and make the move
    auto legal_moves = MoveGenerator::generateLegalMoves(*board_, board_->isWhiteToMove());
    bool is_legal = std::find(legal_moves.begin(), legal_moves.end(), move) != legal_moves.end();

    if (!is_legal) {
        std::cout << "Illegal move. Type 'moves' to see legal moves.\n";
        return true;
    }

    if (board_->makeMove(move)) {
        std::cout << "Move played: " << move << "\n";
    } else {
        std::cout << "Failed to make move.\n";
    }

    return true;
}

bool ChessInterface::handleComputerMove() {
    std::cout << "\nComputer is thinking...\n";

    auto result = search_engine_->findBestMove(*board_, board_->isWhiteToMove());

    if (result.best_move.empty()) {
        std::cout << "Computer has no legal moves.\n";
        return false;
    }

    if (board_->makeMove(result.best_move)) {
        std::cout << "Computer plays: " << result.best_move
                  << " (score: " << result.score
                  << ", depth: " << result.depth
                  << ", nodes: " << result.nodes_searched
                  << ", time: " << result.time_taken.count() << "ms)\n";
    } else {
        std::cout << "Computer failed to make move.\n";
        return false;
    }

    return true;
}

void ChessInterface::displayLegalMoves() {
    auto legal_moves = MoveGenerator::generateLegalMoves(*board_, board_->isWhiteToMove());

    if (legal_moves.empty()) {
        std::cout << "No legal moves available.\n";
        return;
    }

    std::cout << "\nLegal moves (" << legal_moves.size() << "):\n";
    int count = 0;
    for (const auto& move : legal_moves) {
        std::cout << std::setw(6) << move << " ";
        if (++count % 8 == 0) std::cout << "\n";
    }
    if (count % 8 != 0) std::cout << "\n";
}

std::string ChessInterface::parseMove(const std::string& input) {
    std::string move = trim(input);

    // Convert to lowercase for easier parsing
    std::transform(move.begin(), move.end(), move.begin(), ::tolower);

    // Handle castling
    if (move == "o-o" || move == "0-0") {
        return board_->isWhiteToMove() ? "e1g1" : "e8g8";
    } else if (move == "o-o-o" || move == "0-0-0") {
        return board_->isWhiteToMove() ? "e1c1" : "e8c8";
    }

    // If already in long notation (e.g., e2e4), validate and return
    if (isValidMoveFormat(move)) {
        return move;
    }

    // Try to convert from algebraic notation
    return algebraicToLongNotation(move);
}

bool ChessInterface::isValidMoveFormat(const std::string& move) {
    if (move.length() < 4 || move.length() > 5) return false;

    // Check format: [a-h][1-8][a-h][1-8][promotion]?
    if (move[0] < 'a' || move[0] > 'h') return false;
    if (move[1] < '1' || move[1] > '8') return false;
    if (move[2] < 'a' || move[2] > 'h') return false;
    if (move[3] < '1' || move[3] > '8') return false;

    if (move.length() == 5) {
        // Check promotion piece
        char promo = move[4];
        if (promo != 'q' && promo != 'r' && promo != 'b' && promo != 'n') {
            return false;
        }
    }

    return true;
}

std::string ChessInterface::algebraicToLongNotation(const std::string& algebraic) {
    // Simplified algebraic notation parser
    // This is a basic implementation - a full parser would be more complex

    if (algebraic.length() < 2) return "";

    // Simple pawn moves (e.g., "e4")
    if (algebraic.length() == 2 && algebraic[0] >= 'a' && algebraic[0] <= 'h' &&
        algebraic[1] >= '1' && algebraic[1] <= '8') {

        std::string to = algebraic;

        // Find the pawn that can move to this square
        auto legal_moves = MoveGenerator::generateLegalMoves(*board_, board_->isWhiteToMove());
        for (const auto& move : legal_moves) {
            if (move.length() >= 4 && move.substr(2, 2) == to) {
                auto from_pos = MoveGenerator::squareToPosition(move.substr(0, 2));
                char piece = board_->getPiece(from_pos[0], from_pos[1]);
                if (std::tolower(piece) == 'p') {
                    return move;
                }
            }
        }
    }

    return ""; // Could not parse
}

void ChessInterface::newGame() {
    delete board_;
    board_ = new Board();
    game_over_ = false;
    std::cout << "New game started.\n";
}

// loadPosition() and savePosition() functions removed

void ChessInterface::undoMove() {
    std::cout << "Undo not implemented yet.\n";
}

void ChessInterface::setSearchDepth(int depth) {
    search_engine_->setMaxDepth(depth);
}

void ChessInterface::setSearchTime(int milliseconds) {
    search_engine_->setMaxTime(std::chrono::milliseconds(milliseconds));
}

void ChessInterface::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

std::string ChessInterface::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> ChessInterface::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}