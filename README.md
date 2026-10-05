## Language Architecture

The language follows a simple **source-to-execution pipeline**:

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
# 1. Configure the project (Note the space before the period, or just delete the period)
cmake -B build .

# 2. Compile the source
cmake --build build

# 3. Run the compiler executable
.\build\cmm.exe examples\variables.cmm
.\build\cmm.exe examples\booleans.cmm
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

### 1. Source Code

The program written by the user using the language's syntax.

Example:

```text
let x = 10 + 5
say x
```

### 2. Lexer

The **lexer** (lexical analyzer) reads the source code character by character and groups characters into meaningful units.

Its job is to recognize things such as:

- Keywords
- Identifiers
- Numbers
- Strings
- Operators
- Punctuation

Example:

```text
let x = 10 + 5
```

becomes roughly:

```text
LET
IDENTIFIER(x)
EQUALS
NUMBER(10)
PLUS
NUMBER(5)
```

### 3. Tokens

**Tokens** are the structured pieces produced by the lexer.

A token generally contains:

```text
Type + Value + Position
```

For example:

```text
IDENTIFIER("x")
NUMBER(10)
PLUS("+")
```

Tokens make the source code easier for the parser to understand.

### 4. Parser

The **parser** consumes the tokens and determines how they are related according to the language grammar.

For example:

```text
10 + 5
```

is recognized as an addition expression rather than five unrelated tokens.

The parser produces an **AST**.

### 5. AST

AST stands for **Abstract Syntax Tree**.

It represents the logical structure of the program as a tree.

For:

```text
10 + 5
```

the AST is conceptually:

```text
    +
   / \
 10   5
```

The AST removes unnecessary details from the original source and gives the interpreter a structured representation to work with.

### 6. Interpreter

The **interpreter** walks through the AST and executes the operations represented by it.

For example:

```text
10 + 5
```

is evaluated as:

```text
10 + 5 → 15
```

For a variable declaration:

```text
let x = 15
```

the interpreter evaluates `15` and stores it in the runtime environment.

### 7. Runtime

The **runtime** provides the environment required while the program is executing.

It manages things such as:

- Variables and their values
- Scopes
- Functions
- Function calls
- Runtime values
- Built-in operations

For example:

```text
let x = 15
```

may result in the runtime environment containing:

```text
x → 15
```

### Overall Responsibility

```text
Lexer      → Understand individual pieces
Parser     → Understand program structure
AST        → Represent program structure
Interpreter→ Execute the structure
Runtime    → Manage execution state
```

This project initially uses a **tree-walk interpreter**, meaning the interpreter directly walks through the AST to execute the program rather than compiling it to machine code.
