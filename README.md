# Tiny Language Compiler

A small compiler implementation for the TINY programming language, written in C with an optional Python GUI interface. This project demonstrates the fundamental concepts of lexical analysis and parsing in compiler construction.

## Project Structure

This compiler consists of two main components:

**Scanner (Lexical Analyzer)**
- Reads TINY source code and breaks it into tokens
- Recognizes keywords, identifiers, numbers, and operators
- Outputs token information for the parser

**Parser (Syntax Analyzer)**
- Takes tokens from the scanner and builds a parse tree
- Implements the grammar rules of the TINY language
- Generates a visual representation of the program structure

## Building and Running

### Command Line Interface

The project includes a Makefile for easy compilation:

```bash
# Build both scanner and parser
make

# Run the complete compilation process
make run

# Clean compiled files
make clean
```

You can also run the components individually:

```bash
# Run just the scanner
./scanner

# Run just the parser
./parser
```

### Graphical Interface

For a more user-friendly experience, there's a Python GUI application:

```bash
# Make sure you have the required dependencies
pip install PySide6

# Run the GUI
python guii.py
```

The GUI provides a visual interface for editing TINY programs, running the compiler, and viewing both the token output and parse tree.

## Language Features

TINY supports the following language constructs:

- **Variables**: Simple identifiers for storing values
- **Numbers**: Integer literals
- **Arithmetic**: Addition (+), subtraction (-), multiplication (*), division (/)
- **Assignment**: Using the `:=` operator
- **Comparison**: Less than (<) and equality (=) operators
- **Control Flow**: 
  - Conditional statements with `if-then`
  - Loops with `repeat-until`
- **I/O Operations**: `read` and `write` statements

## File Organization

- `scanner.c` - Lexical analyzer implementation
- `parser.c` - Syntax analyzer implementation  
- `tokens.h` - Token type definitions and constants
- `token_strings.c` - String representations of tokens
- `input.tiny` - Sample TINY program for testing
- `guii.py` - Optional graphical user interface
- `run.sh` - Shell script for GUI setup
- `Makefile` - Build configuration

## How It Works

1. **Lexical Analysis**: The scanner reads the source file character by character, grouping them into meaningful tokens (keywords, identifiers, operators, etc.)

2. **Syntax Analysis**: The parser takes the stream of tokens and checks if they follow the grammar rules of TINY, building a parse tree in the process

3. **Output**: The compiler generates two files:
   - `tokens.txt` - List of all tokens found in the source
   - `tree.txt` - Visual representation of the parse tree
