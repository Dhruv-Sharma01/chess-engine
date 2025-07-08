#include "src/board.h"
#include "src/eval.h"
#include <iostream>

int main() {
    // Test 1: Starting position should be ~0
    Board start_board;
    int start_eval = Evaluator::evaluate(start_board, true);
    std::cout << "Starting position evaluation: " << start_eval << std::endl;
    
    // Test 2: Create a position where White is up a knight
    Board test_board;
    // Remove black knight from b8
    // This is a hack for testing - normally we'd use proper move making
    
    std::cout << "Material values:" << std::endl;
    std::cout << "Pawn: " << Evaluator::getPieceValue('P') << std::endl;
    std::cout << "Knight: " << Evaluator::getPieceValue('N') << std::endl;
    std::cout << "Bishop: " << Evaluator::getPieceValue('B') << std::endl;
    std::cout << "Rook: " << Evaluator::getPieceValue('R') << std::endl;
    std::cout << "Queen: " << Evaluator::getPieceValue('Q') << std::endl;
    
    return 0;
}
