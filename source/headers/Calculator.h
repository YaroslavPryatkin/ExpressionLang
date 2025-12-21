#pragma once
#include "Tokenizer.h"
#include "TokenizedToRpn.h"
#include "RpnToTree.h"
#include "DataBase.h"

namespace CalculatorNamespace {

	class Calculator {
		Tokenizer tokenizer;
		TokenizedToRpn tokenizedToRpn;
		RpnToTree rpnToTree;
		std::istream& in;
		std::ostream& out;
	public:
		DataBase base;

		Calculator(std::istream& in = std::cin, std::ostream& out = std::cout, int consoleWidth = 80);

		std::string savedInput;

		std::string parse(std::string input);
	};
}