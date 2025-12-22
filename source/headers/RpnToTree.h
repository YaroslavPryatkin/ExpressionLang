#pragma once
#include <stack>
#include <vector>
#include "Supplementary.h"

namespace CalculatorNamespace {
    class RpnToTree {
        std::stack<Node*> nodeStack;
        Node* header;
        Node* oldTop; // for changing already existing functions

        inline void clearFields();
        inline void addNormalNode(Node* header, int amountOfArgs);
        inline void addRecursionNode(Node* header, int amountOfArgs);
        inline void somethingWentWrong();

    public:
        RpnToTree();
        Node* parse(std::vector<rpnToken>& rpn, int amountOfArgs = 0, int amounOfLocals = 0);
        void parseIntoExisting(std::vector<rpnToken>& rpn, int amountOfArgs, int newAmountOfLocals, Node* header);
    };
}