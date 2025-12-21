# CoolCalculator

**CoolCalculator** is a console calculator that supports **user-defined functions**, **variables**, **recursion**, and **lazy evaluation**.  
The architecture is designed to make adding new **system-defined functions** simple and extensible.

---

## Features

- User-defined variables and functions
- Recursive function calls
- Lazy evaluation
- Function overloading
- Prefix, infix, and postfix system functions
- Prefix system functions may have a variable number of arguments
- User-defined functions are prefix-only
- One-argument prefix functions do **not** require parentheses
- Zero-argument functions can be used as variables (but are still evaluated at runtime)

---

## Parsing Architecture

The parsing process is divided into **three stages**:

1. **Tokenizing** the input
2. Converting tokens into **Reverse Polish Notation (RPN)**
3. Parsing RPN into an **AST (Abstract Syntax Tree)**

Tokenizing and AST construction were relatively straightforward.  
The most challenging part was converting tokens into RPN, because:

- Functions can be prefix, infix, or postfix
- Functions may have zero or one argument
- Prefix functions can be chained
- There is very little documentation for this kind of grammar

Although it is possible to parse tokens directly into an AST without using RPN, it is unclear whether this approach would support the required zero- and one-argument logic.  
Using RPN also makes debugging and extending the parser easier.

---

## Memory Management

Smart pointers were not used because they did not work well with the chosen AST node structure.  
Instead, a custom dependency tracking system was implemented to manage object lifetimes safely.

---

## Known Limitations

- There is currently **no `=` operator inside expressions**
  - Valid:
    ```
    a = 5
    f(x, y) = x + y
    ```
  - Not valid:
    ```
    if(x > 0, x = 0, x = x)
    ```

This feature may be added in the future.

---

## 📖 Help Text (from `calculator.exe`)

```text
Available commands:
  exit - Close the program
  var  - Print all variables
  sys  - Print all system (predefined) functions
  fnc  - Print all user-defined functions
  dpc  - Print all user-defined function dependencies
  dpd  - Print all user-defined function dependants (functions that depend on this function)
  dpv  - Print all variable dependants (functions that depend on this variable)
  set  - Create or update a variable
  dlf  - Delete a user-defined function
  dlr  - Delete a user-defined function and recursively all its dependants
  dlv  - Delete a specific variable
  clr  - Delete all variables and user functions
  help/man - Show this information
  Any other input will be calculated
------------------------------------------------------------------------------------------------------------------------
  You can write input in one of the following forms:

      1) variableName_1 = variableName_2 = expression
         The expression will be calculated, the result will be printed
         and assigned to all listed variables.
         If variables do not exist, they will be created.

      2) functionName_1 = functionName_2(argName_1, argName_2) = expression
         The expression will be stored as a function body.
         Recursion and overloading are allowed.

      3) expression
         The expression will be calculated and the result printed.
------------------------------------------------------------------------------------------------------------------------
  Note that the *last name* determines whether the definition is a function or a variable.

  For example:
      name_1(arg_1, arg_2) = name_2 = expression
  will be treated as a definition of two variables.
------------------------------------------------------------------------------------------------------------------------
  If you use a name in an expression that has no definition,
  you will be prompted to provide its value.
  It will then be stored as a variable.
------------------------------------------------------------------------------------------------------------------------
  Variables always store a value.
  However, a zero-argument function can be used as a variable.

  Parentheses are not required for one-argument functions:
      sin cos x

  This may cause ambiguity:
      f x      vs      f(x)

  If both zero- and one-argument versions exist:
  - f is assumed to be zero-argument by default
  - parentheses force the one-argument version

  If f is chosen as a one-argument function,
  all functions to the left of f are treated as its argument:

      f_1 f_2 f_3 x      == f_1 * f_2 * f_3 * x
      f_1 f_2 f_3(x)     == f_1(f_2(f_3(x)))
      f_1 f_2(f_3 x)     == f_1(f_2(f_3 * x))
------------------------------------------------------------------------------------------------------------------------
  Naming rules:

  - Names may contain letters and the ' symbol
  - Numbers are allowed only when isolated by '_'
      Valid:   name_1, name_1_a, name__a
      Invalid: name1, name_a, name__1

  Functions and variables cannot share the same name.
------------------------------------------------------------------------------------------------------------------------
