#include "headers\Supplementary.h"

namespace CalculatorNamespace {
	Node::Node(Node* myHeader, std::vector<Node*>& params, bool isRecursionNode) : associated(myHeader), params(params) {
        if (isRecursionNode) {
            evaluate = [this]() {
                //when the node is about to evaluate, it calls its associated function and passes parameters to it
                std::vector<Node*> savedParams = this->associated->params; // in case there are several recursions

                std::vector<Node*> newParams;
                int amountOfArgs = this->associated->params.size();
                for (int i = 0;i < amountOfArgs;i++)
                    newParams.push_back(new Node(this->params[i]->evaluate())); // yeah, all parameters we pass into recursion will be calculated, there is no short circuiting here

                this->associated->params = newParams;
                float result = this->associated->evaluate();
                this->associated->params = savedParams;//putting original parameters back

                for (int i = 0;i < amountOfArgs;i++)
                    delete newParams[i]; // dont need anymore

                return result;
                };
        }
        else {
            evaluate = [this]() {
                //when node is about to evaluate, it calls its associated function and passes parameters to it
                std::vector<Node*> savedParams = this->associated->params; // in case that function has several usages and already stores some data
                this->associated->params = this->params; // doesnt allow recursion, since x points to the param and if we replace param with x-1, x will be pointing to x-1 => unending cicle
                float result = this->associated->evaluate();
                this->associated->params = savedParams;//putting original parameters back
                return result;
                };
        }
    }
}