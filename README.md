# Chess Engine

A Defensive chess engine written in C++ featuring alpha-beta pruning, comprehensive move generation, and a clean command-line interface.

## Features

- **Alpha-Beta Pruning** - Efficient minimax search with pruning
- **Iterative Deepening** - Gradually increases search depth
- **Transposition Tables** - Position caching for improved performance
- **Complete Move Generation** - All piece types, castling, en passant, promotion
- **Legal Move Validation** - Proper check detection and evasion
- **Position Management** - New game and move history
- **Command-Line Interface** - User-friendly terminal interaction

## Quick Start

### Compilation

```bash
g++ -std=c++17 -I. -Isrc src/*.cpp -o chess_engine
```

### Running the Engine

```bash
./chess_engine                    # Start with default settings
./chess_engine --black            # Play as black
./chess_engine --depth 8          # Set search depth
./chess_engine --time 3000        # Set thinking time (ms)
./chess_engine --help             # Show all options
```

## 🎮 How to Play

1. **Choose your color** when prompted (w/b)
2. **Make moves** using long algebraic notation:
   - `e2e4` - Move pawn from e2 to e4
   - `g1f3` - Move knight from g1 to f3
   - `o-o` - Castle kingside
   - `o-o-o` - Castle queenside

3. **Use commands** during the game:
   - `help` - Show available commands
   - `moves` - Display all legal moves
   - `new` - Start a new game
   - `quit` - Exit the game

## File Structure

```
src/
├── board.h/cpp      # Board representation and game state
├── movegen.h/cpp    # Move generation for all pieces
├── search.h/cpp     # Alpha-beta search with pruning
├── eval.h/cpp       # Position evaluation function
├── cli.h/cpp        # Command-line interface
└── main.cpp         # Application entry point
```

## Testing

```bash
# Compile and run tests
g++ -std=c++17 -I. -Isrc src/board.cpp src/movegen.cpp src/eval.cpp src/search.cpp src/cli.cpp tests/test_board.cpp -o test_board
./test_board

g++ -std=c++17 -I. -Isrc src/board.cpp src/movegen.cpp src/eval.cpp src/search.cpp src/cli.cpp tests/test_movegen.cpp -o test_movegen
./test_movegen
```

## Technical Details

- **Language**: C++17
- **Search Algorithm**: Alpha-beta pruning with iterative deepening
- **Move Generation**: Pseudo-legal moves filtered to legal moves
- **Evaluation**: Material + positional + pawn structure + mobility
- **Performance**: ~1000+ nodes/second on modern hardware

## Engine Strength

- **Tactical**: Correctly handles check, checkmate, stalemate
- **Strategic**: Basic positional evaluation with piece-square tables
- **Search Depth**: Configurable (default: 6 ply)
- **Time Control**: Configurable thinking time

## Configuration

The engine supports various command-line options:

- `--depth <n>` - Set maximum search depth (1-10)
- `--time <ms>` - Set maximum thinking time in milliseconds
- `--black` - Play as black pieces
- `--help` - Display help information

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is open source and available under the [MIT License](LICENSE).

## Acknowledgments

- Built with modern C++17 features
- Inspired by classical chess programming techniques
- Uses standard alpha-beta pruning algorithms

---

**Enjoy playing chess!**
