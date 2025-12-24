# ExpressionLang

ExpressionLang is a console-based mini programming language and expression evaluator. 
It supports user-defined variables and functions, recursion, references, local scopes, 
sequential execution, and multi-line input. It started as a calculator, but it now 
functions more like a small interpreted language.

---

## Features

- User-defined variables and functions
- Recursive function calls
- Function overloading
- Local variables with `local` and `endlocal`
- Function arguments passed by reference
- Sequential execution using the `;` operator
- Multi-line input support
- Automatic creation of undefined variables
- Prefix, infix, and postfix system functions
- One-argument prefix functions may omit parentheses
- Zero-argument functions can be used as variables
- Automatic value resolution for undefined variables during evaluation

---

## Architecture Overview

The parsing process is divided into three stages:

1. Tokenizing the input
2. Converting tokens into Reverse Polish Notation (RPN)
3. Parsing RPN into an Abstract Syntax Tree (AST)

Tokenizing and AST construction were relatively straightforward. The most challenging part 
was converting tokens into RPN due to:

- Prefix, infix, and postfix functions
- Zero- and one-argument functions
- Function chaining
- Ambiguous grammar rules

While it is possible to parse tokenized input directly into an AST, it is unclear 
whether such an approach would support the required zero- and one-argument logic. 
Using RPN also simplifies debugging and future extensions.

Smart pointers were not used, as they did not fit well with the chosen AST node structure. 
Instead, a custom dependency tracking system is used to safely manage function dependencies 
and allow recursive deletion of dependent functions.

---


## User Manual

Basic Input Forms 
  - 1) `variableName_1 := variableName_2 := expression`
       Calculates expression, shows result, and writes it into all listed variables.
       If variables do not exist, they will be created automatically.
  - 2) `functionName_1 := functionName_2(arg1, arg2) := expression`
       Expression will be stored as a user-defined function. Recursion and overloading are supported.
  - 3) `expression`
       Expression will be calculated and its result will be shown immediately.

Definition Rules
  - Note: The last name in a definition determines whether it is a function or a variable.
  - If it is a function, the last name determines the amount of arguments.
  - Example: 'name_1(arg_1, arg_2) := name_2 := expression' will be treated as two variables.

Undefined Variables
  - If an expression contains an unknown name, the calculator will ask you to provide its value.
  - If a variable is first encountered on the left side of `=`, its value will NOT be requested.
  - Such variables are automatically created with value 0, as they are immediately overwritten.

Local Variables
  - Use 'local' to declare a variable: 'local a = 5'. New local variables are initialized with 0.0f.
  - Technical note: 'local a += 5' is possible, but it is not recommended.
  - Local variables shadow global ones and cannot share names with functions or parameters.
  - Use 'endlocal' to remove a variable from the current scope.
  - Example: `local a=5; a+=1; endlocal a=7; a*=10;` (Writes 6 to local `a`, 70 to global `a`).

Functions
  - In existing functions, all arguments are passed by reference.
  - For one-argument functions, parentheses are optional: 'sin cos x' is valid.
  - Functions with both 0 and 1-arg overloads are assumed to be 0-arg by default.
  - Using parentheses forces 1-argument interpretation for that function and all to its left:
      1) `f_1 f_2 f_3 x`    == `f_1 * f_2 * f_3 * x`
      2) `f_1 f_2 f_3(x)`   == `f_1(f_2(f_3(x)))`
      3) `f_1 f_2(f_3 x)`   == `f_1(f_2(f_3 * x))`

Recursion
  - In recursion: variables (global, local, or refs) are passed by reference.
  - In recursion: expressions (like x-1) are calculated and passed by value.
  - Logic: passing 'x-1' by reference to 'x' would cause an immediate infinite loop.
  - Passing variables by reference allows modifying values at any recursion level.
  - WARNING: The parser does not check for infinite recursion. Use with caution!

Execution and Multi-line
  - Use `;` for sequential execution. The result is the value of the right-side expression.
  - The last expression not followed by `;` becomes the returned result.
  - Input continues if a line ends with: `;`, `,`, `(`, `+`, `-`, `*`, `/`, `^`, `=` or `\` .
  - If a line ends with `\`, the character is removed and input continues.
  - Type 'stop' to terminate input and cancel evaluation.

Naming Constraints
  - Names may contain letters and the `'` symbol.
  - Numbers are allowed only if separated by underscores: `name_1`, `name__a`.
  - Invalid names: `name1`, `name_a`, `name__1`.
  - Functions and variables may not share the same name.

Examples

 ```a := 10``` or ```a = 10```
  - Sets the value of gloval variable `a` to be `10`.

```f(x) := x + 1```
  - Creates the function f(x), that returns `x+1`.

```
 f(x) := if(x>0, f(x-1) + x, 0)
```
  - Returns sum `1` to `x`.

```
 f(x, y) :=
 local res = 0;
 y +=
 for(local i=0, i<=x, i++,
     res += i    \
 );
 amountOfCalls += 1;
 res
```
  - Returns sum `1` to `x`, adds loop count to `y`, and increments global `amountOfCalls`.

```
 f(x) :+
 local res = 1;
 local rt = root x;
 for(local i=0, i <= rt, i++,
     if( x/i - floor (x/i), res = 0)    \
 );
 res

```
  - Returns `1` is `x` is prime, `0` otherwise.
