# C-- Compiler (v0.5)

C-- is a lightweight, custom-built interpreted programming language designed to explore language design, lexing, parsing, and tree-walking interpretation in C++. With **v0.5**, C-- transitions from a simple expression evaluator into a **Turing-complete** language equipped with robust loop constructs, statement flow control, block scoping, and increment/decrement operators.

---

## 🚀 What's New in v0.5?

- **Loop Constructs**: Full support for `while`, `do-while`, and `for` loops.
- **Flow Control Keywords**:
  - `break`: Instantly exits the innermost loop.
  - `continue`: Skips the current iteration and jumps to the next loop cycle / update expression.
  - `pass`: A no-op statement useful as a placeholder in conditional or loop bodies.
- **Unary Increment & Decrement**: Added support for `++` and `--` operators (e.g., `x++`, `x--`).
- **Block Scoping**: Variables declared inside loops and blocks possess proper lexical scoping rules and are cleaned up upon scope exit.
- **Runtime Safety**: Strict runtime validation ensures `break` and `continue` statements cannot be used outside of active loops.

---

## 📂 Project Structure

```text
c--/
├── include/
│   ├── ast/ast.hpp            // AST node definitions (loops, flow control)
│   ├── interpreter/interpreter.hpp // Interpreter runtime with loop depth tracking
│   ├── lexer/lexer.hpp        // Lexer definition for keywords & operators
│   ├── lexer/token.hpp        // Token types (While, Do, For, Break, Continue, Pass, etc.)
│   ├── parser/parser.hpp      // Recursive descent parser declarations
│   └── runtime/
│       ├── environment.hpp    // Lexical scope and variable bindings
│       └── value.hpp          // Runtime values (double, bool, string)
├── src/
│   ├── ast/ast.cpp
│   ├── interpreter/interpreter.cpp // Execution logic for loops & signals
│   ├── lexer/lexer.cpp        // Scanning logic for ++, --, and new keywords
│   ├── parser/parser.cpp      // Parsing logic for loop headers and blocks
│   ├── runtime/
│   │   ├── environment.cpp
│   │   └── value.cpp
│   └── main.cpp               // Entry point supporting file and stdin execution
├── CMakeLists.txt
└── loop.cmm                   // Example v0.5 script
```

---

## 📝 Syntax Overview (v0.5 Examples)

### 1. For Loop

```text
for (let i = 1; i <= 5; i++) {
    say(i)
}
```

### 2. While Loop & Increment

```text
let count = 0
while (count < 3) {
    say("Counting...")
    count++
}
```

### 3. Do-While with Break & Continue

```text
let x = 0
do {
    x++
    if (x == 2) {
        continue // Skips printing 2
    }
    if (x == 4) {
        break    // Exits loop early
    }
    say(x)
} while (x < 5)
```

---

## 🛠️ Building the Project

Ensure you have a C++17 compatible compiler and CMake installed.

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the executable
cmake --build .
```

---

## 🏃 Running C-- Programs

You can run a C-- source file (`.cmm`) by passing it as an argument:

```bash
./c-- ../loop.cmm
```

Or pipe code directly via standard input:

```bash
echo 'for (let n = 1; n <= 3; n++) { say(n) }' | ./c--
```
