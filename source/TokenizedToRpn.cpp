#include "headers\TokenizedToRpn.h"
#include <iostream>

namespace CalculatorNamespace {

	const std::unordered_map<std::string, int> TokenizedToRpn::GLOBAL_EMPTY_MAP = {};
	const std::set<std::string> TokenizedToRpn::GLOBAL_EMPTY_SET = {};

	TokenizedToRpn::TokenizedToRpn() : currentFix(Fix::prefix), base(nullptr), names(nullptr), args(nullptr), amountOfArgs(0) {}


	inline bool TokenizedToRpn::isRecursion(const std::string& name, int amountOfArgsuments)
	{
#ifdef DEBUG
		std::cout << "Checking if recursion: names->find(name) != names->end() == " << (names->find(name) != names->end()) << " amountOfArgs != 0 == " << (amountOfArgs != 0) << " amountOfArgsuments == amountOfArgs == " << (amountOfArgsuments == amountOfArgs) << std::endl;
#endif
		return names->find(name) != names->end() && amountOfArgs != 0 && amountOfArgsuments == amountOfArgs;
	}

	//pop all prefix functions to rpn. By default consider them as zero argument (aka just variables)
	// so f_1 f_2 f_3 x will be parsed as f_1 * f_2 * f_3 * x if all f_i() exist. If f_2() does not, then it will be parsed as f_1*f_2(f_3 * x)
	inline void TokenizedToRpn::parseRowOfZeroFunctions() {
		while (!opStack.empty() && opStack.top().fix == Fix::prefix && opStack.top().amountOfArgs == 0) {
			StackToken& cur = opStack.top();


			if (base->isFunction(cur.name, 0)) {// by default try zero argument
#ifdef DEBUG
				std::cout << "Parsing number, option 1" << std::endl;
#endif
				popOperationToRpn();

				//also need implicit multiplication: f__zero(a+b) == f__zero*(a+b)
				//can do it this way becouse it is always just multiplying 2 numbers
				rpn.push_back(base->makeRpnTokenFromFunction(base->implicitOperationName, Fix::infix, dependencies));
			}
			else {
#ifdef DEBUG
				std::cout << "Parsing number, option 2" << std::endl;
#endif
				cur.amountOfArgs++; // that x was actually an argument
				popOperationToRpn();
			}
		}
	}

	void TokenizedToRpn::prepareFields() {
		while (!opStack.empty()) opStack.pop();
		rpn.clear();
		dependencies.clear();
		variableDependencies.clear();
		currentFix = Fix::prefix;
	}

	void TokenizedToRpn::popOperationToRpn() {
		if (opStack.empty()) throw std::runtime_error("Wrong input, operation stack has unexpectedly became empty");
		StackToken token = opStack.top();
		opStack.pop();

#ifdef DEBUG
		std::cout << "Pushing to rpn function " << token.name << " with " << token.amountOfArgs << " arguments and fix == " << (currentFix == Fix::prefix ? "prefix" : (currentFix == Fix::infix ? "infix" : "postfix")) << std::endl;
#endif

		if (isRecursion(token.name, token.amountOfArgs)) {
#ifdef DEBUG
			std::cout << "it is recursion!" << std::endl;
#endif
			rpn.push_back(rpnToken()); // recursion only if amount of args is correct and not 0. Else try to find overload
		}
		else rpn.push_back(base->makeRpnTokenFromFunction(token.name, token.fix, dependencies, token.amountOfArgs));
	}

