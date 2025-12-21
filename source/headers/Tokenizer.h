#pragma once
#include <string>
#include <vector>
#include <set>
#include "DataBase.h"

namespace CalculatorNamespace {

    struct Tokenized {
        std::vector<std::string> mainPart;
        bool isFunction = false;
        std::vector<std::string> names;
        std::vector<std::string> args;
    };

    class Tokenizer {
    public:
        Tokenizer();
        Tokenized parse(std::string input, const DataBase& base);
        std::string makeExpression(Tokenized& input, int whichName);

    private:
        inline bool isSymbolPart(char c);
        inline bool isStringPart(char c, bool readingNumbers = false);
        inline bool isNumberPart(char c);
        inline bool isSpecialChar(char c);
        inline void skipWhiteSpaces(const std::string& input, size_t& pos);

        std::string readNumber(const std::string& input, size_t& pos);
        std::string readSymbols(const std::string& input, size_t& pos, const DataBase& base);
        std::string readString(const std::string& input, size_t& pos);
    };
}