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
    std::cout << "Type 'help' or 'man' for a list of commands." << std::endl;

    while (true) {
        try {
            std::cout << "\n> ";
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
            else if (line == "help" || line == "man") {
                std::cout << "Available commands:\n"
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
                    << "  help/man - Show this information\n"
                    << "  Any other input will be calculated\n" << separator << "\n"
                    << "  You can write an input if the form of:\n"
                    << "      1) variableName_1 = variableName_2 = expression\n"
                    << "              Expression will be calculated, result will be shown on the screen and written into the variables\n"
                    << "              If variables don't exist, they will be created.\n"
                    << "      2) functionName_1 = functionName_2 ( argName_1, argName_2) = expression\n"
                    << "              Expression will be stored in the function. Recursion and overloading is allowed.\n"
                    << "      3) expression\n"
                    << "              Expression will be calculated, result will be shown on the screen.\n" << separator << "\n"
                    << "  Note, that it is the last name which defines whether it's a function or a variable,\n"
                    << "      For example: name_1(arg_1, arg_2) = name_2 = expression will be considered as a definition of 2 variables.\n" << separator << "\n"
                    << "  If you enter into the expression any name, that have no definition, you will be asked to provide it's value.\n"
                    << "      Also it will be stored as a variable.\n" << separator << "\n"
                    << "  Variables always store some value. However, if you make a function with zero arguments, you may use it as a variable.\n"
                    << "  You dont need to put the argument for one-argument function into paranthesis: sin cos x will work.\n"
                    << "      That may lead to confusion: what will \"f x\" and \"f(x)\" be parsed into, if f can be bouth zero- and one-argument?\n"
                    << "      By default f is assumed to be zero-argument. But with paranthesis, parser will try to find one-argument variant.\n"
                    << "      Also, if f was chosen to be one-argument, all functions on the left of f will be considered as one argument \n"
                    << "      f_1 f_2 f_3 x == f_1 * f_2 * f_3 * x; f_1 f_2 f_3(x) == f_1(f_2(f_3(x))); f_1 f_2(f_3 x) == f_1(f_2(f_3 * x)) \n" << separator << "\n"
                    << "  Note, that it is the last name which defines whether it's a function or a variable,\n"
                    << "      For example: name_1(arg_1, arg_2) = name_2 = expression will be considered as a definition of 2 variables.\n" << separator << "\n"
                    << "  Names for functions and variables may include letters and \' symbol.\n"
                    << "      Also they may include a number, if it is isolated by _ symbol.\n"
                    << "      For example: name_1, name_1_a and name__a are all valid, but name1, name_a or name__1 are not.\n"
                    << "  Functions can not have the same names as variables or vice versa.\n" << separator << "\n" 
                    << "  Have a nice day!" << std::endl;
            }
            else if (line.empty()) {
                continue;
            }
            else {
                std::string result = calc.parse(line);
                std::cout << "Result: " << result << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;

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