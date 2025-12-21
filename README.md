# CoolCalculator

CoolCalculator is a console-based calculator and expression language that supports
user-defined variables, user-defined functions, recursion, references, and
sequential execution.

Although it looks like a calculator, it behaves more like a small interpreted
language with expression-based syntax. The goal of this project is to explore
parsing, expression evaluation, and language design rather than numerical
performance.

---

## Features

- User-defined variables and functions
- Recursive function calls
- Function overloading
- Lazy evaluation
- Prefix, infix, and postfix system functions
- Prefix system functions may accept a variable number of arguments
- User-defined functions are prefix-only
- One-argument prefix functions do not require parentheses
- Zero-argument functions can be used as variables
- Function arguments are passed by reference
- Sequential execution using the ';' operator
- Multi-line input support

---

## Architecture Overview

The parsing process is divided into three stages:

1. Tokenizing the input
2. Converting tokens into Reverse Polish Notation (RPN)
3. Parsing RPN into an Abstract Syntax Tree (AST)

Tokenizing and AST construction were relatively straightforward.
The most challenging part was converting tokens into RPN due to:

- Prefix, infix, and postfix functions
- Zero- and one-argument functions
- Function chaining
- Ambiguous grammar rules

While it is possible to parse tokenized input directly into an AST, it is unclear
whether such an approach would support the required zero- and one-argument logic.
Using RPN also simplifies debugging and future extensions.

Smart pointers were not used, as they did not fit well with the chosen AST node
structure. Instead, a custom dependency tracking system is used to safely manage
object lifetimes and allow recursive deletion of dependent entities.

---

## User Manual

You can enter input in one of the following forms:

1) variableName_1 := variableName_2 := expression  
   Expression will be calculated, its result will be shown and written into all
   listed variables. If variables do not exist, they will be created automatically.

2) functionName_1 := functionName_2(argName_1, argName_2) := expression  
   Expression will be stored as a user-defined function.
   Recursion and overloading are supported.

3) expression  
   Expression will be calculated and its result will be shown.

------------------------------------------------------------------------------------------------------------------------

Note: the last name in a definition determines whether it is a function or a
variable and, if it is a function, determines the number of arguments.

For example:

name_1(arg_1, arg_2) := name_2 := expression

will be treated as a definition of two variables.

------------------------------------------------------------------------------------------------------------------------

If an expression contains a name that has no definition yet, the calculator will
ask you to provide its value and then store it as a variable.

However, if a variable is first encountered on the left side of '=', its value
will NOT be requested and it will be automatically created with value 0, since it
will be immediately overwritten.

------------------------------------------------------------------------------------------------------------------------

Variables always store some value. Functions with zero arguments may be used as
variables, but they are still evaluated at runtime.

For one-argument functions, parentheses around the argument are optional:

sin cos x

This may lead to ambiguity when a function has both zero- and one-argument
overloads.

By default, such functions are assumed to be zero-argument.
Using parentheses makes one-argument interpretation default for this function and
all functions to the left.

Examples:

f_1 f_2 f_3 x        == f_1 * f_2 * f_3 * x  
f_1 f_2 f_3(x)       == f_1(f_2(f_3(x)))  
f_1 f_2(f_3 x)       == f_1(f_2(f_3 * x))

------------------------------------------------------------------------------------------------------------------------

Function arguments are passed by reference.

This means that assigning a new value to a function argument will modify the
variable that was passed into the function.

------------------------------------------------------------------------------------------------------------------------

The ';' operator allows sequential execution of expressions.

left ; right

First, the left expression is evaluated, then the right one.
The result of the entire expression is the result of the right side.

This allows writing full programs as a single expression.
The last expression not followed by ';' becomes the returned result.

------------------------------------------------------------------------------------------------------------------------

If the input ends with ',' or ';', it will not be evaluated immediately.
Instead, it will be stored and waiting for continuation on the next line.

This allows writing programs across multiple input lines.

------------------------------------------------------------------------------------------------------------------------

Names for functions and variables may contain letters and the ''' symbol.
They may also contain numbers if separated by '_' characters.

Valid names:
name_1
name_1_a
name__a

Invalid names:
name1
name_a
name__1

Functions and variables may not share the same name.
