#pragma once
#include <vector>
#include <functional>
#include <string>
#include <stdexcept>
#include <iostream>


namespace CalculatorNamespace {

    enum class Fix {
        prefix,
        infix,
        postfix
    };

    enum class NodeType {
        preDefined, //header of pre defined function
        header, // header of user defined function
        variable, // header of the variable (technically, it is just a constant that lies in variables set)
        common, // points to the header of the real function
        recursion, // points to the header of it's own function. Different with common node is the fact, that it will evaluate all arguments before passing them into header params
        constant, // just returns some value
        input // cant actually be in the field NodeType type; input node for parameter i is coded as type = -i - 1; must be the last on the list
    };

#ifdef DEBUG
    inline const char* toString(NodeType type) {
        switch (type) {
        case NodeType::preDefined: return "preDefined";
        case NodeType::header:     return "header";
        case NodeType::variable:   return "variable";
        case NodeType::common:     return "common";
        case NodeType::recursion:  return "recursion";
        case NodeType::constant:   return "constant";
        case NodeType::input:      return "input";
        default:                   return "unknown NodeType";
        }
    }
#endif

    class Node {
        int type;
    public:
        std::vector<Node*> params;
        Node* associated;
        std::function<float()> evaluate;
        


        //header of user-defined function, should be created only with RPNToTree::createNewFunction method
        Node(int amountOfArgs, int amountOfLocal);

        void changeAmountOfLocals(int amountOfArgs, int newAmountOfLocals);

        //input node
        Node(Node* header, int argNumber) : associated(header), type(-argNumber-1), evaluate([this, argNumber]() {
            if (this->associated->params[argNumber] == nullptr)
                throw std::runtime_error("No parameter was passed to the function");
            
#ifdef DEBUG
            std::cout << "Evaluating input node" << std::endl;
#endif
            float res = this->associated->params[argNumber]->evaluate();
#ifdef DEBUG
            std::cout << "Finished evaluating input node with result = " << res << std::endl;
#endif

            return res;
            }) {
        }

        //common or recursion node
        Node(Node* myHeader, std::vector<Node*>& params, bool isRecursionNode = false);


        //constant number, also can be variable if stored in dataBase 
        Node(float value, NodeType type = NodeType::constant) : associated(nullptr),  evaluate([
#ifdef DEBUG
            this, 
#endif
            value]() {

#ifdef DEBUG
            std::cout << "Evaluating " << (this -> type == static_cast<int>(NodeType::constant) ? "constant" : "variable") <<  " node" << std::endl;
            std::cout << "Finished evaluating " << (this ->type == static_cast<int>(NodeType::constant) ? "constant" : "variable") << " node with result = " << value << std::endl;
#endif

            return value; 
            }) {
            if(type != NodeType::constant && type != NodeType::variable)
                throw std::runtime_error("Wrong node type passed to constructor");


            this->type = static_cast<int>(type);
        }

        // update variable value
        void set(float value) {
            if(type != static_cast<int>(NodeType::variable))
                throw std::runtime_error("Trying to set new value to non-variable node");
            evaluate = [value]() { return value; };
        }

        //preDefined function
        Node(int amountOfArgs, std::function<float(Node* self)> evaluate_func) :
            associated(nullptr), type(static_cast<int>(NodeType::preDefined)),
            evaluate([this, evaluate_func]() { 
            
#ifdef DEBUG
            std::cout << "Evaluating preDefined node" << std::endl;
#endif
            float res = evaluate_func(this);
#ifdef DEBUG
            std::cout << "Finished evaluating preDefined node with result = " << res << std::endl;
#endif
            
            return res; }) { //lambda magic
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
            if(type != static_cast<int>(NodeType::header))
                throw std::runtime_error("Trying to call destroy function from non-header node");
            if (associated != nullptr)
                delete associated;
            delete this;
        }

        NodeType getType();
        int getParameterIndex();
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