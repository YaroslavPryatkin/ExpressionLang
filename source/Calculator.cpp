#include "headers\Calculator.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

namespace CalculatorNamespace {

	Calculator::Calculator(std::istream& in, std::ostream& out, int consoleWidth)
		: in(in), out(out), base(in, out, consoleWidth), tokenizer(), tokenizedToRpn(), rpnToTree() {
	}

	std::string Calculator::parse(std::string input) {

		if (input == "stop") {
			savedInput = "";
			return "Error: stopped\n\n";
		}

		//trim right
		auto pos = input.find_last_not_of(" \t\n\r\f\v");
		if (pos != std::string::npos)
			input.erase(pos + 1);
		else
			return "";


		char back = input.back();

		while (input.back() == '\\')
			input.pop_back();

		if (back == ';' || back == ',' || back == '(' || back == '+' ||
			back == '-' || back == '*' || back == '/' || back == '^' ||
			back == '=' || back == '%' || back == '\\') {

			savedInput += input;
			return std::string("");
		}
		else {
			input = savedInput + input;
			savedInput = "";
		}


		Tokenized tokenized = tokenizer.parse(input, base);
		int amountOfNames = tokenized.names.size();

		if (tokenized.isFunction) {
			int amountOfArgs = tokenized.args.size();

			for (int i = 0; i < amountOfNames; i++)
				if (base.isVariable(tokenized.names[i]))
					throw std::runtime_error("Function can not have the same name as variable: " + tokenized.names[i]);
			for (int i = 0; i < amountOfArgs; i++)
				if (base.isFunction(tokenized.args[i]))
					throw std::runtime_error("Parameter can not have the same name as function: " + tokenized.args[i]);

			std::unordered_map<std::string, int> argsMap;

			for (int i = 0; i < amountOfArgs; i++)
				argsMap[tokenized.args[i]] = i;

#ifdef DEBUG			
			std::cout << tokenizer.makeExpression(tokenized, 0) << std::endl;
#endif

			std::set<std::string> namesSet(tokenized.names.begin(), tokenized.names.end());

#ifdef DEBUG
			std::cout << "Names before sent: " << std::endl;
			for (auto& nm : namesSet)
				std::cout << nm << std::endl;
#endif

			auto [ rpn, dependencies, variableDependencies, amountOfLocals]  = tokenizedToRpn.parse(tokenized.mainPart, base, namesSet, argsMap);
#ifdef DEBUG
			for (int i = 0;i < rpn.size();i++) {
				std::cout << rpn[i].header << " " << rpn[i].intValue << " " << rpn[i].isNumber << " " << rpn[i].value << std::endl;
			}
#endif
			std::string result = "";
			for (int i = 0; i < amountOfNames; i++) {

				std::string name = tokenized.names[i];

				if (base.isPreDefinedFunction(name, amountOfArgs)) {
					result += std::string("Function ") + name + " is pre-defined and can not be changed";
				}
				else {
					if (base.isUserDefinedFunction(name, amountOfArgs)) {
						out << "Function " << name << " with " << amountOfArgs <<
							" arguments already exist. Do you want to change it?\nDont forget, that changing it will affect every function, that depends on it.\nType y (yes) to continue or any other combination to skip" << std::endl << "> ";
						std::string ans;
						in >> ans;
						base.inputBufferClean();
						if (ans == "y" || ans == "yes") {
							try {
								Node* header = base.findUserDefinedFunction(name, amountOfArgs)->header;
								rpnToTree.parseIntoExisting(rpn, amountOfArgs, amountOfLocals, header);
								base.changeUserDefinedFunction(name, tokenizer.makeExpression(tokenized, i), amountOfArgs, amountOfLocals, dependencies, variableDependencies);
								result += std::string("Function ") + name + std::string(" with ") + std::to_string(amountOfArgs) + std::string(" arguments was succesfully changed\n\n");
							}
							catch (const std::exception& e) {
								result += std::string(e.what()) + "\n\n";
							}
						}
						else result += std::string("Function ") + name + " was skipped\n\n";
					}
					else {
						Node* newHeader = rpnToTree.parse(rpn, amountOfArgs, amountOfLocals);
						try {
							base.addUserDefinedFunction(name, tokenizer.makeExpression(tokenized, i), newHeader, amountOfArgs, dependencies, variableDependencies);
							result += std::string("Function ") + name + " with " + std::to_string(amountOfArgs) + " arguments was succesfully written\n\n";
						}
						catch (const std::exception& e) {
							result += std::string(e.what()) + "\n\n";
						}
					}
				}
			}
			return result;
		}
		else {
			for (int i = 0; i < amountOfNames; i++)
				if (base.isFunction(tokenized.names[i]))
					throw std::runtime_error("Variable can not have the same name as function: " + tokenized.names[i]);

			auto [rpn, dependencies, variableDependencies, amountOfLocals] = tokenizedToRpn.parse(tokenized.mainPart, base); //names == empty_set, args == empty_map 
			if (amountOfLocals > 0)
				throw std::runtime_error("You didn't create a functions, but you've defined local variables");
#ifdef DEBUG
			for (int i = 0;i < rpn.size();i++) {
				std::cout << rpn[i].header << " " << rpn[i].intValue << " " << rpn[i].isNumber << " " << rpn[i].value << std::endl;
			}
#endif
			Node* header = rpnToTree.parse(rpn);
			float value = header->evaluate();
			header->destroyFunction();//dont need any more. Since it was just created, no dependants
			for (int i = 0; i < amountOfNames; i++)
				base.changeOrSetVariable(tokenized.names[i], value);
			return std::to_string(value) + "\n\n";
		}
	}