	bool TokenizedToRpn::tryOperation(const std::string& name, Fix fix) {
		if (fix == Fix::postfix) {
			if (!base->isFunction(name, Fix::postfix)) return false; //didnt found
			rpn.push_back(base->makeRpnTokenFromFunction(name, Fix::postfix, dependencies));
		}
		else if (fix == Fix::prefix) {
			if (!base->isFunction(name, Fix::prefix)) return false; //didnt found
			opStack.push(StackToken(name, Fix::prefix, base->defaultPrefixFunctionPrecedence));
		}
		else {
			std::pair<int, bool> info = base->functionInfo(name, Fix::infix);
			if (info == std::pair<int, bool>(base->defaultPrefixFunctionPrecedence * 2, false)) return false; //didnt found
			while (!opStack.empty()) {
				StackToken top = opStack.top();
				if (top.fix == Fix::infix && top.name != "(" &&
					(top.precedence > info.first ||
						(top.precedence == info.first && !info.second))
					) {
					popOperationToRpn();
				}
				else break;

			}
			opStack.push(StackToken(name, Fix::infix, info, 2));
		}
		if (!opStack.empty() && opStack.top().name == "(") {
			opStack.top().amountOfArgs++;//letting ( know, that there was something
#ifdef DEBUG
			std::cout << "was something was increased by try operation, name: " << name << std::endl;
#endif
		}
		return true;
	}

	void TokenizedToRpn::parseOperation(const std::string& name) {
		if (currentFix == Fix::postfix) {
			if (!tryOperation(name, Fix::postfix)) { //try postfix, if not found continue to infix
				if (!tryOperation(name, Fix::infix)) { //try infix, if not found continue to prefix
					tryOperation(base->implicitOperationName, Fix::infix); // situations like (number1)postfix prefix(number2), need implicit default operator before prefix
					if (!tryOperation(name, Fix::prefix)) //try prefix, if not found, error
						throw std::runtime_error("Invalid use of operator: " + name);
					//after prefix, next can be only prefix, so nothing to change
				}
				currentFix = Fix::prefix; // after infix or prefix, next must be prefix
			}
			// after postfix, next can be both postfix and infix, so nothing to change
		}
		else {//currrentFix == Fix::prefix, since it is never infix
			if (!tryOperation(name, Fix::prefix))//try prefix, if not found
				if (!opStack.empty() && opStack.top().fix == Fix::prefix && opStack.top().amountOfArgs == 0 && base->isFunction(opStack.top().name, 0)) {
					// maybe that prefix function on the stack top was actually meant to be zero argument. That was something like x + f_1 * y
					//then let act like that was a variable
#ifdef DEBUG
					std::cout << "Starting zero-function case" << std::endl;
#endif
					popOperationToRpn(); //we've already considered it in the if cundition

					parseRowOfZeroFunctions();

					if (!opStack.empty() && opStack.top().name == "(") {
						opStack.top().amountOfArgs++;//letting ( know, that there was something
#ifdef DEBUG
						std::cout << "was something was increased by operation in zero-function section" << std::endl;
#endif
					}

					currentFix = Fix::postfix;//since that was actually a variable, now should be postfix
					//and now let parse that operation again
					parseOperation(name);
				}
				else throw std::runtime_error("Invalid use of operator: " + name);
			//after prefix, next can be only prefix, so nothing to change
		}
	}

	bool TokenizedToRpn::tryRecurion(const std::string& name) {
		if (names->find(name) == names->end()) return false; //didnt found
#ifdef DEBUG
		std::cout << "recursion was tried" << std::endl;
#endif
		if (currentFix != Fix::prefix)
			tryOperation(base->implicitOperationName, Fix::infix);

		opStack.push(StackToken(name, Fix::prefix, base->defaultPrefixFunctionPrecedence));
		currentFix = Fix::prefix;
		return true;
	}

	void TokenizedToRpn::parseComma() {
		if (opStack.top().name == "(" && opStack.top().amountOfArgs == 0) return; // if there were nothing, skip the typo

		while (!opStack.empty() && opStack.top().name != "(")
			popOperationToRpn();

		if (!opStack.empty()) {
			opStack.pop(); // pop the "("
			if (!opStack.empty()) {
				opStack.top().amountOfArgs++; // increase argument count for function
#ifdef DEBUG
				std::cout << "amoutn of args was increased in parse comma" << std::endl;
#endif
			}
			opStack.push(StackToken()); // push "(" back
		}

		//after comma there wasnt anything at (, so we dont increas its amountOfArgs

		currentFix = Fix::prefix; //after comma, next must be prefix
	}

