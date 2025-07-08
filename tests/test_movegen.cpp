#include "../src/board.h"
#include "../src/movegen.h"
#include "../src/search.h"
#include "../src/eval.h"
#include <iostream>
#include <cassert>

void test_pawn_moves() {
    std::cout << "Testing pawn move generation..." << std::endl;

    Board board;

    // Test white pawn moves from starting position
    auto moves = MoveGenerator::generatePseudoLegalMoves(board, 6, 4); // e2
    assert(moves.size() == 2); // e3 and e4

    // Test black pawn moves
    moves = MoveGenerator::generatePseudoLegalMoves(board, 1, 4); // e7
    assert(moves.size() == 2); // e6 and e5

    std::cout << "Pawn move tests passed!" << std::endl;
}

void test_knight_moves() {
    std::cout << "Testing knight move generation..." << std::endl;

    Board board;

    // Test white knight moves from starting position
    auto moves = MoveGenerator::generatePseudoLegalMoves(board, 7, 1); // b1
    assert(moves.size() == 2); // a3 and c3

    moves = MoveGenerator::generatePseudoLegalMoves(board, 7, 6); // g1
    assert(moves.size() == 2); // f3 and h3

    std::cout << "Knight move tests passed!" << std::endl;
}

void test_check_detection() {
    std::cout << "Testing check detection..." << std::endl;

    // Create a position where white king is in check
    std::string fen = "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKB1R w KQkq - 0 3";
    Board board(fen);

    // Move white king to expose it to check
    board.makeMove("e1e2");

    // The king should not be in check in this position
    assert(board.isInCheck(true) == false);

    std::cout << "Check detection tests passed!" << std::endl;
}

void test_search_engine() {
    std::cout << "Testing search engine..." << std::endl;

    Board board;
    SearchEngine engine(3, std::chrono::milliseconds(1000)); // Depth 3, 1 second

    auto result = engine.findBestMove(board, true);

    // Should find a move
    assert(!result.best_move.empty());
    assert(result.depth > 0);
    assert(result.nodes_searched > 0);

    std::cout << "Found move: " << result.best_move
              << " (score: " << result.score
              << ", depth: " << result.depth
              << ", nodes: " << result.nodes_searched << ")" << std::endl;

    std::cout << "Search engine tests passed!" << std::endl;
}

void test_evaluation() {
    std::cout << "Testing evaluation function..." << std::endl;

    Board board;

    // Starting position should be roughly equal
    int eval = Evaluator::evaluate(board, true);
    assert(abs(eval) < 100); // Should be close to 0

    // Test material counting
    int white_material = Evaluator::getMaterialValue(board, true);
    int black_material = Evaluator::getMaterialValue(board, false);
    assert(white_material == black_material); // Should be equal in starting position

    std::cout << "Starting position evaluation: " << eval << std::endl;
    std::cout << "White material: " << white_material << std::endl;
    std::cout << "Black material: " << black_material << std::endl;

    std::cout << "Evaluation tests passed!" << std::endl;
}

void test_game_scenarios() {
    std::cout << "Testing game scenarios..." << std::endl;

    // Test a simple game sequence
    Board board;

    // Scholar's mate setup
    assert(board.makeMove("e2e4"));
    assert(board.makeMove("e7e5"));
    assert(board.makeMove("d1h5"));
    assert(board.makeMove("b8c6"));
    assert(board.makeMove("f1c4"));
    assert(board.makeMove("d7d6"));

    // This should be a legal position
    assert(!board.isCheckmate(false));
    assert(!board.isStalemate(false));

    std::cout << "Game scenario tests passed!" << std::endl;
}

int main() {
    std::cout << "Running Move Generation and Engine Tests..." << std::endl;

    try {
        test_pawn_moves();
        test_knight_moves();
        test_check_detection();
        test_evaluation();
        test_search_engine();
        test_game_scenarios();

        std::cout << "\nAll tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}