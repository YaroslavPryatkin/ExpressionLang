#include "headers\Supplementary.h"

namespace CalculatorNamespace {
	Node::Node(Node* myHeader, std::vector<Node*>& params, bool isRecursionNode) : associated(myHeader), params(params) {
        if (isRecursionNode) {
            type = static_cast<int>(NodeType::recursion);
            evaluate = [this]() {

#ifdef DEBUG
                std::cout << "Evaluating recursion node" << std::endl;
#endif

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
#ifdef DEBUG
                std::cout << "Finished evaluating recursion node with result = " << result << std::endl;
#endif

                return result;
                };
        }
        else {
            type = static_cast<int>(NodeType::common);
            if (params.size() == 0) {
                //function with zero arguments = variable input
                evaluate = [this]() {


#ifdef DEBUG
                    std::cout << "Evaluating common node" << std::endl;
#endif
                    float res = this->associated->evaluate();
#ifdef DEBUG
                    std::cout << "Finished evaluating common node with result = " << res << std::endl;
#endif

                    return res;
                    };
            }
            else {
                //normal function
                evaluate = [this]() {

#ifdef DEBUG
                    std::cout << "Evaluating common node" << std::endl;
#endif



                    //when node is about to evaluate, it calls its associated function and passes parameters to it
                    std::vector<Node*> savedParams = this->associated->params; // in case that function has several usages and already stores some data
                    this->associated->params = this->params; // doesnt allow recursion, since x points to the param and if we replace param with x-1, x will be pointing to x-1 => unending cicle
                    float result = this->associated->evaluate();
                    this->associated->params = savedParams;//putting original parameters back

#ifdef DEBUG
                    std::cout << "Finished evaluating common node with result = " << result << std::endl;
#endif

                    return result;
                    };
            }
        }
    }

    NodeType Node::getType() {
        if (type >= static_cast<int>(NodeType::input))
            throw std::runtime_error("Node has wrong type");
        if (type >= 0)
            return static_cast<NodeType>(type);
        if (-type - 1 < associated->params.size())
            return NodeType::input;
        throw std::runtime_error("Input node: paramter index out of range");
    }
    int Node::getParameterIndex()
    {
        if (type >= 0)
            throw std::runtime_error("It is not an input node");
        return -type - 1;
    }
}