#pragma once
// #define exprtk_enable_debugging

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "exprtk.hpp"

using SymbolTableT		   = exprtk::symbol_table<double>;
using ExpressionT		   = exprtk::expression<double>;
using ParserT			   = exprtk::parser<double>;
using CompositorT		   = exprtk::function_compositor<double>;
using ErrorT			   = exprtk::parser_error::type;
using VariableMapT		   = std::unordered_map<std::string, double>;
using ExpressionContainerT = std::unordered_map<uint64_t, ExpressionT>;

class MathParser {
public:
	MathParser();

	virtual ~MathParser() = default;

protected:
	void AddVariable(const std::string& name, double value);

	bool ParseExpression(uint64_t key, const std::string& expression, bool OneShot);

	double GetOutput(const ExpressionT& Expression) const;

	template <typename Function>
	void AddFunction(std::string name, Function function) {
		this->SymbolTable.add_function(name, function);
	}

	static void ParserError(const ParserT& parser, const std::string& expression);

	void setToken(double token_);

	ExpressionContainerT ExpressionContainer;
	ParserT				 Parser;
	SymbolTableT		 SymbolTable;
	VariableMapT		 VariableMap;

	bool   _parserON = false;
	double _token	 = 0;

	std::mutex _lock;
};
