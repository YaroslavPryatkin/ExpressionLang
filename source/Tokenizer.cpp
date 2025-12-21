#include "headers\Tokenizer.h"
#include <stdexcept>
#include <cctype>

namespace CalculatorNamespace {

    Tokenizer::Tokenizer() {}

    inline bool Tokenizer::isSpecialChar(char c) {
        return c == '(' || c == ')' || c == ',';
    }

    inline bool Tokenizer::isSymbolPart(char c) {
        return !std::isalnum(static_cast<unsigned char>(c)) &&
            !std::isspace(static_cast<unsigned char>(c)) &&
            c != '.' && c != '(' && c != ')' && c != '_';
    }

    inline bool Tokenizer::isStringPart(char c, bool readingNumbers) {
        if (c == '_' || c == '\'') return true;
        if (!readingNumbers) return std::isalpha(static_cast<unsigned char>(c));
        return isNumberPart(c);
    }

    inline bool Tokenizer::isNumberPart(char c) {
        return std::isdigit(static_cast<unsigned char>(c)) || c == '.';
    }

    inline void Tokenizer::skipWhiteSpaces(const std::string& input, size_t& pos) {
        while (pos != input.size() && std::isspace(static_cast<unsigned char>(input[pos]))) {
            pos++;
        }
    }

    Tokenized Tokenizer::parse(std::string input, const DataBase& base) {
        std::vector<std::string> beforeEq;
        Tokenized answer;

        // dividing name_1=name_2=expressing into {name_1, name_2}
        for (;;) {
            size_t eqpos = input.find('=');
            if (eqpos == std::string::npos) break;

            if (eqpos == 0 || eqpos == input.size() - 1)
                throw std::runtime_error("Wrong left side of the input");

            // to not mistake with >=, <=, !=, ==, =>
            if ((input[eqpos - 1] == '!' || input[eqpos - 1] == '<' || input[eqpos - 1] == '>' ||
                input[eqpos + 1] == '=' || input[eqpos + 1] == '>')) {
                break;
            }

            beforeEq.push_back(input.substr(0, eqpos));
            input = input.substr(eqpos + 1);
        }

        // allows name_1=name_2(arg_1, arg_2) = expression.
        for (size_t i = 0; i < beforeEq.size(); i++) {
            answer.args.clear();
            size_t localPos = 0;
            skipWhiteSpaces(beforeEq[i], localPos);

            if (localPos == beforeEq[i].size() || !isStringPart(beforeEq[i][localPos]))
                throw std::runtime_error("Wrong naming (empty): " + beforeEq[i]);

            answer.names.push_back(readString(beforeEq[i], localPos));
            skipWhiteSpaces(beforeEq[i], localPos);

            if (localPos == beforeEq[i].size()) continue;

            if (beforeEq[i][localPos] != '(') throw std::runtime_error("Wrong naming (several names inside one [ = ... = ]: " + beforeEq[i]);
            answer.isFunction = true;
            localPos++;

            for (;;) {
                skipWhiteSpaces(beforeEq[i], localPos);
                if (localPos == beforeEq[i].size())
                    throw std::runtime_error("Wrong naming (no closing paranthesis): " + beforeEq[i]);
                if (beforeEq[i][localPos] == ')') 
                    break;
                if (beforeEq[i][localPos] == ',') {
                    localPos++;
                    continue;
                }
                if(!isStringPart(beforeEq[i][localPos]))
                    throw std::runtime_error("Wrong naming (constant in the parameters): " + beforeEq[i]);

                answer.args.push_back(readString(beforeEq[i], localPos));
                skipWhiteSpaces(beforeEq[i], localPos);
            }
        }

        if (input.size() == 0) throw std::runtime_error("No input given");
        size_t pos = 0;
        while (pos < input.size()) {
            if (std::isspace(static_cast<unsigned char>(input[pos]))) {
                pos++;
                continue;
            }
            if (isSpecialChar(input[pos])) {
                answer.mainPart.push_back(std::string(1, input[pos]));
                pos++;
                continue;
            }
            if (isNumberPart(input[pos])) {
                answer.mainPart.push_back(readNumber(input, pos));
            }
            else if (isStringPart(input[pos])) {
                answer.mainPart.push_back(readString(input, pos));
            }
            else {
                answer.mainPart.push_back(readSymbols(input, pos, base));
            }
        }
        return answer;
    }

    std::string Tokenizer::makeExpression(Tokenized& input, int whichName) {
        if (whichName < 0 || whichName >= input.names.size()) throw std::runtime_error("Make expression index out of range");
        std::string ans = input.names[whichName];

        if (input.isFunction) {
            ans += "(";
            for (size_t i = 0; i < input.args.size(); i++) {
                ans += input.args[i];
                if (i != input.args.size() - 1) ans += ", ";
            }
            ans += ")";
        }
        ans += " = ";

        std::vector<std::string>& lines = input.mainPart;
        for (size_t i = 0; i < lines.size(); i++) {
            if (lines[i] == "(")
                ans += "(";
            else if (lines[i] == ",") {
                if(ans.back() == ' ') ans.pop_back();
                ans += ", ";
            }
            else if (lines[i] == ")") {
                if (ans.back() == ' ') ans.pop_back();
                ans += ") ";
            }
            else
                ans += lines[i] + " ";
        }
        if (ans.back() == ' ') ans.pop_back();
        return ans;
    }

    std::string Tokenizer::readNumber(const std::string& input, size_t& pos) {
        std::string number;
        bool hasDot = false;
        while (pos < input.size() && isNumberPart(input[pos])) {
            if (input[pos] == '.') {
                if (hasDot) { pos++; continue; }
                hasDot = true;
            }
            number += input[pos];
            pos++;
        }
        if (!number.empty() && number[0] == '.') number = "0" + number;
        return number;
    }

    std::string Tokenizer::readString(const std::string& input, size_t& pos) {
        std::string result;
        bool readingNumbers = false;
        while (pos < input.size() && isStringPart(input[pos], readingNumbers)) {
            if (input[pos] == '_') readingNumbers = !readingNumbers;
            result += input[pos];
            pos++;
        }
        return result;
    }

    std::string Tokenizer::readSymbols(const std::string& input, size_t& pos, const DataBase& base) {
        size_t start = pos;
        size_t lastMatchLen = 0;
        while (pos < input.size() && isSymbolPart(input[pos])) {
            std::string sub = input.substr(start, pos - start + 1);
            if (const_cast<DataBase&>(base).isFunction(sub)) {
                lastMatchLen = pos - start + 1;
            }
            pos++;
        }
        if (lastMatchLen > 0) {
            pos = start + lastMatchLen;
            return input.substr(start, lastMatchLen);
        }
        throw std::runtime_error("Unknown operator: " + input.substr(start, pos - start));
    }
}