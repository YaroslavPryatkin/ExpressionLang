#include "headers\DataBase.h"

namespace CalculatorNamespace {

	UserDefinedFunction::UserDefinedFunction() : expression(""), header(nullptr), amountOfArguments(0) {}

	UserDefinedFunction::UserDefinedFunction(std::string expression, Node* header, int amountOfArguments, std::set<UserDefinedFunction*> dependencies, std::set<Variable*> variableDependencies) :
		expression(expression), header(header), amountOfArguments(amountOfArguments), dependencies(dependencies), variableDependencies(variableDependencies){
		if (amountOfArguments < 0)
			throw std::runtime_error("Negative amount of arguments was passed");

	}

	UserDefinedFunction::UserDefinedFunction(UserDefinedFunction&& other) noexcept
		: header(other.header), expression(std::move(other.expression)), amountOfArguments(std::move(other.amountOfArguments)), 
		dependencies(std::move(other.dependencies)), dependants(std::move(other.dependants)), variableDependencies(std::move(other.variableDependencies)){
		other.header = nullptr;
	}

	UserDefinedFunction& UserDefinedFunction::operator=(UserDefinedFunction&& other) noexcept {
		if (this != &other) {
			header = other.header;
			expression = std::move(other.expression);
			amountOfArguments = other.amountOfArguments;
			dependencies = other.dependencies;
			dependants = other.dependants;
			variableDependencies = other.variableDependencies;
			other.header = nullptr;
		}
		return *this;
	}

	UserDefinedFunction::~UserDefinedFunction() {
		if (header)
			header->destroyFunction();
	}
	
	
	
	
	Variable::Variable() : name(""), header(nullptr) {}

	Variable::Variable(std::string name, float value) : name(name), header(new Node(value, NodeType::variable)) {}

	Variable::Variable(Variable&& other) noexcept
		: header(other.header), name(std::move(other.name)) {
		other.header = nullptr;
	}

	Variable& Variable::operator=(Variable&& other) noexcept {
		if (this != &other) {
			header = other.header;
			name = std::move(other.name);
			other.header = nullptr;
		}
		return *this;
	}

	Variable::~Variable() {
		if (header)
			header->destroyFunction();
	}

	
	
