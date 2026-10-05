# C-- Language Syntax & Design Sheet

> **Status:** Initial language design
> **Language:** C--
> **Implementation:** C++
> **Goal:** Build a small programming language with its own intentional syntax.

---

## 1. Core Philosophy

C-- should not simply copy the syntax of C++, JavaScript, Python, or another existing language.

The syntax should be:

- Simple
- Consistent
- Readable
- Easy to parse
- Intentionally designed
- Small enough to understand completely

Every language feature should have a deliberate syntax decision before implementation.

---

# 2. Variables

## Mutable variable — `let`

```c--
let age = 20
let name = "Sumit"
```

`let` declares a variable whose value can later be changed.

```c--
let age = 20

age = 21
```

---

## Immutable variable — `fix`

```c--
fix pi = 3.14
fix maxUsers = 100
```

`fix` declares a variable that cannot be reassigned.

```c--
fix maxUsers = 100

maxUsers = 200   // Error
```

### Design idea

```text
let → value can change
fix → value is fixed
```

This gives C-- a clear distinction between mutable and immutable variables without requiring a separate `const` keyword.

---

# 3. Output

## `say()`

Console output uses a function-like syntax:

```c--
say("Hello")
say(age)
say("Age:", age)
```

The language intentionally uses `say()` instead of `print`, `println`, or `console.log`.

Example:

```c--
let name = "Sumit"

say("Hello")
say(name)
```

---

# 4. Input

## `ask()`

User input uses:

```c--
ask()
```

Example:

```c--
let name = ask()

say(name)
```

With a prompt:

```c--
let name = ask("What is your name?")

say(name)
```

### Design idea

The pair is intentionally simple:

```text
say()  → output
ask()  → input
```

---

# 5. Semicolons

C-- does **not require semicolons**.

Instead:

```c--
let x = 10
let y = 20

say(x + y)
```

is valid.

There is no need for:

```c--
let x = 10;
let y = 20;
say(x + y);
```

The parser determines statement boundaries using the language's syntax rules.

### Design principle

> Newlines and structural syntax should be enough to make statements readable.

---

# 6. Blocks

Blocks **require `{}`**.

Example:

```c--
if age > 18 {
    say("Adult")
}
```

Not:

```c--
if age > 18
    say("Adult")
```

and not:

```c--
if age > 18:
    say("Adult")
```

The braces explicitly define the beginning and end of a block.

---

# 7. Booleans

C-- uses:

```c--
true
false
```

Example:

```c--
let loggedIn = true
let banned = false
```

Boolean expressions:

```c--
let adult = age >= 18
```

---

# 8. Comments

C-- supports two types of comments.

## Single-line comments

```c--
// This is a comment

let age = 20 // This is also a comment
```

Everything after `//` on that line is ignored.

---

## Multiline comments

```c--
/*
    This is a multiline comment.

    Multiple lines can be written here.
*/

let age = 20
```

The comment begins with:

```text
/*
```

and ends with:

```text
*/
```

---

# 9. Functions

Functions use the `fn` keyword.

Basic structure:

```c--
fn greet() {
    say("Hello")
}
```

Calling the function:

```c--
greet()
```

With parameters:

```c--
fn greet(name) {
    say("Hello", name)
}
```

Calling it:

```c--
greet("Sumit")
```

Functions will eventually support return values:

```c--
fn add(a, b) {
    return a + b
}
```

The exact function/parameter/return syntax can be refined before the function milestone.

---

# 10. Initial Syntax Summary

| Feature              | C-- Syntax   |
| -------------------- | ------------ |
| Mutable variable     | `let`        |
| Immutable variable   | `fix`        |
| Output               | `say()`      |
| Input                | `ask()`      |
| Boolean true         | `true`       |
| Boolean false        | `false`      |
| Single-line comment  | `//`         |
| Multiline comment    | `/* */`      |
| Function declaration | `fn`         |
| Block                | `{ }`        |
| Semicolon            | Not required |

---

# 11. Example C-- Program

Putting the current design together:

```c--
/*
    Simple C-- program
*/

fix language = "C--"

fn greet(name) {
    say("Hello", name)
}

let name = ask("What is your name?")

greet(name)

let age = ask("How old are you?")

let adult = age >= 18

say("Adult:", adult)
say("Language:", language)
```

---

# 12. Syntax Decisions Still To Be Designed

These should be decided **before implementing the corresponding features**.

### Types

Should C-- be:

```text
Dynamic
```

or:

```c--
let age: int = 20
```

or use another type system?

---

### Strings

We need to decide:

```c--
"Hello"
```

and possibly:

```c--
'Hello'
```

Should both be supported?

---

### Operators

Current candidates:

```text
+
-
*
/
>
>=
<
<=
==
!=
```

Later:

```text
&&
||
!
```

We should decide the exact operator set and precedence.

---

### Conditional syntax

Possible direction:

```c--
if condition {
    ...
} else {
    ...
}
```

But this should be formally decided before v0.4.

---

### Loops

Syntax is still undecided.

For example:

```c--
while condition {
    ...
}
```

or something more uniquely C--.

---

### Functions

Already decided:

```c--
fn
```

Still to define:

- Parameter syntax
- Return syntax
- Whether functions are first-class values
- Whether nested functions are allowed
- Function overloading
- Default parameters

---

# 13. Language Design Rule

Before implementing a feature, follow this process:

```text
Design syntax
      ↓
Define grammar
      ↓
Define tokens
      ↓
Design AST
      ↓
Implement parser
      ↓
Implement runtime behavior
      ↓
Add examples/tests
      ↓
Document the feature
```

This prevents implementation details from accidentally becoming language design.

---

# 14. Current Status

### Decided

```text
let       → mutable variable
fix       → immutable variable
say()     → console output
ask()     → user input
true      → boolean true
false     → boolean false
//        → single-line comment
/* */     → multiline comment
fn        → function declaration
{}        → required blocks
;         → unnecessary
```

### Not yet decided

```text
Type syntax
String rules
Operator set
Operator precedence
if / else syntax
Loop syntax
return syntax
Function details
Collections
Error-handling syntax
Imports/modules
```

**Important:** These undecided parts should stay undecided until we intentionally design them.
