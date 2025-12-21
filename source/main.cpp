#include <iostream>
#include <string>
#include <limits>
#include "headers\Calculator.h"
/*
cd "C:\Users\yaros\OneDrive\Документы\Calculator"

cl.exe /EHsc /std:c++17 /I source source\*.cpp /Fo"build\\" /Fe:calculator.exe /O2
*/

int main() {
    const int consoleWidth = 120;
    const std::string separator(consoleWidth, '-');

    CalculatorNamespace::Calculator calc(std::cin, std::cout, consoleWidth);

    std::string line;

    std::cout << "--- Yaroslav Calculator Loaded ---" << std::endl;
    std::cout << "Type 'help' for a list of commands, 'man' for a manual." << std::endl;

    while (true) {
        try {
            std::cout << "> ";
            if (!std::getline(std::cin, line)) break;

            if (line == "exit") {
                break;
            }
            else if (line == "var") {
                calc.base.printVariables();
            }
            else if (line == "sys") {
                calc.base.printPreDefinedFunctions();
            }
            else if (line == "fnc") {
                calc.base.printUserDefinedFunctions();
            }
            else if (line == "dpc") {
                calc.base.askToPrintDependancies();
            }
            else if (line == "dpd") {
                calc.base.askToPrintDependants();
            }
            else if (line == "dpv") {
                calc.base.askToPrintVariableDependants();
            }
            else if (line == "set") {
                calc.base.enterVariable();
            }
            else if (line == "dlf") {
                calc.base.askToDelete();
            }
            else if (line == "dlr") {
                calc.base.askToDeleteDependantTree();
            }
            else if (line == "dlv") {
                calc.base.askToDeleteVariable();
            }
            else if (line == "clr") {
                calc.base.clear();
            }
            else if (line == "help") {
                std::cout << calc.base.makeTitle("Available commands", consoleWidth, ':') << "\n" << separator << "\n"
                    << "Available commands:\n"
                    << "  exit - Close the program\n"
                    << "  var  - Print all variables\n"
                    << "  sys  - Print all system (predefined) functions\n"
                    << "  fnc  - Print all user-defined functions\n"
                    << "  dpc  - Print all user-defined function dependencies\n"
                    << "  dpd  - Print all user-defined function dependants (functions, that depend on this function)\n"
                    << "  dpv  - Print all variable dependants (functions, that depend on this variable)\n"
                    << "  set  - Create or update a variable\n"
                    << "  dlf  - Delete a user-defined function\n"
                    << "  dlr  - Delete a user-defined function and recursively all its dependants\n"
                    << "  dlv  - Delete a specific variable\n"
                    << "  clr  - Delete all variables and user functions\n"
                    << "  man - Show user manual\n"
                    << "  help - Show this information\n"
                    << "  Any other input will be calculated\n" << separator << "\n" << std::endl;
            }
            else if(line == "man") {
                std::cout << calc.base.makeTitle("User manual", consoleWidth, ':') << "\n"
                    << separator << "\n"
                    << "  You can enter input in one of the following forms:\n"
                    << "      1) variableName_1 := variableName_2 := expression\n"
                    << "              Expression will be calculated, its result will be shown and\n"
                    << "              written into all listed variables. If variables do not exist,\n"
                    << "              they will be created automatically.\n"
                    << "      2) functionName_1 := functionName_2(argName_1, argName_2) := expression\n"
                    << "              Expression will be stored as a user-defined function.\n"
                    << "              Recursion and overloading are supported.\n"
                    << "      3) expression\n"
                    << "              Expression will be calculated and its result will be shown.\n"
                    << separator << "\n"
                    << "  Note: the last name in a definition determines whether it is a function or\n"
                    << "        a variable and if it is a function, determines an amount of arguments.\n" 
                    << "        For example : \n"
                    << "            name_1(arg_1, arg_2) := name_2 := expression\n"
                    << "        will be treated as a definition of two variables.\n"
                    << separator << "\n"
                    << "  If an expression contains a name that has no definition yet, the calculator\n"
                    << "  will ask you to provide its value and then store it as a variable.\n"
                    << "  However, if a variable is first encountered on the left side of '=', its\n"
                    << "  value will NOT be requested and it will be automatically created with value 0,\n"
                    << "  since it will be immediately overwritten.\n"
                    << separator << "\n"
                    << "  Variables always store some value. Functions with zero arguments may be used\n"
                    << "  as variables, but they are still evaluated at runtime.\n"
                    << "  For one-argument functions, parentheses around the argument are optional:\n"
                    << "      sin cos x  is valid input.\n"
                    << "  This may lead to ambiguity when a function has both zero- and one-argument\n"
                    << "  overloads.\n"
                    << "      By default, such functions are assumed to be zero-argument.\n"
                    << "      Using parentheses makes one-argument interpretation default\n"
                    << "      for this function and all functions to the left.\n"
                    << "      Examples:\n"
                    << "          f_1 f_2 f_3 x        == f_1 * f_2 * f_3 * x\n"
                    << "          f_1 f_2 f_3(x)       == f_1(f_2(f_3(x)))\n"
                    << "          f_1 f_2(f_3 x)       == f_1(f_2(f_3 * x))\n"
                    << separator << "\n"
                    << "  Function arguments are passed by reference.\n"
                    << "  This means that assigning a new value to a function argument will modify the\n"
                    << "  variable that was passed into the function.\n"
                    << separator << "\n"
                    << "  The ';' operator allows sequential execution of expressions.\n"
                    << "      left ; right\n"
                    << "  First, the left expression is evaluated, then the right one.\n"
                    << "  The result of the entire expression is the result of the right side.\n"
                    << "  This allows writing full programs as a single expression.\n"
                    << "  The last expression not followed by ';' becomes the returned result.\n"
                    << separator << "\n"
                    << "  If the input ends with ',' or ';', it will not be evaluated immediately.\n"
                    << "  Instead, it will be stored and waiting for continuation on the next line.\n"
                    << "  This allows writing programs across multiple input lines.\n"
                    << separator << "\n"
                    << "  Names for functions and variables may contain letters and the '\'' symbol.\n"
                    << "  They may also contain numbers if separated by '_' characters.\n"
                    << "      Valid names:   name_1, name_1_a, name__a\n"
                    << "      Invalid names: name1, name_a, name__1\n"
                    << "  Functions and variables may not share the same name.\n"
                    << separator << "\n" << std::endl;

            }
            else if (line.empty()) {
                continue;
            }
            else {
                std::string result = calc.parse(line);
                std::cout << result;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n" <<  std::endl;

            if (std::cin.fail()) {
                std::cin.clear();
            }

            if (std::cin.rdbuf()->in_avail() > 0) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    std::cout << "Goodbye!" << std::endl;
    return 0;
}