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
                std::cout << std::endl;
            }
            else if (line == "sys") {
                calc.base.printPreDefinedFunctions();
                std::cout << std::endl;
            }
            else if (line == "fnc") {
                calc.base.printUserDefinedFunctions();
                std::cout << std::endl;
            }
            else if (line == "dpc") {
                calc.base.askToPrintDependancies();
                std::cout << std::endl;
            }
            else if (line == "dpd") {
                calc.base.askToPrintDependants();
                std::cout << std::endl;
            }
            else if (line == "dpv") {
                calc.base.askToPrintVariableDependants();
                std::cout << std::endl;
            }
            else if (line == "set") {
                calc.base.enterVariable();
                std::cout << std::endl;
            }
            else if (line == "dlf") {
                calc.base.askToDelete();
                std::cout << std::endl;
            }
            else if (line == "dlr") {
                calc.base.askToDeleteDependantTree();
                std::cout << std::endl;
            }
            else if (line == "dlv") {
                calc.base.askToDeleteVariable();
                std::cout << std::endl;
            }
            else if (line == "clr") {
                calc.base.clear();
                std::cout << std::endl;
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
                    << calc.base.makeTitle("Basic Input Forms", consoleWidth, '-') << "\n"
                    << "  - 1) variableName_1 := variableName_2 := expression\n"
                    << "       Calculates expression, shows result, and writes it into all listed variables.\n"
                    << "       If variables do not exist, they will be created automatically.\n"
                    << "  - 2) functionName_1 := functionName_2(arg1, arg2) := expression\n"
                    << "       Expression will be stored as a user-defined function. Recursion and overloading are supported.\n"
                    << "  - 3) expression\n"
                    << "       Expression will be calculated and its result will be shown immediately.\n"
                    << calc.base.makeTitle("Definition Rules", consoleWidth, '-') << "\n"
                    << "  - Note: The last name in a definition determines whether it is a function or a variable.\n"
                    << "  - If it is a function, the last name determines the amount of arguments.\n"
                    << "  - Example: 'name_1(arg_1, arg_2) := name_2 := expression' will be treated as two variables.\n"
                    << calc.base.makeTitle("Undefined Variables", consoleWidth, '-') << "\n"
                    << "  - If an expression contains an unknown name, the calculator will ask you to provide its value.\n"
                    << "  - If a variable is first encountered on the left side of '=', its value will NOT be requested.\n"
                    << "  - Such variables are automatically created with value 0, as they are immediately overwritten.\n"
                    << calc.base.makeTitle("Local Variables", consoleWidth, '-') << "\n"
                    << "  - Use 'local' to declare a variable: 'local a = 5'. New local variables are initialized with 0.0f.\n"
                    << "  - Technical note: 'local a += 5' is possible, but it is not recommended.\n"
                    << "  - Local variables shadow global ones and cannot share names with functions or parameters.\n"
                    << "  - Use 'endlocal' to remove a variable from the current scope.\n"
                    << "  - Example: local a=5; a+=1; endlocal a=7; a*=10; (Writes 6 to local 'a', 70 to global 'a').\n"
                    << calc.base.makeTitle("Functions", consoleWidth, '-') << "\n"
                    << "  - In existing functions, all arguments are passed by reference.\n"
                    << "  - For one-argument functions, parentheses are optional: 'sin cos x' is valid.\n"
                    << "  - Functions with both 0 and 1-arg overloads are assumed to be 0-arg by default.\n"
                    << "  - Using parentheses forces 1-argument interpretation for that function and all to its left:\n"
                    << "      1) f_1 f_2 f_3 x    == f_1 * f_2 * f_3 * x\n"
                    << "      2) f_1 f_2 f_3(x)   == f_1(f_2(f_3(x)))\n"
                    << "      3) f_1 f_2(f_3 x)   == f_1(f_2(f_3 * x))\n"
                    << calc.base.makeTitle("Recursion", consoleWidth, '-') << "\n"
                    << "  - In recursion: variables (global, local, or refs) are passed by reference.\n"
                    << "  - In recursion: expressions (like x-1) are calculated and passed by value.\n"
                    << "  - Logic: passing 'x-1' by reference to 'x' would cause an immediate infinite loop.\n"
                    << "  - Passing variables by reference allows modifying values at any recursion level.\n"
                    << "  - WARNING: The parser does not check for infinite recursion. Use with caution!\n"
                    << calc.base.makeTitle("Execution and Multi-line", consoleWidth, '-') << "\n"
                    << "  - Use ';' for sequential execution. The result is the value of the right-side expression.\n"
                    << "  - The last expression not followed by ';' becomes the returned result.\n"
                    << "  - Input continues if a line ends with: ';', ',', '(', '+', '-', '*', '/', '^', '=', or '\\'.\n"
                    << "  - If a line ends with '\\', the character is removed and input continues.\n"
                    << "  - Type 'stop' to terminate input and cancel evaluation.\n"
                    << calc.base.makeTitle("Naming Constraints", consoleWidth, '-') << "\n"
                    << "  - Names may contain letters and the ''' symbol.\n"
                    << "  - Numbers are allowed only if separated by underscores: 'name_1', 'name__a'.\n"
                    << "  - Invalid names: 'name1', 'name_a', 'name__1'.\n"
                    << "  - Functions and variables may not share the same name.\n"
                    << calc.base.makeTitle("Complex Example", consoleWidth, '-') << "\n"
                    << "  f(x, y) := \n"
                    << "  local res = 0; \n"
                    << "  y += \n"
                    << "  for(local i=0, i<=x, i++, \n"
                    << "  res += i    \\\n"
                    << "  );\n"
                    << "  amountOfCalls += 1;\n"
                    << "  res\n"
                    << "  - Returns sum 1 to x, adds loop count to y, and increments global 'amountOfCalls'.\n"
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