	PreDefinedFunction::PreDefinedFunction(std::string name, std::string expression, std::string comment, int amountOfArguments, Fix fix, int precedence, bool rightAssociative, std::function<float(Node* self)> evaluate_func) :
		name(name), expression(expression), comment(comment), amountOfArguments(amountOfArguments), fix(fix),
		precedence(precedence), rightAssociative(rightAssociative), header(new Node(amountOfArguments, evaluate_func)) {
	}

	
	
	
	DataBase::DataBase(std::istream& in, std::ostream& out, int consoleWidth) : in(in), out(out), consoleWidth(consoleWidth) {
		preDefined = {
				{implicitOperationName, {
					new PreDefinedFunction("implicit *", "a*b", "Implicit multiplication", 2, Fix::infix, 60, false,
									[](Node* self) {
							if (self->params[0] == nullptr || self->params[1] == nullptr)
								throw std::runtime_error("No parameter was passed to the function");
							return self->params[0]->evaluate() * self->params[1]->evaluate();
						}
					)
				}
			},
			{"=", {
					new PreDefinedFunction("=", "a=1", "Assigning the value to the variable, return the assigned value", 2, Fix::infix, 9, true,
									[](Node* self) {
							if (self->params[0] == nullptr || self->params[1] == nullptr)
								throw std::runtime_error("No parameter was passed to the function");

							Node* zeroParam = self->params[0];

							if (zeroParam->getType() == NodeType::common && zeroParam->associated->getType() == NodeType::variable) {
								float res = self->params[1]->evaluate();
								zeroParam->associated->set(res);

								return res;
							}

							if (zeroParam->getType() == NodeType::input){
								Node* headerParam = zeroParam->associated->params[zeroParam->getParameterIndex()];
								if (headerParam != nullptr && headerParam->getType() == NodeType::common && headerParam->associated->getType() == NodeType::variable) {
									float res = self->params[1]->evaluate();
									headerParam->associated->set(res);

									return res;
								}
							}
							throw std::runtime_error("Trying to assign a value to the non-variable");
						}
					)
				}
			},
			{"+=", {
					new PreDefinedFunction("+=", "a+=b", "a = a + b", 2, Fix::infix, 9, true,
									[](Node* self) {
							if (self->params[0] == nullptr || self->params[1] == nullptr)
								throw std::runtime_error("No parameter was passed to the function");

							Node* zeroParam = self->params[0];

							if (zeroParam->getType() == NodeType::common && zeroParam->associated->getType() == NodeType::variable) {
								float resRight = self->params[1]->evaluate();
								float resLeft = zeroParam->associated->evaluate();
								zeroParam->associated->set(resLeft + resRight);

								return resLeft + resRight;
							}

							if (zeroParam->getType() == NodeType::input) {
								Node* headerParam = zeroParam->associated->params[zeroParam->getParameterIndex()];
								if (headerParam != nullptr && headerParam->getType() == NodeType::common && headerParam->associated->getType() == NodeType::variable) {
									float resRight = self->params[1]->evaluate();
									float resLeft = headerParam->associated->evaluate();
									headerParam->associated->set(resLeft + resRight);

									return resLeft + resRight;
								}
							}
							throw std::runtime_error("Trying to assign a value to the non-variable");
						}
					)
				}
			},
			{"-=", {
					new PreDefinedFunction("-=", "a-=b", "a = a - b", 2, Fix::infix, 9, true,
									[](Node* self) {
							if (self->params[0] == nullptr || self->params[1] == nullptr)
								throw std::runtime_error("No parameter was passed to the function");

							Node* zeroParam = self->params[0];

							if (zeroParam->getType() == NodeType::common && zeroParam->associated->getType() == NodeType::variable) {
								float resRight = self->params[1]->evaluate();
								float resLeft = zeroParam->associated->evaluate();
								zeroParam->associated->set(resLeft - resRight);

								return resLeft - resRight;
							}

							if (zeroParam->getType() == NodeType::input) {
								Node* headerParam = zeroParam->associated->params[zeroParam->getParameterIndex()];
								if (headerParam != nullptr && headerParam->getType() == NodeType::common && headerParam->associated->getType() == NodeType::variable) {
									float resRight = self->params[1]->evaluate();
									float resLeft = headerParam->associated->evaluate();
									headerParam->associated->set(resLeft - resRight);

									return resLeft - resRight;
								}
							}
							throw std::runtime_error("Trying to assign a value to the non-variable");
						}
					)
				}
			},
			{ "*=", {
				new PreDefinedFunction("*=", "a*=b", "a = a * b", 2, Fix::infix, 9, true,
					[](Node* self) {
						if (!self->params[0] || !self->params[1])
							throw std::runtime_error("No parameter was passed to the function");

						Node* zeroParam = self->params[0];

						if (zeroParam->getType() == NodeType::common && zeroParam->associated->getType() == NodeType::variable) {
							float resRight = self->params[1]->evaluate();
							float resLeft = zeroParam->associated->evaluate();
							zeroParam->associated->set(resLeft * resRight);

							return resLeft * resRight;
						}

						if (zeroParam->getType() == NodeType::input) {
							Node* headerParam = zeroParam->associated->params[zeroParam->getParameterIndex()];
							if (headerParam && headerParam->getType() == NodeType::common && headerParam->associated->getType() == NodeType::variable) {
								float resRight = self->params[1]->evaluate();
								float resLeft = headerParam->associated->evaluate();
								headerParam->associated->set(resLeft * resRight);

								return resLeft * resRight;
							}
						}

						throw std::runtime_error("Trying to assign a value to the non-variable");
					}
				)
			} },

			{ "/=", {
				new PreDefinedFunction("/=", "a/=b", "a = a / b", 2, Fix::infix, 9, true,
					[](Node* self) {
						if (!self->params[0] || !self->params[1])
							throw std::runtime_error("No parameter was passed to the function");

						Node* zeroParam = self->params[0];

						if (zeroParam->getType() == NodeType::common && zeroParam->associated->getType() == NodeType::variable) {
							float resRight = self->params[1]->evaluate();
							if (resRight == 0.0f)
								throw std::runtime_error("Division by zero");

							float resLeft = zeroParam->associated->evaluate();
							zeroParam->associated->set(resLeft / resRight);

							return resLeft / resRight;
						}

						if (zeroParam->getType() == NodeType::input) {
							Node* headerParam = zeroParam->associated->params[zeroParam->getParameterIndex()];
							if (headerParam && headerParam->getType() == NodeType::common && headerParam->associated->getType() == NodeType::variable) {
								float resRight = self->params[1]->evaluate();
								if (resRight == 0.0f)
									throw std::runtime_error("Division by zero");

								float resLeft = headerParam->associated->evaluate();
								headerParam->associated->set(resLeft / resRight);

								return resLeft / resRight;
							}
						}

						throw std::runtime_error("Trying to assign a value to the non-variable");
					}
				)
			} },

			{ "^=", {
				new PreDefinedFunction("^=", "a^=b", "a = a ^ b", 2, Fix::infix, 9, true,
					[](Node* self) {
						if (!self->params[0] || !self->params[1])
							throw std::runtime_error("No parameter was passed to the function");

						Node* zeroParam = self->params[0];

						if (zeroParam->getType() == NodeType::common && zeroParam->associated->getType() == NodeType::variable) {
							float resRight = self->params[1]->evaluate();
							float resLeft = zeroParam->associated->evaluate();
							zeroParam->associated->set(std::pow(resLeft, resRight));

							return std::pow(resLeft, resRight);
						}

						if (zeroParam->getType() == NodeType::input) {
							Node* headerParam = zeroParam->associated->params[zeroParam->getParameterIndex()];
							if (headerParam && headerParam->getType() == NodeType::common && headerParam->associated->getType() == NodeType::variable) {
								float resRight = self->params[1]->evaluate();
								float resLeft = headerParam->associated->evaluate();
								headerParam->associated->set(std::pow(resLeft, resRight));

								return std::pow(resLeft, resRight);
							}
						}

						throw std::runtime_error("Trying to assign a value to the non-variable");
					}
				)
			} 
			},
			{ "++", {
					new PreDefinedFunction("++", "++a", "Increment variable (prefix)", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
						[](Node* self) {
							if (!self->params[0])
								throw std::runtime_error("No parameter passed to ++");

							Node* varNode = self->params[0];

							if (varNode->getType() == NodeType::common && varNode->associated->getType() == NodeType::variable) {
								float val = varNode->associated->evaluate() + 1.0f;
								varNode->associated->set(val);
								return val;
							}

							if (varNode->getType() == NodeType::input) {
								Node* headerParam = varNode->associated->params[varNode->getParameterIndex()];
								if (!headerParam || headerParam->getType() != NodeType::common || headerParam->associated->getType() != NodeType::variable)
									throw std::runtime_error("Trying to increment a non-variable");

								float val = headerParam->associated->evaluate() + 1.0f;
								headerParam->associated->set(val);
								return val;
							}

							throw std::runtime_error("Trying to increment a non-variable");
						}
					),
					new PreDefinedFunction("++", "a++", "Increment variable (postfix)", 1, Fix::postfix, defaultPrefixFunctionPrecedence, false,
						[](Node* self) {
							if (!self->params[0])
								throw std::runtime_error("No parameter passed to ++");

							Node* varNode = self->params[0];
							float oldVal;

							if (varNode->getType() == NodeType::common && varNode->associated->getType() == NodeType::variable) {
								oldVal = varNode->associated->evaluate();
								varNode->associated->set(oldVal + 1.0f);
								return oldVal;
							}

							if (varNode->getType() == NodeType::input) {
								Node* headerParam = varNode->associated->params[varNode->getParameterIndex()];
								if (!headerParam || headerParam->getType() != NodeType::common || headerParam->associated->getType() != NodeType::variable)
									throw std::runtime_error("Trying to increment a non-variable");

								oldVal = headerParam->associated->evaluate();
								headerParam->associated->set(oldVal + 1.0f);
								return oldVal;
							}

							throw std::runtime_error("Trying to increment a non-variable");
						}
					)
				}
			},
			{ "--", {
					new PreDefinedFunction("--", "--a", "Decrement variable (prefix)", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
						[](Node* self) {
							if (!self->params[0])
								throw std::runtime_error("No parameter passed to --");

							Node* varNode = self->params[0];

							if (varNode->getType() == NodeType::common && varNode->associated->getType() == NodeType::variable) {
								float val = varNode->associated->evaluate() - 1.0f;
								varNode->associated->set(val);
								return val;
							}

							if (varNode->getType() == NodeType::input) {
								Node* headerParam = varNode->associated->params[varNode->getParameterIndex()];
								if (!headerParam || headerParam->getType() != NodeType::common || headerParam->associated->getType() != NodeType::variable)
									throw std::runtime_error("Trying to decrement a non-variable");

								float val = headerParam->associated->evaluate() - 1.0f;
								headerParam->associated->set(val);
								return val;
							}

							throw std::runtime_error("Trying to decrement a non-variable");
						}
					),
					new PreDefinedFunction("--", "a--", "Decrement variable (postfix)", 1, Fix::postfix, defaultPrefixFunctionPrecedence, false,
						[](Node* self) {
							if (!self->params[0])
								throw std::runtime_error("No parameter passed to --");

							Node* varNode = self->params[0];
							float oldVal;

							if (varNode->getType() == NodeType::common && varNode->associated->getType() == NodeType::variable) {
								oldVal = varNode->associated->evaluate();
								varNode->associated->set(oldVal - 1.0f);
								return oldVal;
							}

							if (varNode->getType() == NodeType::input) {
								Node* headerParam = varNode->associated->params[varNode->getParameterIndex()];
								if (!headerParam || headerParam->getType() != NodeType::common || headerParam->associated->getType() != NodeType::variable)
									throw std::runtime_error("Trying to decrement a non-variable");

								oldVal = headerParam->associated->evaluate();
								headerParam->associated->set(oldVal - 1.0f);
								return oldVal;
							}

							throw std::runtime_error("Trying to decrement a non-variable");
						}
					)
				} 
			},
			{";", {
					new PreDefinedFunction(";", "code; code", "Evaluates both sides, returns right-size result", 2, Fix::infix, 8, false,
									[](Node* self) {
							if (self->params[0] == nullptr || self->params[1] == nullptr)
								throw std::runtime_error("No parameter was passed to the function");

							self->params[0]->evaluate();
							return self->params[1]->evaluate();
						}
					)
				}
			},
			{"while", {
					new PreDefinedFunction("while", "while(condition, code)", "while cond != 0.0f do code, returns amount of cicles", 2, Fix::prefix, defaultPrefixFunctionPrecedence, false,
									[](Node* self) {
							if (self->params[0] == nullptr || self->params[1] == nullptr)
								throw std::runtime_error("No parameter was passed to the function");

							float amountOfCicles = 0.0f;
							while (self->params[0]->evaluate() != 0.0f) {
								self->params[1]->evaluate();
								amountOfCicles++;
							}
							return amountOfCicles;
						}
					)
				}
			},
			{"for", {
					new PreDefinedFunction("for", "for(start, condition, code)", "Equals to: start; while( condition, code)", 3, Fix::prefix, defaultPrefixFunctionPrecedence, false,
									[](Node* self) {
							if (self->params[0] == nullptr || self->params[1] == nullptr || self->params[2] == nullptr)
								throw std::runtime_error("No parameter was passed to the function");

							self->params[0]->evaluate();
							float amountOfCicles = 0.0f;
							while (self->params[1]->evaluate() != 0.0f) {
								self->params[2]->evaluate();
								amountOfCicles++;
							}
							return amountOfCicles;
						}
					),
					new PreDefinedFunction("for", "for(start, condition, iter, code)", "Equals to: start; while( condition, code; iter)", 4, Fix::prefix, defaultPrefixFunctionPrecedence, false,
									[](Node* self) {
							if (self->params[0] == nullptr || self->params[1] == nullptr || self->params[2] == nullptr || self->params[3] == nullptr)
								throw std::runtime_error("No parameter was passed to the function");

							self->params[0]->evaluate();
							float amountOfCicles = 0.0f;
							while (self->params[1]->evaluate() != 0.0f) {
								self->params[3]->evaluate();
								self->params[2]->evaluate();
								amountOfCicles++;
							}
							return amountOfCicles;
						}
					)
				}
			},
			{ "+", {
					new PreDefinedFunction("+", "a+b", "Just addition", 2, Fix::infix, 50, false,
									[](Node* self) {
							if (self->params[0] == nullptr || self->params[1] == nullptr)
								throw std::runtime_error("No parameter was passed to the function");
							float left = self->params[0]->evaluate();
							float right = self->params[1]->evaluate();
							return left + right;
						}
					)
				}
			},
			{"-", {
					new PreDefinedFunction("-2", "a-b", "Just subtraction", 2, Fix::infix, 50, false,
									[](Node* self) {
							if (self->params[0] == nullptr || self->params[1] == nullptr)
								throw std::runtime_error("No parameter was passed to the function");
							float left = self->params[0]->evaluate();
							float right = self->params[1]->evaluate();
							return left - right;
						}
					),
						new PreDefinedFunction("-1", "-a", "Unary minus", 1, Fix::prefix, 80, false,
									[](Node* self) {
								if (self->params[0] == nullptr)
									throw std::runtime_error("No parameter was passed to the function");
								return 0.0f - self->params[0]->evaluate();
						}
					)
				}
			},
			{"*", {
					new PreDefinedFunction("*", "a*b", "Just multiplication", 2, Fix::infix, 60, false,
									[](Node* self) {
							if (self->params[0] == nullptr || self->params[1] == nullptr)
								throw std::runtime_error("No parameter was passed to the function");
							float left = self->params[0]->evaluate();
							float right = self->params[1]->evaluate();
							return left * right;
						}
					)
				}
			},
			{"/", {
					new PreDefinedFunction("/", "a/b", "Just division", 2, Fix::infix, 60, false,
									[](Node* self) {
							if (self->params[0] == nullptr || self->params[1] == nullptr)
								throw std::runtime_error("No parameter was passed to the function");
							float left = self->params[0]->evaluate();
							float right = self->params[1]->evaluate();
							return left / right;
						}
					)
				}
			},
			{"^", {
					new PreDefinedFunction("^", "a^b", "Exponetiating a to the power of b", 2, Fix::infix, 70, true,
									[](Node* self) {
							if (self->params[0] == nullptr || self->params[1] == nullptr)
								throw std::runtime_error("No parameter was passed to the function");
							float left = self->params[0]->evaluate();
							float right = self->params[1]->evaluate();
							return static_cast<float>(std::pow(left, right));
						}
					)
				}
			},
			{"!", {
				new PreDefinedFunction("!", "a!", "Factorial", 1, Fix::postfix, 90, false,
					[](Node* self) {
						if (!self->params[0]) throw std::runtime_error("No parameter");
						float val = self->params[0]->evaluate();
						if (val < 0) throw std::runtime_error("Factorial of negative number");
						float res = 1.0f;
						for (int i = 1; i <= static_cast<int>(val); ++i) res *= i;
						return res;
					}
				)
			}},

			{"pow", {
				new PreDefinedFunction("pow2", "pow(a,b)", "a to the power of b", 2, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						return std::pow(self->params[0]->evaluate(), self->params[1]->evaluate());
					}
				),
				new PreDefinedFunction("pow1", "pow(a)", "10 to the power of a", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						return std::pow(10.0f, self->params[0]->evaluate());
					}
				)
			}},

