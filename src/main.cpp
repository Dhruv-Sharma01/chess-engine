#include "cli.h"
#include "board.h"
#include "search.h"
#include "eval.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    try {
        ChessInterface chess_interface;

        // Parse command line arguments
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];

            if (arg == "--depth" && i + 1 < argc) {
                int depth = std::stoi(argv[++i]);
                chess_interface.setSearchDepth(depth);
                std::cout << "Search depth set to " << depth << "\n";
            } else if (arg == "--time" && i + 1 < argc) {
                int time_ms = std::stoi(argv[++i]);
                chess_interface.setSearchTime(time_ms);
                std::cout << "Search time set to " << time_ms << "ms\n";
            } else if (arg == "--black") {
                chess_interface.setHumanColor(false);
                std::cout << "Human will play as Black\n";
            } else if (arg == "--help" || arg == "-h") {
                std::cout << "Chess Engine v1.0\n";
                std::cout << "Usage: " << argv[0] << " [options]\n";
                std::cout << "\nOptions:\n";
                std::cout << "  --depth <n>    Set search depth (default: 6)\n";
                std::cout << "  --time <ms>    Set search time in milliseconds (default: 5000)\n";
                std::cout << "  --black        Play as black (default: white)\n";
                std::cout << "  --help, -h     Show this help message\n";
                std::cout << "\nDuring the game, type 'help' for available commands.\n";
                return 0;
            } else {
                std::cout << "Unknown option: " << arg << "\n";
                std::cout << "Use --help for usage information.\n";
                return 1;
            }
        }

        // Run the chess interface
        chess_interface.run();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }

    return 0;
}