	void TokenizedToRpn::parseOpeningParanthesis() {
		if (currentFix == Fix::postfix)
			tryOperation(base->implicitOperationName, Fix::infix);

		if (!opStack.empty() && opStack.top().name == "(") opStack.top().amountOfArgs++;//letting ( know, that there was something. Hadles f((x)) cases

		opStack.push(StackToken()); // pushing opening paranthesis

		currentFix = Fix::prefix; //after opening paranthesis, next must be prefix
	}

	void TokenizedToRpn::parseClosingParanthesis() {
#ifdef DEBUG
		std::cout << "stack top info: top name: " << opStack.top().name << " top amount of arguments:" << opStack.top().amountOfArgs << std::endl;
#endif
		int wasSomething = (opStack.top().name != "(" || opStack.top().amountOfArgs > 0) ? 1 : 0; //check if there were anything after last comma, so func(a) doesnt have one argument

		while (!opStack.empty() && opStack.top().name != "(") {
			if (opStack.top().fix == Fix::prefix && opStack.top().amountOfArgs == 0 && base->isFunction(opStack.top().name, 0)) {
				popOperationToRpn();//already know that it mimics the variable
				parseRowOfZeroFunctions();
			}
			else
				popOperationToRpn();
		}
			

		if (opStack.empty())
			throw std::runtime_error("Mismatched parentheses");


		opStack.pop();// pop the "(" 
#ifdef DEBUG
		std::cout << "Closing Paranthesis: " << opStack.top().amountOfArgs << " " << wasSomething << " " << std::endl;
#endif
		// after closing parenthesis, pop all prefix operators to rpn, by default considering them as non zero arguments. 
		// So f_1 f_2 f_3 will be parsed as f_1(f_2(f_3(x))), if all f_i(x) exist. If f_2(x) does not, then it will barsed as f_1(f_2()*f_3(x))
		while (!opStack.empty() && opStack.top().fix == Fix::prefix) {
			StackToken& cur = opStack.top();


			if (cur.amountOfArgs == 0 && wasSomething == 0) { // there were just func()
				popOperationToRpn();
				wasSomething = 0; // if there are next prefix function, then we will try to multiply 
#ifdef DEBUG
				std::cout << " option 1" << std::endl;
#endif
			}
			else if (isRecursion(cur.name, cur.amountOfArgs + wasSomething)) {// if recursion, then we choose recursion above all
				rpn.push_back(rpnToken());
				opStack.pop();
				wasSomething = 1;
			}
			else if (base->isFunction(cur.name, cur.amountOfArgs + wasSomething)) {//if there were arguments, then we check if that function exist.
				//dont need to call popOperationToRpn(), since we've already checked everything
				rpn.push_back(base->makeRpnTokenFromFunction(cur.name, cur.fix, dependencies, cur.amountOfArgs + wasSomething));
				opStack.pop();
				wasSomething = 1; // if there are next prefix function, then we try to pass result of this function into it, thus having one argument
#ifdef DEBUG
				std::cout << " option 2" << std::endl;
#endif
			}
			else if (base->isFunction(cur.name, 0) && cur.amountOfArgs == 0) {// if function doesn't exist and there were only one argument, we try zero argument
				popOperationToRpn();
				wasSomething = 0; // if there are next prefix function, then we will try to multiply 

				//also need implicit multiplication: f__zero(a+b) == f__zero*(a+b)
				//can do it this way becouse it is always just multiplying 2 numbers
				rpn.push_back(base->makeRpnTokenFromFunction(base->implicitOperationName, Fix::infix, dependencies));
#ifdef DEBUG
				std::cout << " option 3" << std::endl;
#endif
			}
			else
				throw std::runtime_error(std::string("Function ") + cur.name + " does not have overload with " + std::to_string(cur.amountOfArgs + wasSomething) + " arguments");

			

		}

		if (!opStack.empty() && opStack.top().name == "(") {
			opStack.top().amountOfArgs++;//letting ( know, that there was something. Hadles f(g(x)) situations - here first ( must know there was an argument g(x)
#ifdef DEBUG
			std::cout << "was something increased by closing paranthesis" << std::endl;
#endif
		}
		currentFix = Fix::postfix; //after closing paranthesis, next must be postfix. Implicit multiplication will be handled automaticly on the next token
	}

