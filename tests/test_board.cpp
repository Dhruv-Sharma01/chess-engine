#include "../src/board.h"
#include "../src/movegen.h"
#include <iostream>
#include <cassert>

void test_board_initialization() {
    std::cout << "Testing board initialization..." << std::endl;

    Board board;

    // Test starting position
    assert(board.getPiece(0, 0) == 'r');  // Black rook on a8
    assert(board.getPiece(0, 4) == 'k');  // Black king on e8
    assert(board.getPiece(7, 0) == 'R');  // White rook on a1
    assert(board.getPiece(7, 4) == 'K');  // White king on e1
    assert(board.getPiece(1, 0) == 'p');  // Black pawn on a7
    assert(board.getPiece(6, 0) == 'P');  // White pawn on a2

    // Test empty squares
    assert(board.getPiece(3, 3) == '.');  // Empty square on d5

    // Test turn
    assert(board.isWhiteToMove() == true);

    std::cout << "Board initialization tests passed!" << std::endl;
}

void test_basic_moves() {
    std::cout << "Testing basic moves..." << std::endl;

    Board board;

    // Test pawn move
    bool success = board.makeMove("e2e4");
    assert(success == true);
    assert(board.getPiece(6, 4) == '.');  // e2 should be empty
    assert(board.getPiece(4, 4) == 'P');  // e4 should have white pawn
    assert(board.isWhiteToMove() == false);  // Should be black's turn

    // Test black pawn move
    success = board.makeMove("e7e5");
    assert(success == true);
    assert(board.getPiece(1, 4) == '.');  // e7 should be empty
    assert(board.getPiece(3, 4) == 'p');  // e5 should have black pawn
    assert(board.isWhiteToMove() == true);  // Should be white's turn

    std::cout << "Basic move tests passed!" << std::endl;
}

void test_move_generation() {
    std::cout << "Testing move generation..." << std::endl;

    Board board;

    // Test initial position move generation
    auto legal_moves = MoveGenerator::generateLegalMoves(board, true);
    assert(legal_moves.size() == 20);  // 16 pawn moves + 4 knight moves

    // Make a move and test again
    board.makeMove("e2e4");
    legal_moves = MoveGenerator::generateLegalMoves(board, false);
    assert(legal_moves.size() == 20);  // Black should also have 20 moves

    std::cout << "Move generation tests passed!" << std::endl;
}

void test_fen_parsing() {
    std::cout << "Testing FEN parsing..." << std::endl;

    // Test starting position FEN
    std::string start_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Board board(start_fen);

    assert(board.getPiece(0, 0) == 'r');
    assert(board.getPiece(7, 4) == 'K');
    assert(board.isWhiteToMove() == true);

    // Test FEN generation
    std::string generated_fen = board.toFEN();
    assert(generated_fen == start_fen);

    std::cout << "FEN parsing tests passed!" << std::endl;
}

int main() {
    std::cout << "Running Board Tests..." << std::endl;

    try {
        test_board_initialization();
        test_basic_moves();
        test_move_generation();
        test_fen_parsing();

        std::cout << "\nAll board tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}