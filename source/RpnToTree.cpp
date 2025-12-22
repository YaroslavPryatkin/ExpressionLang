#include "headers\RpnToTree.h"

namespace CalculatorNamespace {

	RpnToTree::RpnToTree() : header(nullptr), nodeStack() {}

	inline void RpnToTree::clearFields() {
		while (!nodeStack.empty()) nodeStack.pop();
		header = nullptr;
		oldTop = nullptr;
	}

	inline void RpnToTree::addNormalNode(Node* header, int amountOfArgs) {
		std::vector<Node*> args;
		for (int i = 0; i < amountOfArgs; i++) {
			if (nodeStack.empty()) {
				for (int j = 0; j < args.size(); j++) delete args[j];
				somethingWentWrong();
			}
			args.push_back(nodeStack.top());
			nodeStack.pop();
		}
		std::reverse(args.begin(), args.end());
		Node* newNode = new Node(header, args, false); // creating the node that links to the function header, that it should represent, also passing pointers to its children
		nodeStack.push(newNode);
	}

	inline void RpnToTree::addRecursionNode(Node* header, int amountOfArgs) {
		std::vector<Node*> args;
		for (int i = 0; i < amountOfArgs; i++) {
			if (nodeStack.empty()) {
				for (int j = 0; j < args.size(); j++) delete args[j];
				somethingWentWrong();
			}
			args.push_back(nodeStack.top());
			nodeStack.pop();
		}
		std::reverse(args.begin(), args.end());
		Node* newNode = new Node(header, args, true); // creating the node that links to the function header, that it should represent, also passing pointers to its children
		nodeStack.push(newNode);
	}

	inline void RpnToTree::somethingWentWrong() {
		//something went wrong, so we need to delete everything we have created
		while (!nodeStack.empty()) {
			delete nodeStack.top();
			nodeStack.pop();
		}
		if (oldTop == nullptr) {
			if (header != nullptr)
				header->destroyFunction();
		}
		else {
			//we've been changing already existing function, so we need to put everything back
			header->associated = oldTop;
			oldTop = nullptr;
		}
		header = nullptr;
		throw std::runtime_error("Wrong RPN, cannot parse");
	}

	Node* RpnToTree::parse(std::vector<rpnToken>& rpn, int amountOfArgs, int amountOfLocals) {
		clearFields();

		// header.associated = pointer to the root of the tree, Also it contain array of parameters (nodes point to them indirectly using special "input nodes")
		// read constructor
		header = new Node(amountOfArgs, amountOfLocals);


		for (int i = 0; i < rpn.size(); i++) {
			rpnToken cur = rpn[i];
			if (cur.isNumber) nodeStack.push(new Node(cur.value, NodeType::constant)); // constant number
			else {
				if (cur.header == nullptr) {
					if (cur.intValue == -1 && amountOfArgs > 0)
						addRecursionNode(header, amountOfArgs);//recursion (cant have recursion if there is no parameters)
					else if (cur.intValue >= amountOfArgs + amountOfLocals || cur.intValue < -1)
						somethingWentWrong();
					else
						nodeStack.push(new Node(header, cur.intValue)); //input node for parameter (read constructor)
				}
				else
					addNormalNode(cur.header, cur.intValue); //is already defined function
			}
		}

		if (nodeStack.size() != 1)
			somethingWentWrong();

		header->associated = nodeStack.top();
		return header;
	}

	void RpnToTree::parseIntoExisting(std::vector<rpnToken>& rpn, int amountOfArgs, int newAmountOfLocals, Node* header)
	{
		if (header == nullptr) 
			throw std::runtime_error("Trying to change nullptr function");

		clearFields();
		if (header->associated != nullptr) {
			oldTop = header->associated;
			header->associated = nullptr;
		}

		

		// header.associated = pointer to the root of the tree, Also it contain array of parameters (nodes point to them indirectly using special "input nodes")
		// read constructor
		this->header = header;


		for (int i = 0; i < rpn.size(); i++) {
			rpnToken cur = rpn[i];
			if (cur.isNumber) nodeStack.push(new Node(cur.value, NodeType::constant)); // constant number
			else {
				if (cur.header == nullptr) {
					if (cur.intValue == -1 && amountOfArgs > 0)
						addRecursionNode(header, amountOfArgs);//recursion (cant have recursion if there is no parameters)
					else if (cur.intValue >= amountOfArgs + newAmountOfLocals || cur.intValue < -1)
						somethingWentWrong();
					else
						nodeStack.push(new Node(header, cur.intValue)); //input node for parameter (read constructor)
				}
				else
					addNormalNode(cur.header, cur.intValue); //is already defined function
			}
		}

		if (nodeStack.size() != 1)
			somethingWentWrong();

		header->associated = nodeStack.top();
		header->changeAmountOfLocals(amountOfArgs, newAmountOfLocals);
		delete oldTop;
		oldTop = nullptr;
	}
}