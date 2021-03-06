//
// Created by Vladimir Shubarin on 17.05.2020.
//

#ifndef SPO_COMPILER_SRC_VARIABLE_TABLE_HPP_
#define SPO_COMPILER_SRC_VARIABLE_TABLE_HPP_

#include <vector>
#include "variable.hpp"

class VariableTable {
 public:
  ~VariableTable() { variables.clear(); }

  /**
   * @brief позволяет добавить переменую
   * @param var
   */
  void addVar(Variable *var) {
	if (!isVarDefined(var->getName()))
	  variables.push_back(var);
	else
	  throw NotImplementedException("var redef");
  }

  /**
   * @brief возвращает тип переменной
   * @param _val
   * @return
   */
  Variable::varType getVarType(const std::string &_val) {
	// since what semanticAnalyser passes here might not be a varName we need to check and determine type of what was passed
	if (Token::determineTokenType(_val) != Token::tokenType::Id) {
	  return Variable::determineVarType(Token::typeToString(Token::determineTokenType(_val)));
	}
	return getVarByName(_val)->getType();
  }

  /**
   * @brief возвращает переменную по имени
   */
  Variable *getVarByName(const std::string &_name) {
	for (auto &var : variables) {
	  if (var->getName() == _name) {
		return var;
	  }
	}
	throw VariableNotDefinedError(_name);
  }

  /**
   * @brief проверяет определеная ли переменная
   * @param _name
   * @return
   */
  bool isVarDefined(const std::string &_name) {
	for (auto &var : variables) {
	  if (var->getName() == _name) {
		return true;
	  }
	}
	return false;
  }

  /**
   * @brief выводит список переменных в консоль
   */
  void printToConsole() {
	for (auto &var : variables) {
	  if (var->getIsParam())
		std::cout << "Param: ";
	  std::cout << var->getName() << ": " << Variable::varTypeToString(var->getType()) << std::endl;
	}
  }
 private:
  std::vector<Variable *> variables;
 public:
  [[nodiscard]] const std::vector<Variable *> &getVariables() const {
	return variables;
  }
};

#endif //SPO_COMPILER_SRC_VARIABLE_TABLE_HPP_
