#pragma once
#include <vector>
#include <string>
#include <stack>
#include <set>
#include <unordered_map>
#include <tuple>
#include "Supplementary.h"
#include "DataBase.h"

namespace CalculatorNamespace {

	class TokenizedToRpn {
		struct StackToken {
			std::string name;
			Fix fix;
			int amountOfArgs;
			int precedence;
			bool rightAssociative = false;
			//for opening paranthesis
			StackToken() : name("("), fix(Fix::infix), amountOfArgs(0), precedence(0) {}

			StackToken(std::string name, Fix fix, std::pair<int, bool> info, int amountOfArgs = 0) :
				name(name), fix(fix), amountOfArgs(amountOfArgs), precedence(info.first), rightAssociative(info.second) {
			}
			StackToken(std::string name, Fix fix, int precedence = 0, int amountOfArgs = 0) :
				name(name), fix(fix), amountOfArgs(amountOfArgs), precedence(precedence) {
			}
		};


		std::stack<StackToken> opStack;
		std::vector<rpnToken> rpn;

		DataBase* base;
		Fix currentFix;
		const std::set<std::string>* names;
		const std::unordered_map<std::string, int>* args;
		std::unordered_map<std::string, int> localVariables; // they act like parameters
		std::unordered_map<std::string, int> endedLocalVariables; //local variables, that were switched off using endlocal key word

		std::set<UserDefinedFunction*> dependencies;
		std::set<Variable*> variableDependencies;

		int amountOfArgs;
		int amountOfLocals;

		static const std::unordered_map<std::string, int> GLOBAL_EMPTY_MAP;
		static const std::set<std::string> GLOBAL_EMPTY_SET;
		//shunting yard algorythm. A little bigger then on wikipedia

		//checks if there is name in this->names, this->args, this->localVariables
		inline bool isAmongLocalFields(const std::string& name);
		bool isNumber(const std::string& name);
		inline bool isAssign(const std::string& name);
		inline bool isRecursion(const std::string& name, int amountOfArgs);

		bool tryKeyWord(const std::string& name, const std::string& next);
		inline void parseRowOfZeroFunctions();
		void prepareFields();
		void popOperationToRpn();
		bool tryOperation(const std::string& name, Fix fix);
		void parseOperation(const std::string& name);
		bool tryRecurion(const std::string& name, const std::string& next);
		void parseComma();
		void parseOpeningParanthesis();
		void parseClosingParanthesis();
		void parseVariable(const std::string& name, const std::string& next);
		void parseNumberOrVariable(const std::string& name, const std::string& next);
		void parseEnding();

	public:
		TokenizedToRpn();
		std::tuple<std::vector<rpnToken>, std::set<UserDefinedFunction*>, std::set<Variable*>, int> 
			parse(const std::vector<std::string>& input, const DataBase& newBase, const std::set<std::string>& names = GLOBAL_EMPTY_SET, const std::unordered_map<std::string, int>& args = GLOBAL_EMPTY_MAP);
	};
}