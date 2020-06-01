//
// Created by Vladimir Shubarin on 17.05.2020.
//

#ifndef SPO_COMPILER_SRC_SEMANTIC_ANALYZER_HPP_
#define SPO_COMPILER_SRC_SEMANTIC_ANALYZER_HPP_

#include "Node.hpp"
#include "Parser.h"
#include "variable_table.hpp"
#include "function_table.hpp"
class SemanticAnalyzer {
 private:
  Node *tree{};
  Parser *parser{};
  VariableTable *variables{};
  VariableTable *globalVariables{};
  FunctionTable *functions{};
 public:
  explicit SemanticAnalyzer(const std::string &_filename) {
	parser = new Parser(_filename);
	parser->parse();
	tree = parser->GetTree();
	variables = new VariableTable();
	globalVariables = new VariableTable();
	functions = new FunctionTable();
	analyze();
  }
 private:
  void analyze() {
	std::cout << "=======================\n SemanticAnalyzer \n===========" << std::endl;
	lookForVariableDeclaration();
	lookForFunctionsAndProcedures(tree);
	for (auto &function:functions->getFunctions()) {
	  checkTypeMismatch(function->getFuncAddr(), function);
	}

	std::cout << "Global variables:" << std::endl;
	globalVariables->printToConsole();
	std::cout << "------------------\nIn function variables:" << std::endl;
	functions->printToConsole();
	std::cout << "------------------\nLocal variables:" << std::endl;
	variables->printToConsole();
  }

  void lookForVariableDeclaration() {
	if (tree->op1->type == Node::nodeType::VAR_SECTION &&
		tree->op2->type == Node::nodeType::STATEMENT) {
	  declareVariables(tree->op1); // registering local variables
	  // register local variables do not expect functions/procedures
	} else if (tree->op1->list.front()->type == Node::nodeType::VAR_SECTION) {
	  declareVariables(tree->op1->list.front(), nullptr, true); //  registering global variables
	}
	if (tree->op1->list.back()->type == Node::nodeType::VAR_SECTION) {
	  declareVariables(tree->op1->list.back()); // registering local variables
	}
	checkVariableDeclaration(tree->op2);
	
  }

  void lookForFunctionsAndProcedures(Node *currentNode) {
	// если текущая нода ноль, то делать с ней ничего нельзя
	// так что выходим из функции
	if (currentNode == nullptr)
	  return;

	if (currentNode->type == Node::nodeType::PROCEDURE ||
		currentNode->type == Node::nodeType::FUNCTION) {
	  functions->addFunction(new Function(currentNode->value, currentNode));
	  if (currentNode->op1->type == Node::nodeType::PARAMS) {
		for (auto &param : currentNode->op1->list) {
		  if (param->type == Node::nodeType::VARLIST) {
			for (auto &var : param->list) {
			  functions->getFuncByName(currentNode->value)->
				  variables.addVar(new Variable(var->value, Variable::determineVarType(param->op1->value), true));
			}
		  } else if (param->type == Node::nodeType::PARAM) {
			for (auto &var:param->op2->list) {
			  functions->getFuncByName(currentNode->value)->
				  variables.addVar(new Variable(var->value, Variable::determineVarType(param->op2->op1->value)));
			}
		  }
		}
	  }
	  if (currentNode->op2->type == Node::nodeType::STATEMENT) {
		checkVariableDeclaration(currentNode, functions->getFuncByName(currentNode->value));
	  }
	  if (currentNode->op2->op1->type == Node::nodeType::VAR_SECTION) {
		declareVariables(currentNode->op2->op1, functions->getFuncByName(currentNode->value), false);
	  }
	}

	lookForFunctionsAndProcedures(currentNode->op1);
	lookForFunctionsAndProcedures(currentNode->op2);
	lookForFunctionsAndProcedures(currentNode->op3);
	lookForFunctionsAndProcedures(currentNode->op4);
	for (auto &node :currentNode->list) {
	  lookForFunctionsAndProcedures(node);
	}
  }

  void declareVariables(Node *currentNode, Function *parentFunction = nullptr, bool bGlobal = false) {
	// если текущая нода ноль, то делать с ней ничего нельзя
	// так что выходим из функции
	if (currentNode == nullptr)
	  return;

	if (currentNode->type == Node::nodeType::VARDECL) {
	  std::string varType = currentNode->op2->value;
	  for (auto &varName:currentNode->op1->list) {
		if (bGlobal) { // если глобальная переменная
		  globalVariables->addVar(new Variable(varName->value, Variable::determineVarType(varType)));
		} else if (parentFunction != nullptr) { // если переменная относится к функции
		  parentFunction->variables.addVar(new Variable(varName->value, Variable::determineVarType(varType)));
		  if (globalVariables->isVarDefined(parentFunction->getVariables().back()->getName())) {
			throw VariableShadowing(parentFunction->getVariables().back()->getName());
		  }
		} else { // если обычная переменная в программе
		  variables->addVar(new Variable(varName->value, Variable::determineVarType(varType)));
		  if (globalVariables->isVarDefined(variables->getVariables().back()->getName())) {
			throw VariableShadowing(variables->getVariables().back()->getName());
		  }
		}
	  }
	}

	declareVariables(currentNode->op1, parentFunction, bGlobal);
	declareVariables(currentNode->op2, parentFunction, bGlobal);
	declareVariables(currentNode->op3, parentFunction, bGlobal);
	declareVariables(currentNode->op4, parentFunction, bGlobal);
	for (auto &node :currentNode->list) {
	  declareVariables(node, parentFunction, bGlobal);
	}
  }


  void checkVariableDeclaration(Node *currentNode, Function *func = nullptr) {
	if (currentNode == nullptr)
	  return;

	if (currentNode->type == Node::nodeType::VAR) {
	  if (func != nullptr) {
		if (!func->variables.isVarDefined(currentNode->value) && !globalVariables->isVarDefined(currentNode->value)) {
		  throw VariableNotDefinedError(currentNode->value, func->getName());
		}
	  } else {
		if (!variables->isVarDefined(currentNode->value) && !globalVariables->isVarDefined(currentNode->value)) {
		  throw VariableNotDefinedError(currentNode->value);
		}
	  }
	}
	checkVariableDeclaration(currentNode->op1, func);
	checkVariableDeclaration(currentNode->op2, func);
	checkVariableDeclaration(currentNode->op3, func);
	checkVariableDeclaration(currentNode->op4, func);
	for (auto &node: currentNode->list) {
	  checkVariableDeclaration(node, func);
	}
  }

  void checkTypeMismatch(Node *currentNode, Function *func = nullptr) {
	if (currentNode == nullptr)return;

	if (currentNode->type == Node::nodeType::BINOP && currentNode->op2->type != Node::nodeType::BINOP) {
	  if (func != nullptr) {
		if (!Variable::areTypesCompatible(func->variables.getVarType(currentNode->op1->value),
										  func->variables.getVarType(currentNode->op2->value))) {
		  throw TypeMismatchError(Variable::varTypeToString(func->variables.getVarType(currentNode->op1->value)),
								  Variable::varTypeToString(func->variables.getVarType(currentNode->op2->value)));
		}
	  }
	}
	checkTypeMismatch(currentNode->op1, func);
	checkTypeMismatch(currentNode->op2, func);
	checkTypeMismatch(currentNode->op3, func);
	checkTypeMismatch(currentNode->op4, func);
	for (auto &node: currentNode->list) {
	  checkTypeMismatch(node, func);
	}
  }
};
#endif //SPO_COMPILER_SRC_SEMANTIC_ANALYZER_HPP_