	std::string Calculator::askForSavefileName() {
		std::cout << "Enter file name "
			"(empty = " << defaultSavefileName << "): ";

		std::string name;
		if (!std::getline(std::cin, name)) {
			throw std::runtime_error("Could not read file name");
		}

		if (name.empty()) {
			name = defaultSavefileName;
		}

		if (name.size() < savefileExtension.size() ||
			name.compare(
				name.size() - savefileExtension.size(),
				savefileExtension.size(),
				savefileExtension
			) != 0) {
			name += savefileExtension;
		}

		return name;
	}

	void Calculator::saveIntoFile()
	{
		savedInput.clear();


		const std::string fileName = askForSavefileName();

		std::ostringstream serialized;
		base.writeDefinitions(serialized);

		std::ofstream file(fileName);

		if (!file) {
			throw std::runtime_error(
				"Could not open file for writing: " + fileName
			);
		}

		file << serialized.str();

		file.close();

		if (!file) {
			throw std::runtime_error(
				"Failed to finish writing file: " + fileName
			);
		}

		std::cout << "Saved to " << fileName << std::endl;
	}

	void Calculator::loadFromFile()
	{
		savedInput.clear();

		const std::string fileName = askForSavefileName();
		std::ifstream file(fileName);

		if (!file) {
			throw std::runtime_error(
				"Could not open save file: " + fileName
			);
		}

		std::vector<std::string> definitions;
		std::string definition;

		while (std::getline(file, definition)) {
			if (definition.find_first_not_of(" \t\r") !=
				std::string::npos) {
				definitions.push_back(definition);
			}
		}

		if (file.bad()) {
			throw std::runtime_error(
				"Error while reading save file: " + fileName
			);
		}

		base.clear(false);

		for (std::size_t i = 0; i < definitions.size(); ++i) {
			try {
				std::cout << parse(definitions[i]);
			}
			catch (const std::exception& e) {
				throw std::runtime_error(
					"Failed to load definition " +
					std::to_string(i + 1) + ": " + e.what()
				);
			}
		}

		savedInput.clear();
		std::cout << "Loaded from " << fileName << std::endl;
	}

}