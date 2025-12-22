#include "headers\Supplementary.h"

namespace CalculatorNamespace {
	Node::Node(Node* myHeader, std::vector<Node*>& params, bool isRecursionNode) : associated(myHeader), params(params) {
        if (isRecursionNode) {
            type = static_cast<int>(NodeType::recursion);
            evaluate = [this]() {

#ifdef DEBUG
                std::cout << "Evaluating recursion node" << std::endl;
#endif

                
                std::vector<Node*> savedParams = this->associated->params; // in case there are several recursion calls, we need to save previouse call

                std::vector<Node*> newParams;
                int amountOfArgs = this->params.size();
                for (int i = 0;i < amountOfArgs;i++) {
                    if (this->params[i]->getType() == NodeType::variable) // global variable
                        newParams.push_back(this->params[i]);
                    else if (this->params[i]->getType() == NodeType::input) {
                        Node* headerParam = this->params[i]->associated->params[this->params[i]->getParameterIndex()];

                        if (headerParam == nullptr)
                            throw std::runtime_error("Null pointer was passed into recursion");

                        // local variable or global variable passed as a parameter
                        if((headerParam->getType() == NodeType::variable) || (headerParam->getType() == NodeType::common && headerParam->associated->getType() == NodeType::variable))  
                            newParams.push_back(headerParam);
                        else
                            newParams.push_back(new Node(this->params[i]->evaluate())); //everything except different forms of variables will be evaluated. So yeah, no short circuiting
                    }
                    else
                        newParams.push_back(new Node(this->params[i]->evaluate())); //everything except different forms of variables will be evaluated. So yeah, no short circuiting
                }

                this->associated->params = newParams;
                float result = this->associated->evaluate();
                this->associated->params = savedParams;//putting original parameters back

                for (int i = 0;i < amountOfArgs;i++)
                    if(newParams[i]->getType() == NodeType::constant)
                        delete newParams[i]; // just created and dont need anymore
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
#ifdef DEBUG
        std::cout << "Getting error at assosiated->params.size() == " << associated->params.size() << " and -type -1 == " << -type-1 << std::endl;
#endif
        throw std::runtime_error("Getting type of Input node: parameter index out of range");
    }
    int Node::getParameterIndex()
    {
        if (type >= 0)
            throw std::runtime_error("It is not an input node");
        return -type - 1;
    }

    //header
    Node::Node(int amountOfArgs, int amountOfLocal) : associated(nullptr), type(static_cast<int>(NodeType::header)), 
        evaluate([this, amountOfLocal, amountOfArgs]() {
#ifdef DEBUG
        std::cout << "Calling header node with amount of args == " <<amountOfArgs << " and amount of Locals == " << amountOfLocal << "  now will start checking local variables" << std::endl;
#endif
        //putting local variables to their place
        int currentSize = params.size();
        for (int i = 0;i < amountOfLocal;i++) {
#ifdef DEBUG
            std::cout << "param.size() == " << params.size() << std::endl;
#endif
            if (amountOfArgs + i >= currentSize) {
                params.push_back(new Node(0.0f, NodeType::variable));
#ifdef DEBUG
                std::cout << "lack of param.size(), so pushing new node" << std::endl;
#endif
            }
            else {
#ifdef DEBUG
                std::cout << "params.size() is enough, so replacing null prt (and if already exist, checking if variable) " << std::endl;
#endif
                Node* cur = params[amountOfArgs + i];
                if (cur == nullptr)
                    cur = new Node(0.0f, NodeType::variable);
                else if (cur->getType() != NodeType::variable)
                    throw std::runtime_error("More arguments then needed was passed into the function");
            }
        }

#ifdef DEBUG
        std::cout << "Evaluating header node" << std::endl;
#endif
        float res = this->associated->evaluate();
#ifdef DEBUG
        std::cout << "Finished evaluating header node with result = " << res << std::endl;
#endif

        return res; }) {
#ifdef DEBUG
        std::cout << "created header with " << amountOfArgs << " arguments and " << amountOfLocal << " local variables" << std::endl;
#endif
        params.resize(amountOfArgs + amountOfLocal, nullptr);
    }

    void Node::changeAmountOfLocals(int amountOfArgs, int newAmountOfLocals) {
        if (this->getType() != NodeType::header)
            throw std::runtime_error("Trying to change amount of local variables for non-header node");

        for (int i = params.size() - 1;i >= amountOfArgs + newAmountOfLocals;i--) {
            if (params[i] != nullptr)
                if (params[i]->getType() == NodeType::variable)
                    delete params[i];
                else throw std::runtime_error("non local variable node in local variables section");
        }

        params.resize(amountOfArgs + newAmountOfLocals, nullptr);

        evaluate = [this, newAmountOfLocals, amountOfArgs]() {
#ifdef DEBUG
            std::cout << "Calling header node with amount of args == " << amountOfArgs << " and amount of Locals == " << newAmountOfLocals << "  now will start checking local variables" << std::endl;
#endif
            //putting local variables to their place
            int currentSize = params.size();
            for (int i = 0;i < newAmountOfLocals;i++) {
#ifdef DEBUG
                std::cout << "param.size() == " << params.size() << std::endl;
#endif
                if (amountOfArgs + i >= currentSize) {
                    params.push_back(new Node(0.0f, NodeType::variable));
#ifdef DEBUG
                    std::cout << "lack of param.size(), so pushing new node" << std::endl;
#endif
                }
                else {
#ifdef DEBUG
                    std::cout << "params.size() is enough, so replacing null prt (and if already exist, checking if variable) " << std::endl;
#endif
                    Node* cur = params[amountOfArgs + i];
                    if (cur == nullptr)
                        cur = new Node(0.0f, NodeType::variable);
                    else if (cur->getType() != NodeType::variable)
                        throw std::runtime_error("More arguments then needed was passed into the function");
                }
            }

#ifdef DEBUG
            std::cout << "Evaluating header node" << std::endl;
#endif
            float res = this->associated->evaluate();
#ifdef DEBUG
            std::cout << "Finished evaluating header node with result = " << res << std::endl;
#endif

            return res;
            };
    }
}