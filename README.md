# CoolCalculator
My project of creating a calculator that can handle user-defined functions, variables, recursion. Also it is really easy to add new system-defined functions.

System functions can be prefix, infix, postfix; prefix system functions may have undefined amount of arguments. User defined functions can be only prefix. Any prefix function may have overloads. Functions with one argument dont need paranthesis around their argument. Functions with zero arguments can be used as variables (but still be evaluated at runtime). Also supports lazy-evaluation.

I've separated the parsing process into 3 steps: tokenizing the input, parsing tokenized input into reverse polish notation (rpn), parsing rpn into AST-tree. The fitst and the third parts were pretty easy, but parsing into rpn was a little tricky becouse functions may have different fix and complicated zero- and one-argument logic (and lack of info on wikipedia). 

I know it is possible to avoid creating rpn and parse raw tokenized input directly to AST-tree, but I'm not shure it allows my zero- and one-argument logic. Also, using rpn makes it easier to debug and add new features.

I didn't use clever pointers, since they didn't work with my AST node structure. So I've added my own dependencies control.

This calculator currently has no = operator (You can write definitions like a=5 and f(x,y) = x + y, but not if(x > 0, x = 0, x = x) ). May be I will add it in the future, it shouldn't be hard.

Here is the "help" text from the calculator.exe

Available commands:
  exit - Close the program
  var  - Print all variables
  sys  - Print all system (predefined) functions
  fnc  - Print all user-defined functions
  dpc  - Print all user-defined function dependencies
  dpd  - Print all user-defined function dependants (functions, that depend on this function)
  dpv  - Print all variable dependants (functions, that depend on this variable)
  set  - Create or update a variable
  dlf  - Delete a user-defined function
  dlr  - Delete a user-defined function and recursively all its dependants
  dlv  - Delete a specific variable
  clr  - Delete all variables and user functions
  help/man - Show this information
  Any other input will be calculated
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  You can write an input if the form of:
      1) variableName_1 = variableName_2 = expression
              Expression will be calculated, result will be shown on the screen and written into the variables
              If variables don't exist, they will be created.
      2) functionName_1 = functionName_2 ( argName_1, argName_2) = expression
              Expression will be stored in the function. Recursion and overloading is allowed.
      3) expression
              Expression will be calculated, result will be shown on the screen.
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  Note, that it is the last name which defines whether it's a function or a variable,
      For example: name_1(arg_1, arg_2) = name_2 = expression will be considered as a definition of 2 variables.

  If you enter into the expression any name, that have no definition, you will be asked to provide it's value.
      Also it will be stored as a variable.
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  Variables always store some value. However, if you make a function with zero arguments, you may use it as a variable.
  You dont need to put the argument for one-argument function into paranthesis: sin cos x will work.
      That may lead to confusion: what will "f x" and "f(x)" be parsed into, if f can be bouth zero- and one-argument?
      By default f is assumed to be zero-argument. But with paranthesis, parser will try to find one-argument variant.
      Also, if f was chosen to be one-argument, all functions on the left of f will be considered as one argument
      f_1 f_2 f_3 x == f_1 * f_2 * f_3 * x; f_1 f_2 f_3(x) == f_1(f_2(f_3(x))); f_1 f_2(f_3 x) == f_1(f_2(f_3 * x))
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  Note, that it is the last name which defines whether it's a function or a variable,
      For example: name_1(arg_1, arg_2) = name_2 = expression will be considered as a definition of 2 variables.
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  Names for functions and variables may include letters and ' symbol.
      Also they may include a number, if it is isolated by _ symbol.
      For example: name_1, name_1_a and name__a are all valid, but name1, name_a or name__1 are not.
  Functions can not have the same names as variables or vice versa.
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