			{"root", {
				new PreDefinedFunction("root2", "root(a,b)", "root a base b", 2, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						float val = self->params[1]->evaluate();
						if (val == 0.0f) return 0.0f;
						return std::pow(self->params[0]->evaluate(), 1.0f/val);
					}
				),
				new PreDefinedFunction("root1", "root(a)", "root a base 2", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						return std::sqrt(self->params[0]->evaluate());
					}
				)
			}},

			{ "sqrt", {
				new PreDefinedFunction("sqrt", "sqrt(a)", "root a base 2", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						return std::sqrt(self->params[0]->evaluate());
					}
				)
			} },

			{"exp", {
				new PreDefinedFunction("exp", "exp(a)", "e to the power of a", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						return std::exp(self->params[0]->evaluate());
					}
				)
			}},

			{"log", {
				new PreDefinedFunction("log2", "log(a,b)", "log b base a", 2, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						return std::log(self->params[1]->evaluate()) / std::log(self->params[0]->evaluate());
					}
				),
				new PreDefinedFunction("log1", "log(a)", "log a base 10", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						return std::log10(self->params[0]->evaluate());
					}
				)
			}},

			{"ln", {
				new PreDefinedFunction("ln", "ln(a)", "natural logarithm", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						return std::log(self->params[0]->evaluate());
					}
				)
			}},

			{"sum", {
				new PreDefinedFunction("sum", "sum(...)", "sum of all arguments", -1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						float res = 0;
						for (auto& p : self->params) if (p) res += p->evaluate();
						return res;
					}
				)
			}},

			{"prod", {
				new PreDefinedFunction("prod", "prod(...)", "product of all arguments", -1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						float res = 1;
						for (auto& p : self->params) if (p) res *= p->evaluate();
						return res;
					}
				)
			}},

			{"max", {
				new PreDefinedFunction("max", "max(...)", "maximum of arguments", -1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						if (self->params.empty()) return 0.0f;
						float res = self->params[0]->evaluate();
						for (size_t i = 1; i < self->params.size(); ++i)
							res = std::max(res, self->params[i]->evaluate());
						return res;
					}
				)
			}},

			{"min", {
				new PreDefinedFunction("min", "min(...)", "minimum of arguments", -1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						if (self->params.empty()) return 0.0f;
						float res = self->params[0]->evaluate();
						for (size_t i = 1; i < self->params.size(); ++i)
							res = std::min(res, self->params[i]->evaluate());
						return res;
					}
				)
			}},

			{ "!=", { new PreDefinedFunction("!=", "a!=b", "not equal", 2, Fix::infix, 40, false, [](Node* self) { return (self->params[0]->evaluate() != self->params[1]->evaluate()) ? 1.0f : 0.0f; }) } },
			{ "==", { new PreDefinedFunction("==", "a==b", "equal", 2, Fix::infix, 40, false, [](Node* self) { return (self->params[0]->evaluate() == self->params[1]->evaluate()) ? 1.0f : 0.0f; }) } },
			{ ">=", { new PreDefinedFunction(">=", "a>=b", "greater or equal", 2, Fix::infix, 40, false, [](Node* self) { return (self->params[0]->evaluate() >= self->params[1]->evaluate()) ? 1.0f : 0.0f; }) } },
			{ "<=", { new PreDefinedFunction("<=", "a<=b", "less or equal", 2, Fix::infix, 40, false, [](Node* self) { return (self->params[0]->evaluate() <= self->params[1]->evaluate()) ? 1.0f : 0.0f; }) } },
			{ "<",  { new PreDefinedFunction("<", "a<b", "less", 2, Fix::infix, 40, false, [](Node* self) { return (self->params[0]->evaluate() < self->params[1]->evaluate()) ? 1.0f : 0.0f; }) } },
			{ ">",  { new PreDefinedFunction(">", "a>b", "greater", 2, Fix::infix, 40, false, [](Node* self) { return (self->params[0]->evaluate() > self->params[1]->evaluate()) ? 1.0f : 0.0f; }) } },

			{"if", {
				new PreDefinedFunction("if", "if(cond, then, else)", "conditional", 3, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						if (self->params[0]->evaluate() != 0.0f) return self->params[1]->evaluate();
						return self->params[2]->evaluate();
					}
				)
			}},

			{"&&", {
				new PreDefinedFunction("&&", "a&&b", "logical AND", 2, Fix::infix, 30, false,
					[](Node* self) {
						if (self->params[0]->evaluate() == 0.0f) return 0.0f;
						return self->params[1]->evaluate() != 0.0f ? 1.0f : 0.0f;
					}
				)
			}},

			{"||", {
				new PreDefinedFunction("||", "a||b", "logical OR", 2, Fix::infix, 20, false,
					[](Node* self) {
						if (self->params[0]->evaluate() != 0.0f) return 1.0f;
						return self->params[1]->evaluate() != 0.0f ? 1.0f : 0.0f;
					}
				)
			}},

			{"=>", {
				new PreDefinedFunction("=>", "a=>b", "implication", 2, Fix::infix, 10, true,
					[](Node* self) {
						if (self->params[0]->evaluate() == 0.0f) return 1.0f;
						return self->params[1]->evaluate() != 0.0f ? 1.0f : 0.0f;
					}
				)
			}},
			{ "sin", {
				new PreDefinedFunction("sin", "sin(a)", "sine function", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						return std::sin(self->params[0]->evaluate());
					}
				)
			} },

			{ "cos", {
				new PreDefinedFunction("cos", "cos(a)", "cosine function", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						return std::cos(self->params[0]->evaluate());
					}
				)
			} },

			{ "tan", {
				new PreDefinedFunction("tan", "tan(a)", "tangent function", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						float val = self->params[0]->evaluate();
						float cos_val = std::cos(val);
						if (std::abs(cos_val) < 1e-7) throw std::runtime_error("Tangent is undefined (division by zero)");
						return std::tan(val);
					}
				)
			} },

			{ "ctg", {
				new PreDefinedFunction("ctg", "ctg(a)", "cotangent function", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						float val = self->params[0]->evaluate();
						float sin_val = std::sin(val);
						if (std::abs(sin_val) < 1e-7) throw std::runtime_error("Cotangent is undefined (division by zero)");
						return 1.0f / std::tan(val);
					}
				)
			} },

			{ "asin", {
				new PreDefinedFunction("asin", "asin(a)", "arcsine function", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						float val = self->params[0]->evaluate();
						if (val < -1.0f || val > 1.0f) throw std::runtime_error("asin argument out of range [-1, 1]");
						return std::asin(val);
					}
				)
			} },

			{ "acos", {
				new PreDefinedFunction("acos", "acos(a)", "arccosine function", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						float val = self->params[0]->evaluate();
						if (val < -1.0f || val > 1.0f) throw std::runtime_error("acos argument out of range [-1, 1]");
						return std::acos(val);
					}
				)
			} },

			{ "atan", {
				new PreDefinedFunction("atan", "atan(a)", "arctangent function", 1, Fix::prefix, defaultPrefixFunctionPrecedence, false,
					[](Node* self) {
						return std::atan(self->params[0]->evaluate());
					}
				)
			} }
		};
		initDefaultVariables();
	}



	inline void DataBase::initDefaultVariables() {
		if(variables.find("pi")==variables.end()) 
			variables.insert(std::make_pair("pi", new Variable("pi", 3.14159265f)));
		if (variables.find("e") == variables.end())
			variables.insert(std::make_pair("e", new Variable("e", 2.71828182f)));
		if (variables.find("phi") == variables.end())
			variables.insert(std::make_pair("phi", new Variable("phi", 1.618034f)));
	}

	inline void DataBase::checkIfExit(std::string& input) {
		if (input != "exit") return;
		inputBufferClean();
		throw std::runtime_error("You have typed exit");
	}

	inline void DataBase::inputBufferClean() {
		in.clear();
		in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}




	Variable* DataBase::changeOrSetVariable(std::string name, float newValue) {
		if (variables.find(name) != variables.end()) variables[name]->header->set(newValue);
		else variables.insert(std::make_pair(name, new Variable(name, newValue)));
		return variables.at(name);
	}

	Variable* DataBase::askForAVariable(std::string name) {
		out << "Enter the value for varible " << name << " (or type exit):" << std::endl << "> ";
		std::string input;
		in >> input;
		checkIfExit(input);
		inputBufferClean();
		float value;
		try {
			size_t pos;
			value = std::stof(input, &pos);

			//if input == 12.3abc, then it is not a number
			if (pos != input.size()) throw std::runtime_error("Invalid number input: " + input);

		}
		catch (...) {
			throw std::runtime_error("Invalid number input: " + input);
		}
		return changeOrSetVariable(name, value);
	}

	const bool DataBase::isVariable(std::string name) {
		return variables.find(name) != variables.end();
	}

	rpnToken DataBase::makeRpnTokenFromVariable(const std::string& name, std::set<Variable*>& variableDependancies, bool shouldAsk) {
		Variable* var;
		if (variables.find(name) != variables.end())  var = variables[name];
		else if (shouldAsk) var = askForAVariable(name);
		else var = changeOrSetVariable(name, 0.0f);
		variableDependancies.insert(var);
		return rpnToken(var->header);
	}


	//returns precedence and rightAssociative
	const std::pair<int, bool> DataBase::functionInfo(std::string name, Fix fix) {
		if (fix == Fix::prefix)
			return std::pair<int, bool>(defaultPrefixFunctionPrecedence, false);

		if (fix == Fix::postfix)
			return std::pair<int, bool>(0, false);

		if (preDefined.find(name) != preDefined.end()) {
			for (auto& fnc : preDefined[name])
				if (fnc->fix == fix) return std::pair<int, bool>(fnc->precedence, fnc->rightAssociative);
		}
		return std::pair<int, bool>(defaultPrefixFunctionPrecedence * 2, false); // didnt found
	}

	const bool DataBase::isFunction(std::string name, Fix fix) {
		if (preDefined.find(name) != preDefined.end()) {
			for (auto& fnc : preDefined[name])
				if (fnc->fix == fix) return true;
		}
		if (fix == Fix::prefix && userDefined.find(name) != userDefined.end()) return true;
		return false;
	}

	const bool DataBase::isFunction(std::string name, int amountOfArgs) {
		if (amountOfArgs == -1) {
			if (preDefined.find(name) != preDefined.end()) return true;
			if (userDefined.find(name) != userDefined.end()) return true;
		}
		else {
			if (preDefined.find(name) != preDefined.end()) {
				for (auto& fnc : preDefined[name])
					if (fnc->amountOfArguments == amountOfArgs || fnc->amountOfArguments == -1) return true;
			}
			if (userDefined.find(name) != userDefined.end()) {
				for (auto& fnc : userDefined[name])
					if (fnc->amountOfArguments == amountOfArgs) return true;
			}
		}
		return false;
	}

	const bool DataBase::isPreDefinedFunction(std::string name, int amountOfArgs) {
		if (amountOfArgs == -1) {
			if (preDefined.find(name) != preDefined.end()) return true;
		}
		else {
			if (preDefined.find(name) != preDefined.end()) {
				for (auto& fnc : preDefined[name])
					if (fnc->amountOfArguments == amountOfArgs || fnc->amountOfArguments == -1) return true;
			}
		}
		return false;
	}

	const bool DataBase::isUserDefinedFunction(std::string name, int amountOfArgs) {
		if (amountOfArgs == -1) {
			if (userDefined.find(name) != userDefined.end()) return true;
		}
		else {
			if (userDefined.find(name) != userDefined.end()) {
				for (auto& fnc : userDefined[name])
					if (fnc->amountOfArguments == amountOfArgs) return true;
			}
		}
		return false;
	}

	const UserDefinedFunction* DataBase::findUserDefinedFunction(const std::string& name, int amountOfArgs)
	{
		auto it = userDefined.find(name);
		if (it != userDefined.end()) {
			for (auto& fnc : it->second)
				if (fnc->amountOfArguments == amountOfArgs) return fnc;
		}
		throw std::runtime_error(std::string("There is no function ") + name + " with " + std::to_string(amountOfArgs) + " arguments");
	}


	const rpnToken DataBase::makeRpnTokenFromFunction(std::string name, Fix fix, std::set<UserDefinedFunction*>& dependencies ,int amountOfArguments) {
		if (preDefined.find(name) != preDefined.end()) {
			if (fix == Fix::infix) {
				for (auto& fnc : preDefined[name]) {
					if (fnc->fix == Fix::infix) {
						if (fnc->amountOfArguments == 2)
							return rpnToken(fnc->header, 2);
					}
				}
			}
			if (fix == Fix::postfix) {
				for (auto& fnc : preDefined[name]) {
					if (fnc->fix == Fix::postfix) {
						return rpnToken(fnc->header, fnc->amountOfArguments); // there is currently no control over amount of args for postfix functions, so anything will do
					}
				}
			}
			if (fix == Fix::prefix) {
				for (auto& fnc : preDefined[name]) {
					if (fnc->fix == Fix::prefix) {
						if (fnc->amountOfArguments == amountOfArguments || amountOfArguments == -1) // -1 means any amount of argument, works only for predefined prefix functions
							return rpnToken(fnc->header, fnc->amountOfArguments);
					}
				}
			}
		}
		if (fix == Fix::prefix && userDefined.find(name) != userDefined.end()) {
			for (auto& fnc : userDefined[name])
				if (fnc->amountOfArguments == amountOfArguments) {
					dependencies.insert(fnc);
					return rpnToken(fnc->header, amountOfArguments);
				}
		}
		throw std::runtime_error("There is no function with  name = \"" + name + "\", fix = " + (fix == Fix::prefix ? "prefix" : (fix == Fix::infix ? "infix" : "postfix")) + ", and amount of arguments = " + std::to_string(amountOfArguments));
	}

	void DataBase::addUserDefinedFunction(std::string& name, std::string& expression, Node* header, int amountOfArguments, 
	const std::set<UserDefinedFunction*>& dependencies, const std::set<Variable*> variableDependencies) {

		auto& overloads = userDefined[name]; // if no name, just empty
		for (const auto& func : overloads) {
			if (func->amountOfArguments == amountOfArguments) {
				header->destroyFunction();
				throw std::runtime_error("Function " + name + " with " + std::to_string(amountOfArguments) + " args already exists");
			}
		}
																												 
		userDefined[name].push_back(new UserDefinedFunction(expression, header, amountOfArguments, dependencies, variableDependencies));

		UserDefinedFunction* justCreated = userDefined[name].back(); // function we've just created

		for (auto& dep : dependencies) // justCreated depends on them, so they should not be destroyed
			dep->dependants.insert(justCreated);

		for (auto& varDep : variableDependencies) // justCreated depends on them, so they should not be destroyed
			varDep->dependants.insert(justCreated);
	}

	
	void DataBase::changeUserDefinedFunction(std::string& name, std::string& newExpression, int amountOfArguments,
		const std::set<UserDefinedFunction*>& newDependencies, const std::set<Variable*> newVariableDependencies) {

		bool foundAndChanged = false;

		auto& overloads = userDefined[name]; // if no name, just empty
		for (const auto& func : overloads) {
			if (func->amountOfArguments == amountOfArguments) {
				foundAndChanged = true;

				for (auto& dep : func->dependencies)
					dep->dependants.erase(func);
				for (auto& varDep : func->variableDependencies)
					varDep->dependants.erase(func);

				func->dependencies = newDependencies;
				func->variableDependencies = newVariableDependencies;

				for (auto& dep : newDependencies)
					dep->dependants.insert(func);
				for (auto& varDep : newVariableDependencies)
					varDep->dependants.insert(func);

				func->expression = newExpression;
				break;
			}
		}

		if (!foundAndChanged)
			throw std::runtime_error("Function you want to change does not exist");
	}

	std::pair<std::string, int> DataBase::askForFunctionCredentials()
	{
		out << "Enter the function name (or type exit):" << std::endl;
		std::string name;
		in >> name;
		checkIfExit(name);

		out << "Enter amount of arguments (or type exit):" << std::endl;
		std::string numberStr;
		in >> numberStr;
		checkIfExit(numberStr);

		int amountOfArgs;
		try {
			amountOfArgs = std::stoi(numberStr);
		}
		catch (...) {
			throw std::runtime_error("Error: Invalid number format");
		}
		inputBufferClean();
		return std::pair<std::string, int>(name, amountOfArgs);
	}



	void DataBase::printDependants(const std::string& name, int amountOfArgs) {
		auto it = userDefined.find(name);
		if (it == userDefined.end()) throw std::runtime_error(std::string("There is no function with name ") + name);

		for (auto& itVec : it->second) {
			if (itVec->amountOfArguments = amountOfArgs) {
				if(itVec->dependants.size() == 0)
					out << makeTitle("Function " + itVec->expression + " has no dependants", consoleWidth, ':');
				else {
					const std::string separator(consoleWidth, '-');
					out << makeTitle("Here are all dependants of the function " + itVec->expression, consoleWidth, ':') << "\n" << separator << std::endl;

					for(auto& dep : itVec->dependants)
						out << ":::::::::: " << dep->expression << std::endl;

					out << separator << std::endl;
				}
				return; // since there can be only one function with this amount of args
			}
		}
		throw std::runtime_error(std::string("Function ") + name + " does not have an overload with " + std::to_string(amountOfArgs) + " arguments");
	}

	void DataBase::printDependancies(const std::string& name, int amountOfArgs) {
		auto it = userDefined.find(name);
		if (it == userDefined.end()) throw std::runtime_error(std::string("There is no function with name ") + name);

		for (auto& itVec : it->second) {
			if (itVec->amountOfArguments = amountOfArgs) {
				if (itVec->dependencies.size() == 0)
					out << makeTitle("Function " + itVec->expression + " has no dependencies", consoleWidth, ':');
				else {
					const std::string separator(consoleWidth, '-');
					out << makeTitle("Here are all dependencies of the function " + itVec->expression, consoleWidth, ':') << "\n" << separator << std::endl;

					for (auto& dep : itVec->dependencies)
						out << ":::::::::: " << dep->expression << std::endl;

					out << separator << std::endl;
				}
				return; // since there can be only one function with this amount of args
			}
		}
		throw std::runtime_error(std::string("Function ") + name + " does not have an overload with " + std::to_string(amountOfArgs) + " arguments");
	}

	void DataBase::printVariableDependants(const std::string& name) {
		auto it = variables.find(name);
		if (it == variables.end()) throw std::runtime_error(std::string("There is no variable with name ") + name);

		if (it->second->dependants.size() == 0)
			out << makeTitle("Variable " + it->first + " has no dependants", consoleWidth, ':');
		else {
			const std::string separator(consoleWidth, '-');
			out << makeTitle("Here are all dependants of the variable " + it->first, consoleWidth, ':') << "\n" << separator << std::endl;

			for (auto& dep : it->second->dependants)
				out << ":::::::::: " << dep->expression << std::endl;

			out << separator << std::endl;
		}
	}


	void DataBase::askToPrintDependancies() {
		std::pair<std::string, int> input = askForFunctionCredentials();

		try {
			// Calling the core logic
			printDependancies(input.first, input.second);
		}
		catch (const std::exception& e) {
			out << "Error: " << e.what() << std::endl;
		}
	}

	void DataBase::askToPrintDependants() {
		std::pair<std::string, int> input = askForFunctionCredentials();

		try {
			// Calling the core logic to print functions that depend on this one
			printDependants(input.first, input.second);
		}
		catch (const std::exception& e) {
			out << "Error: " << e.what() << std::endl;
		}
	}

	void DataBase::askToPrintVariableDependants() {
		out << "Enter the name of the variable (or type exit):" << std::endl << "> ";
		std::string name;
		in >> name;
		checkIfExit(name);

		try {
			// Calling the core logic to print functions that depend on this one
			printVariableDependants(name);
		}
		catch (const std::exception& e) {
			out << "Error: " << e.what() << std::endl;
		}
	}



	void DataBase::deleteUserDefinedFunction(const std::string& name, int amountOfArgs) {
		auto it = userDefined.find(name);
		if (it == userDefined.end())
			throw std::runtime_error("There is no function " + name);

		auto& overloads = it->second;
		bool found = false;

		for (auto itVec = overloads.begin(); itVec != overloads.end(); ) {

			if ((*itVec)->amountOfArguments == amountOfArgs) {
				found = true;

				if (!(*itVec)->dependants.empty()) {
					throw std::runtime_error("Can not delete function " + (*itVec)->expression +
						", since there are " + std::to_string((*itVec)->dependants.size()) + " other functions that depend on it");
				}

				//removing this function from dependants lists
				for (UserDefinedFunction* dep : (*itVec)->dependencies)
					if (dep) dep->dependants.erase(*itVec);

				for (Variable* varDep : (*itVec)->variableDependencies)
					if (varDep) varDep->dependants.erase(*itVec);

				delete* itVec;

				itVec = overloads.erase(itVec);
			}
			else {
				++itVec;
			}
		}

		if (!found)
			throw std::runtime_error("There is no function " + name + " with " + std::to_string(amountOfArgs) + " arguments");

		if (overloads.empty())
			userDefined.erase(it);
	}

	std::pair<int,int> DataBase::deleteAllPossibleOverloadsOfUserDefinedFunction(const std::string& name) {
		auto it = userDefined.find(name);
		if (it == userDefined.end()) {
			throw std::runtime_error("There is no function with name " + name);
		}

		auto& overloads = it->second;
		int startingSize = overloads.size();

		int deletedCount = 0;
		int deletedInThisCicle = 1;

		while (deletedInThisCicle > 0) {
			deletedInThisCicle = 0;
			for (auto itVec = overloads.begin(); itVec != overloads.end(); ) {

				// Check if there are any functions that depend on this specific overload
				if ((*itVec)->dependants.empty()) {

					//removing this function from dependants lists
					for (UserDefinedFunction* dep : (*itVec)->dependencies)
						if (dep) dep->dependants.erase(*itVec);

					for (Variable* varDep : (*itVec)->variableDependencies)
						if (varDep) varDep->dependants.erase(*itVec);

					delete *itVec;
					itVec = overloads.erase(itVec);
					deletedCount++;
					deletedInThisCicle++;
				}
				else {
					// Overload has dependants, so we skip it
					++itVec;
				}
			}
		} 

		if (overloads.empty())
			userDefined.erase(it);

		return std::make_pair(deletedCount, startingSize - deletedCount);
	}



	void DataBase::gatherDependantTree(UserDefinedFunction* toDelete, std::set<UserDefinedFunction*>& toDeleteSet) {
		if (toDeleteSet.find(toDelete) != toDeleteSet.end()) return; // have already been here

		for (auto& dep : toDelete->dependencies)
			dep->dependants.erase(toDelete);

		for (auto& varDep : toDelete->variableDependencies)
			varDep->dependants.erase(toDelete);

		while (!toDelete->dependants.empty())
			gatherDependantTree(*toDelete->dependants.begin(), toDeleteSet);

		toDeleteSet.insert(toDelete);
	}

	int DataBase::deleteDependantTree(const std::string& name, int amountOfArgs) {
		auto it = userDefined.find(name);
		if (it == userDefined.end())
			throw std::runtime_error("There is no function " + name);

		auto& overloads = it->second;

		UserDefinedFunction* cur = nullptr;
		for (auto itVec = overloads.begin(); itVec != overloads.end(); ++itVec ) {
			if ((*itVec)->amountOfArguments == amountOfArgs) {
				cur = *itVec;
				break;
			}
		}

		if (cur == nullptr)
			throw std::runtime_error("There is no function " + name + " with " + std::to_string(amountOfArgs) + " arguments");

		std::set<UserDefinedFunction*> toDeleteSet;
		gatherDependantTree(cur, toDeleteSet);

		for (auto it = userDefined.begin(); it != userDefined.end(); )
		{
			for (auto itVec = it->second.begin(); itVec != it->second.end();) {
				if (toDeleteSet.find(*itVec) != toDeleteSet.end())
					itVec = it->second.erase(itVec);
				else
					++itVec;
			}


			if (it->second.empty())
				it = userDefined.erase(it);
			else
				++it;
		}

		for (auto& f : toDeleteSet)
			delete f;	

		return toDeleteSet.size();
	}


	void DataBase::askToDelete() {
		out << "Enter function name (or type exit):" << std::endl << "> ";
		std::string name;
		in >> name;
		checkIfExit(name);

		out << "Now enter amount of arguments. Enter -1 if you want to delete all overloads (or type exit):" << std::endl << "> ";
		std::string numberStr;
		in >> numberStr;
		checkIfExit(numberStr);
		inputBufferClean();
		int amountOfArgs;
		try {
			amountOfArgs = std::stoi(numberStr);
		}
		catch (...) {
			out << "Error: Invalid number format" << std::endl;
			return;
		}

		if (amountOfArgs != -1) {
			try {
				deleteUserDefinedFunction(name, amountOfArgs);
				out << "Function " << name << " with " << amountOfArgs << " arguments was succesfully deleted" << std::endl;
			}
			catch (const std::exception& e) {
				out << "Error: " << e.what() << std::endl;
			}
		}
		else {
			std::pair<int,int> result = deleteAllPossibleOverloadsOfUserDefinedFunction(name);
			if (result.second == 0) {
				// Case 1: All overloads were successfully removed
				out << "All overloads of '" << name << "' were deleted." << std::endl;
			}
			else if (result.first > 0) {
				// Case 2: Some were deleted, but some remain
				out << "Deleted " << result.first << " overloads of the function " << name << ", "
					<< result.second << " remain because there are functions that depend on them." << std::endl;
			}
			else {
				// Case 3: Nothing was deleted
				out << "No overloads of the function " << name << "were deleted because there are functions that depend on them." << std::endl;
			}
		}
	}

	void DataBase::askToDeleteDependantTree() {
		std::pair<std::string, int> input = askForFunctionCredentials();

		try {
			// Calling the core logic
			out << deleteDependantTree(input.first, input.second) << " functions were deleted" << std::endl;
		}
		catch (const std::exception& e) {
			out << "Error: " << e.what() << std::endl;
		}
	}




	inline std::string DataBase::makeTitle(const std::string& name, int width, char fillingSymbol) {
		int size = static_cast<int>(name.size());

		if (size + 2 >= width) return name;

		int totalPadding = width - size - 2; 
		int leftPadding = totalPadding / 2;
		int rightPadding = totalPadding - leftPadding;

		return std::string(leftPadding, fillingSymbol) + " " + name + " " + std::string(rightPadding, fillingSymbol);
	}

	void DataBase::printUserDefinedFunctions() {
		if (userDefined.empty()) {
			out << makeTitle("There are no user-defined functions now", consoleWidth, ':');
			return;
		}
		const std::string separator(consoleWidth, '-');
		out << makeTitle("Here are all user-defined functions", consoleWidth, ':') << "\n" << separator << std::endl;
		for (const auto& fnc : userDefined) {
			if (fnc.second.empty()) continue;

			for (size_t i = 0; i < fnc.second.size(); ++i)
				out << ":::::::::: " << fnc.second[i]->expression << std::endl;
		}
		out << separator << std::endl;
	}

	void DataBase::printPreDefinedFunctions() {
		const std::string separator(consoleWidth, '-');
		out << makeTitle("Here are all system functions", consoleWidth, ':') << "\n" << separator << std::endl;
		for (const auto& fnc : preDefined) {
			if (fnc.second.empty()) continue;

			for (size_t i = 0; i < fnc.second.size(); ++i)
				out << ":::::::::: " << fnc.second[i]->expression << " :::::::::: " + fnc.second[i]->comment << std::endl;

		}
		out << separator << std::endl;
	}

	void DataBase::printVariables() {
		if (variables.empty()) {
			out << makeTitle("There are no defined variables now", consoleWidth, ':') << std::endl;
			return;
		}
		const std::string separator(consoleWidth, '-');
		out << makeTitle("Here are all variables", consoleWidth, ':') << "\n" << separator << std::endl;
		for (const auto& var : variables)
			out << var.first << " = " << std::to_string(var.second->header->evaluate()) << std::endl;
		out << separator << std::endl;
	}



	void DataBase::enterVariable() {
		out << "Enter the name of the new variable (or type exit):" << std::endl << "> ";
		std::string name;
		in >> name;
		checkIfExit(name);
		askForAVariable(name);
		out << "Variable '" << name << "' set successfully." << std::endl;

	}

	void DataBase::deleteVariable(const std::string& name) {
		auto it = variables.find(name);
		if (it != variables.end()) {
			if (!it->second->dependants.empty()) 
				throw std::runtime_error("There are functions that depend on the variable " + name);
			delete it->second;
			variables.erase(it);
			initDefaultVariables();
		}
		else
			throw std::runtime_error("Variable " + name + " not found");
	}

	void DataBase::askToDeleteVariable() {
		out << "Enter the name of the variable you want to delete (or type exit):" << std::endl << "> ";
		std::string name;
		in >> name;
		checkIfExit(name);
		inputBufferClean();
		try {
			deleteVariable(name);
			out << "Variable " << name << " was succesfully deleted" << std::endl;
		}
		catch (const std::exception& e) {
			out << "Error: " << e.what() << std::endl;
		}
	}

	void DataBase::clear() {
		variables.clear();
		userDefined.clear();
		initDefaultVariables();
		out << "All variables and user-defined functions have been cleared." << std::endl;
	}
}	