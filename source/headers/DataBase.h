#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <set>
#include "Supplementary.h"

namespace CalculatorNamespace {
    struct UserDefinedFunction;

    struct Variable {
        Node* header;
        std::string name;
        std::set<UserDefinedFunction*> dependants; // functions that depend on this variable
        Variable();
        Variable(std::string name, float value);
        Variable(Variable&& other) noexcept;
        Variable& operator=(Variable&& other) noexcept;
        ~Variable();
        Variable(const Variable&) = delete;
    };

    struct UserDefinedFunction {
        Node* header;
        std::string expression;
        int amountOfArguments;
        std::set<UserDefinedFunction*> dependants; // functions that depend on this function
        std::set<UserDefinedFunction*> dependencies;
        std::set<Variable*> variableDependencies;
        UserDefinedFunction();
        UserDefinedFunction(std::string expression, Node* header, int amountOfArguments, std::set<UserDefinedFunction*> dependencies, std::set<Variable*> variableDependencies);
        UserDefinedFunction(UserDefinedFunction&& other) noexcept;
        UserDefinedFunction& operator=(UserDefinedFunction&& other) noexcept;
        ~UserDefinedFunction();
        UserDefinedFunction(const UserDefinedFunction&) = delete;
    };


    struct PreDefinedFunction {
        Node* header;
        std::string name;
        std::string expression;
        std::string comment;
        int amountOfArguments;
        Fix fix;
        bool rightAssociative;
        int precedence;
        PreDefinedFunction(std::string name, std::string expression, std::string comment, int amountOfArguments, Fix fix, int precedence, bool rightAssociative, std::function<float(Node* self)> evaluate_func);
    };

    class DataBase {
        std::unordered_map<std::string, std::vector<UserDefinedFunction*>> userDefined;
        std::unordered_map<std::string, Variable*> variables;
        std::unordered_map<std::string, std::vector<PreDefinedFunction*>> preDefined;

        void gatherDependantTree(UserDefinedFunction* toDelete, std::set<UserDefinedFunction*>& toDeleteSet);

    public:
        const int defaultPrefixFunctionPrecedence = 100;
        const std::string implicitOperationName = "1__implicit_miltiplication__";
        std::ostream& out;
        std::istream& in;
        int consoleWidth = 80;

        DataBase(std::istream& in, std::ostream& out, int consoleWidth = 80);

        inline void initDefaultVariables();
        inline void checkIfExit(std::string& input);
        inline void inputBufferClean();

        Variable* changeOrSetVariable(std::string name, float newValue);
        Variable* askForAVariable(std::string name);
        const bool isVariable(std::string name);


        rpnToken makeRpnTokenFromVariable(std::string name, std::set<Variable*>& variableDependancies);

        const std::pair<int, bool> functionInfo(std::string name, Fix fix);

        const bool isFunction(std::string name, Fix fix);
        const bool isFunction(std::string name, int amountOfArgs = -1);
        const bool isPreDefinedFunction(std::string name, int amountOfArgs = -1);
        const bool isUserDefinedFunction(std::string name, int amountOfArgs = -1);

        const UserDefinedFunction* findUserDefinedFunction(const std::string& name, int amountOfArgs);

        const rpnToken makeRpnTokenFromFunction(std::string name, Fix fix, std::set<UserDefinedFunction*>& dependencies ,int amountOfArguments = 0);

        void addUserDefinedFunction(std::string& name, std::string& expression, Node* header, int amountOfArguments, 
            const std::set<UserDefinedFunction*>& dependencies, const std::set<Variable*> variableDependencies);

        //changes meta-data of the function, changes in the header should be done by RpnToTree::parseIntoExisting
        void changeUserDefinedFunction(std::string& name, std::string& newExpression, int amountOfArguments, const std::set<UserDefinedFunction*>& newDependencies, const std::set<Variable*> newVariableDependencies);

        std::pair<std::string, int> askForFunctionCredentials();

        void printDependants(const std::string& name, int amountOfArgs);
        void printDependancies(const std::string& name, int amountOfArgs);
        void printVariableDependants(const std::string& name);

        void askToPrintDependants();
        void askToPrintDependancies();
        void askToPrintVariableDependants();


        void deleteUserDefinedFunction(const std::string& name, int amountOfArgs);
        std::pair<int, int> deleteAllPossibleOverloadsOfUserDefinedFunction(const std::string& name); // <returns amount of deleted, amount of left>
        int deleteDependantTree(const std::string& name, int amountOfArgs);

        void askToDelete();
        void askToDeleteDependantTree();

        inline std::string makeTitle(const std::string& name, int width, char fillingSymbol = ' ');

        void printUserDefinedFunctions();
        void printPreDefinedFunctions();
        void printVariables();

        void enterVariable();
        void deleteVariable(const std::string& name);
        void askToDeleteVariable();

        void clear();
    };
}