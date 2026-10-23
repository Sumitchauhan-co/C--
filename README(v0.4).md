# C-- v0.4

C-- is a small interpreted language implemented in C++17. Version 0.4 adds conditional execution (`if`, `elif`, `else`) and console input using `ask()`.

## Features

- `let` for mutable variables and `fix` for immutable variables
- `say(...)` for output; multiple arguments are separated by spaces
- `ask()` for input; `ask("Prompt: ")` prints a prompt first
- `if`, `elif`, and `else` blocks using required `{}` braces
- `true`, `false`, comparisons, arithmetic, equality, and logical operators `and`, `or`, `not`
- `//` single-line comments and `/* ... */` block comments
- Semicolons are optional; newlines separate statements

## Example

```c--
let age = ask("How old are you? ")

if age >= 18 {
    say("You are an adult.")
} elif age >= 13 {
    say("You are a teenager.")
} else {
    say("You are a child.")
}
```

`ask()` reads one line. Text that parses entirely as a number becomes a number; `true` and `false` become booleans; all other input remains a string. Conditions must evaluate to booleans: use comparisons or `true`/`false` directly.

## Build and run

Requires CMake and a C++17 compiler.

```sh
cmake -S . -B build
cmake --build build
./build/cmm examples/age_check.cmm
```

On Windows, the executable may be at `build\\Debug\\cmm.exe` or `build\\cmm.exe`, depending on the CMake generator.

## Project structure

```text
include/                 Public declarations and interfaces
  ast/ast.hpp             AST node types for expressions and statements
  lexer/                  Token definitions and lexer interface
  parser/parser.hpp       Parser interface
  interpreter/            Interpreter interface
  runtime/                Value and environment interfaces
src/                     Implementations
  ast/ast.cpp             AST extension point (nodes currently are simple data structures)
  lexer/lexer.cpp         Converts source text into tokens
  parser/parser.cpp       Converts tokens into AST statements and expressions
  interpreter/            Evaluates expressions and executes statements
  runtime/                Implements values and variable storage
  main.cpp                Connects the stages and runs a source file
examples/                 Sample C-- programs
tests/                    Manual test checklist
```

## How the interpreter works

1. **Lexer** reads characters and produces tokens such as `LET`, `IF`, identifiers, numbers, and braces.
2. **Parser** applies operator precedence and builds AST nodes from tokens.
3. **AST** represents the program's expressions and statements independently of source text.
4. **Interpreter** evaluates expressions and executes statements, including conditional branches.
5. **Runtime environment** stores variable values and whether each binding is immutable.

## Current limitations

This is an educational interpreter, not a production language. Loops, user-defined functions, arrays/lists, imports, and block-local variable scope are not implemented. Strings use double quotes and currently do not support escape sequences. `fn` remains a planned keyword/syntax decision for a future version; functions are not implemented yet.

## Version history

- **v0.1** — arithmetic foundation
- **v0.2** — variables and assignments
- **v0.3** — booleans and comparisons
- **v0.4** — `if` / `elif` / `else` and `ask()` input, with modular source layout