	void TokenizedToRpn::parseVariable(const std::string& name) {
		if (args->find(name) != args->end()) rpn.push_back(rpnToken(args->at(name))); //it is a parameter
		else rpn.push_back(base->makeRpnTokenFromVariable(name, variableDependencies));
	}

	void TokenizedToRpn::parseNumberOrVariable(const std::string& name) {
		//situations like (number1)postfix number2 or number1 number2, need implicit default operator before number2
		if (currentFix == Fix::postfix)
			tryOperation(base->implicitOperationName, Fix::infix);

		currentFix = Fix::postfix;// after number or variable, must be postfix

		try {
			float value;
			size_t pos;
			value = std::stof(name, &pos);


			if (pos == name.size()) rpn.push_back(rpnToken(value));	//if everything is ok, then it is a constant number
			else parseVariable(name); // it is something like 12.4abc
		}
		catch (...) {
			//if didnt manage to transform into a number, then it must be variable
			parseVariable(name);
		}
 
		parseRowOfZeroFunctions();


		if (!opStack.empty() && opStack.top().name == "(") {
			opStack.top().amountOfArgs++;//letting ( know, that there was something
#ifdef DEBUG
			std::cout << "was something increased by number" << std::endl;
#endif
		}
	}

	void TokenizedToRpn::parseEnding() {
		// after reading input, pop all remaining operators from stack to output
		while (!opStack.empty()) {
			if (opStack.top().name == "(")
				throw std::runtime_error("Mismatched parentheses");


			if (opStack.top().fix == Fix::prefix && opStack.top().amountOfArgs == 0 && base->isFunction(opStack.top().name, 0)) {
				//that is a function that mimic variable
				// 
				//then let act like that is a variable
#ifdef DEBUG
				std::cout << "Starting interesting case" << std::endl;
#endif
				popOperationToRpn(); //we've already considered it in the if cundition

				parseRowOfZeroFunctions();
			}
			else
				popOperationToRpn();
		}
	}

	std::tuple<std::vector<rpnToken>, std::set<UserDefinedFunction*>, std::set<Variable*>>
		TokenizedToRpn::parse(const std::vector<std::string>& input, const DataBase& newBase, const std::set<std::string>& names, const std::unordered_map<std::string, int>& args) {
		prepareFields();
		base = const_cast<DataBase*>(&newBase);
		this->names = &names;
		this->args = &args;
		amountOfArgs = args.size();
#ifdef DEBUG
		std::cout << "Names after sent: " << std::endl;
		for (auto& nm : (*this->names))
			std::cout << nm << std::endl;
#endif



		if (input.size() == 0) throw std::runtime_error("Input is empty (tokenized -> rpn)");

		for (int i = 0; i < input.size(); i++) {
			if (input[i] == "") throw std::runtime_error("Input is empty (tokenized -> rpn)");

#ifdef DEBUG
			std::cout << "Considering input token: " << input[i] << ", current fix == " << (currentFix == Fix::prefix ? "prefix" : (currentFix == Fix::infix ? "infix" : "postfix")) << std::endl;
#endif

			if (input[i].size() == 1 && input[i][0] == ',') parseComma();
			else if (input[i].size() == 1 && input[i][0] == '(') parseOpeningParanthesis();
			else if (input[i].size() == 1 && input[i][0] == ')') parseClosingParanthesis();
			else if (!tryRecurion(const_cast<std::string&>(input[i]))) { // if tryRecurtion returned false, should try already defined functions and variables
				if (base->isFunction(input[i])) {
					std::string temp = input[i];
					parseOperation(temp);
				}
				else {
					std::string temp = input[i];
					parseNumberOrVariable(temp);
				}
			}
		}
		parseEnding();
		return std::make_tuple(rpn, dependencies, variableDependencies);
	}
}