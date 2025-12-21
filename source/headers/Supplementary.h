#pragma once
#include <vector>
#include <functional>
#include <string>
#include <stdexcept>


namespace CalculatorNamespace {

    enum class Fix {
        prefix,
        infix,
        postfix
    };

    class Node {
    public:
        std::vector<Node*> params;
        Node* associated;
        std::function<float()> evaluate;

        //header of user-defined function, should be created only with RPNToTree::createNewFunction method
        Node(int amountOfArgs) : associated(nullptr), evaluate([this]() { return this->associated->evaluate(); }) {
            params.resize(amountOfArgs, nullptr);
        }

        //input node
        Node(Node* header, int argNumber) : associated(header), evaluate([this, argNumber]() {
            if (this->associated->params[argNumber] == nullptr)
                throw std::runtime_error("No parameter was passed to the function");
            return this->associated->params[argNumber]->evaluate();
            }) {
        }

        //common or recursion node
        Node(Node* myHeader, std::vector<Node*>& params, bool isRecursionNode = false);


        //constant number, also can be variable if stored in dataBase 
        Node(float value) : associated(nullptr), evaluate([value]() { return value; }) {}

        // update variable value
        void set(float value) {
            evaluate = [value]() { return value; };
        }

        //preDefined function
        Node(int amountOfArgs, std::function<float(Node* self)> evaluate_func) :
            associated(nullptr),
            evaluate([this, evaluate_func]() { return evaluate_func(this); }) { //lambda magic
            if(amountOfArgs>=0 ) 
                params.resize(amountOfArgs, nullptr);
        }

        ~Node() {
            for (auto& nd : params) {
                if (nd != nullptr) delete nd;
            }
        }
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

        //CALL ONLY IF IT IS THE HEADER, also can ruin dependent functions
        void destroyFunction() {
            if (associated != nullptr)
                delete associated;
            delete this;
        }
    };

    struct rpnToken {
        Node* header = nullptr;
        int intValue = 0;
        bool isNumber = false;
        float value = 0.0f;

        rpnToken() : intValue(-1) {} // recursion function
        rpnToken(float val) : isNumber(true), value(val) {} // for constants
        rpnToken(Node* h) : header(h), intValue(0) {} // for variables = functions with 0 arguments
        rpnToken(Node* h, int args) : header(h), intValue(args) {} // for functions
        rpnToken(int paramIdx) : intValue(paramIdx) {} // for parameters
    };
}