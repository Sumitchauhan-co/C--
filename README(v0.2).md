# C--

> A tiny interpreted programming language written in C++ — built as a learning project.

C-- is a small programming language created to explore how programming languages work internally.

The project is intentionally built from scratch in C++ instead of relying on parser generators or compiler frameworks.

## Current Version

**C-- v0.2 — Variables & Expressions**

At this stage C-- supports:

- Integer numbers
- Arithmetic: `+`, `-`, `*`, `/`
- Parentheses
- Variable declaration with `let`
- Variable reassignment
- Reading variables inside expressions
- `say` for output
- Basic runtime errors
- Source files with the `.cmm` extension

### Example

```c--
let x = 10;
let y = x * 2 + 5;

say y;

x = 100;
say x;
```

Output:

```text
25
100
```

---

## Architecture

C-- follows a simple source-to-execution pipeline:

```text
Source Code
     ↓
   Lexer
     ↓
   Tokens
     ↓
   Parser
     ↓
    AST
     ↓
 Interpreter
     ↓
   Runtime
```

### 1. Source Code

The program written by the user using C-- syntax.

```c--
let x = 10 + 5;
say x;
```

### 2. Lexer

The lexer reads the source code character by character and converts it into meaningful tokens.

For example:

```text
let x = 10 + 5;
```

becomes conceptually:

```text
LET
IDENTIFIER(x)
EQUAL
NUMBER(10)
PLUS
NUMBER(5)
SEMICOLON
```

### 3. Tokens

Tokens are structured pieces produced by the lexer.

Each token has a type, its original text, and its source position.

Example:

```text
NUMBER("10")
IDENTIFIER("x")
PLUS("+")
```

### 4. Parser

The parser consumes tokens according to C-- grammar and builds an AST.

For:

```text
10 + 5 * 2
```

the parser understands that multiplication has higher precedence:

```text
      +
     / \
   10   *
       / \
      5   2
```

### 5. AST

AST means **Abstract Syntax Tree**.

It represents the logical structure of the program without needing to retain every piece of source formatting.

C-- currently has AST nodes for:

- Number literals
- Variable references
- Binary expressions
- Unary expressions
- Variable declarations
- Assignments
- `say` statements
- Expression statements

### 6. Interpreter

The interpreter walks the AST and executes it.

For example:

```text
10 + 5 * 2
```

becomes:

```text
10 + 10
→ 20
```

### 7. Runtime

The runtime maintains the state of the executing program.

Currently this mainly means an environment containing variable names and values:

```text
x → 10
y → 25
```

Later, this will expand to support scopes, functions, arrays, objects, and other runtime features.

---

## Project Structure

```text
c--/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── ast/
│   │   └── ast.hpp
│   ├── interpreter/
│   │   └── interpreter.hpp
│   ├── lexer/
│   │   ├── lexer.hpp
│   │   └── token.hpp
│   ├── parser/
│   │   └── parser.hpp
│   └── runtime/
│       └── environment.hpp
├── src/
│   ├── ast/
│   │   └── ast.cpp
│   ├── interpreter/
│   │   └── interpreter.cpp
│   ├── lexer/
│   │   └── lexer.cpp
│   ├── parser/
│   │   └── parser.cpp
│   ├── runtime/
│   │   └── environment.cpp
│   └── main.cpp
├── examples/
│   └── variables.cmm
└── tests/
    └── lexer_demo.cpp
```

---

## Building

Requirements:

- C++17 or newer
- CMake 3.20+
- A C++ compiler (GCC/MinGW, Clang, or MSVC)

### 1. Install CMake

Choose the command matching your operating system package manager to install CMake:

#### Windows (PowerShell / winget)

```powershell
winget install Kitware.CMake
```

#### Windows (Chocolatey)

```powershell
choco install cmake
```

_Note: Restart your terminal/VS Code after installing so the system recognizes the `cmake` path._

#### macOS (Homebrew)

```bash
brew install cmake
```

#### Linux (Debian / Ubuntu)

```bash
sudo apt update && sudo apt install cmake
```

---

### 2. Configure & Build

Depending on your local compiler configuration, choose the build method that matches your environment:

#### Option A: Windows (using MinGW / GCC)

Explicitly configure CMake to generate MinGW Makefiles to prevent defaulting to Visual Studio's `nmake`:

```powershell
# Configure env setup
$env:Path += ";C:\Program Files\CMake\bin"
```

```powershell
# 1. Configure the project
cmake -S . -B build -G "MinGW Makefiles"

# 2. Compile the source
cmake --build build

# 3. Run the compiler executable
.\build\c--.exe examples\variables.cmm
```

#### Option B: Windows (using Visual Studio / MSVC)

```powershell
# 1. Configure for Visual Studio 2022 solution generators
cmake -S . -B build -G "Visual Studio 17 2022"

# 2. Compile the source
cmake --build build

# 3. Run the compiler executable
.\build\Debug\c--.exe examples\variables.cmm
```

#### Option C: macOS & Linux

```bash
# 1. Configure the project
cmake -S . -B build

# 2. Compile the source
cmake --build build

# 3. Run the compiler executable
./build/c-- examples/variables.cmm
```

---

## C-- v0.2 Syntax

### Variables

Declare a variable:

```c--
let age = 20;
```

Reassign it:

```c--
age = 21;
```

Use it in an expression:

```c--
let next = age + 1;
```

### Arithmetic

```c--
let result = (10 + 5) * 2;
```

Supported operators:

```text
+   addition
-   subtraction
*   multiplication
/   division
```

### Output

```c--
say result;
```

---

## Development Philosophy

C-- is being developed incrementally.

The goal is not to immediately create a production compiler. The goal is to understand each stage by implementing it manually.

The intended progression is:

```text
v0.1  → Arithmetic expressions
v0.2  → Variables and assignments
v0.3  → Booleans and comparisons
v0.4  → if / else
v0.5  → Loops
v0.6  → Functions and return
v0.7  → Scope
v0.8  → Collections
v0.9  → Standard library
v1.0  → A coherent small language
```

The version numbers are learning milestones, not promises of production readiness.

---

## Design Principle

When adding a new feature, follow the pipeline:

```text
Syntax
  ↓
Lexer
  ↓
Token
  ↓
Parser
  ↓
AST
  ↓
Interpreter
  ↓
Runtime
```

For example, adding `let` requires:

```text
"let"
  ↓
LET token
  ↓
Parser recognizes declaration
  ↓
VariableDeclaration AST node
  ↓
Interpreter evaluates initializer
  ↓
Runtime stores variable
```

This keeps the architecture understandable as C-- grows.

---

## Next Milestone

C-- v0.3 will introduce:

- Boolean values
- `true` / `false`
- Comparison operators
- Equality operators
- Logical operators

Example target syntax:

```c--
let age = 20;

say age >= 18;
say age == 20;
```

After that, control flow can be built on top of these primitives.

---

## Why C--?

The name is intentionally playful.

It is inspired by C++ while representing a much smaller language built specifically for learning how languages work.

**C--: less language, more understanding.**
