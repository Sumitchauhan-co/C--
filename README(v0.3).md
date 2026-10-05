# C--

C-- is a small programming language written in C++.

The project is intentionally built incrementally to learn how programming languages work internally.

## Current Version

**v0.3 — Booleans & Comparisons**

v0.3 adds:
- Boolean values: `true`, `false`
- Comparisons: `>`, `>=`, `<`, `<=`
- Equality: `==`, `!=`
- Variables that can store integers or booleans
- `say` output for integers and booleans
- Runtime type errors
- Expression precedence for arithmetic and comparisons

Example:

```c--
let age = 20;
say age > 18;
say age == 20;
say age != 15;
say age <= 25;
let adult = age >= 18;
say adult;
```

Output:

```text
true
true
true
true
true
```

## Language Pipeline

```text
Source Code → Lexer → Tokens → Parser → AST → Interpreter → Runtime
```

- **Source Code** — `.cmm` program written by the user.
- **Lexer** — Converts characters into tokens.
- **Tokens** — Keywords, identifiers, numbers, operators, punctuation, etc.
- **Parser** — Consumes tokens according to the grammar.
- **AST** — Represents the logical structure of the program.
- **Interpreter** — Walks the AST and executes it.
- **Runtime** — Stores values and program state.

## v0.3 Architecture Change

v0.2 only needed integer values. v0.3 introduces two runtime value types:

```text
Value
├── Integer
└── Boolean
```

The implementation uses:

```cpp
using Value = std::variant<long long, bool>;
```

So the model changes from:

```text
Expression → long long
```

to:

```text
Expression → Value
              ├── integer
              └── boolean
```

Arithmetic requires integers. Comparisons produce booleans. Equality can compare integers or booleans.

## Expression Grammar

```text
expression
    → equality

equality
    → comparison ( ("==" | "!=") comparison )*

comparison
    → term ( (">" | ">=" | "<" | "<=") term )*

term
    → factor ( ("+" | "-") factor )*

factor
    → unary ( ("*" | "/") unary )*

unary
    → "-" unary
    | primary

primary
    → number
    | true
    | false
    | identifier
    | "(" expression ")"
```

Therefore:

```c--
let result = 10 + 5 > 12;
```

means:

```text
(10 + 5) > 12
```

## Project Structure

```text
c---v0.3/
├── README.md
├── CMakeLists.txt
├── include/
│   ├── ast/ast.hpp
│   ├── interpreter/interpreter.hpp
│   ├── lexer/lexer.hpp
│   ├── lexer/token.hpp
│   ├── parser/parser.hpp
│   └── runtime/
│       ├── environment.hpp
│       └── value.hpp
├── src/
│   ├── interpreter/interpreter.cpp
│   ├── lexer/lexer.cpp
│   ├── parser/parser.cpp
│   ├── runtime/environment.cpp
│   └── main.cpp
├── examples/booleans.cmm
└── tests/lexer_demo.cpp
```

## Build

```bash
cmake -S . -B build
cmake --build build
```

Run:

```bash
./build/cmm examples/booleans.cmm
```

On Windows with Visual Studio, the executable may be under `build/Debug/cmm.exe`.

## Runtime Type Rules

Arithmetic requires integers:

```c--
10 + 5
10 - 5
10 * 5
10 / 5
```

Comparisons require integers and return booleans:

```c--
10 > 5
10 >= 10
10 < 20
10 <= 20
```

Equality supports both current value types:

```c--
10 == 10
true == true
false != true
```

## Design Principle

Every language feature is connected through the full pipeline:

```text
Syntax → Lexer → Token → Parser → AST → Interpreter → Runtime
```

For booleans:

```text
true / false
     ↓
True / False tokens
     ↓
BooleanExpr AST
     ↓
Value = bool
     ↓
Interpreter evaluation
```

For comparisons:

```text
> >= < <= == !=
       ↓
     Tokens
       ↓
 Parser precedence
       ↓
   BinaryExpr
       ↓
 Interpreter
       ↓
      bool
```

## Not Included Yet

Intentionally deferred:

- `if` / `else`
- loops
- functions
- strings
- arrays
- logical `&&` / `||`
- unary `!`

## Roadmap

```text
v0.1  Arithmetic expressions
v0.2  Variables and assignments
v0.3  Booleans and comparisons     ← current
v0.4  if / else
v0.5  Loops
v0.6  Functions and return
v0.7  Scope
v0.8  Collections
v0.9  Standard library
v1.0  Coherent small language
```

Version numbers are learning milestones, not production readiness.

## Development Philosophy

C-- is built from the inside out. Each version adds a small language concept and connects it through the whole pipeline instead of introducing a large framework all at once.

The goal is to understand lexical analysis, parsing, grammar, AST design, expression precedence, interpretation, runtime values, environments, type errors, and language architecture.

The v0.3 lesson is especially important: an expression does not have to produce one fixed C++ type. The runtime now has a `Value` abstraction, which will make future language features easier to add.
