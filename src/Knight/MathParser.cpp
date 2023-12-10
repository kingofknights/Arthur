#include "MathParser.hpp"

#include <Lancelot/Logger/Logger.hpp>

MathParser::MathParser() {
    this->SymbolTable.add_variable("token_", _token);
}

void MathParser::ParserError(const ParserT& parser, const std::string& expression) {
    LOG(WARNING, "Error : [{}] Expression : [{}]", parser.error(), expression)

    for (std::size_t i = 0; i < parser.error_count(); ++i) {
        ErrorT error = parser.get_error(i);
        LOG(WARNING, "\tError :[{}], Position [{}] Type [{}] Message [{}] Expression [{}]", i, error.token.position, exprtk::parser_error::to_str(error.mode), error.diagnostic, expression)
    }
}

void MathParser::AddVariable(const std::string& name, double value) {
    auto iterator = this->VariableMap.find(name);
    if (iterator == VariableMap.end()) {
        auto success = this->VariableMap.insert(VariableMapT::value_type(name, value));
        SymbolTable.add_variable(name, success.first->second);
    } else {
        SymbolTable.get_variable(name)->ref() = value;
    }
}

bool MathParser::ParseExpression(uint64_t key, const std::string& expression, bool OneShot) {
    _parserON = false;
    bool return_(false);
    if (_lock.try_lock()) {
        if (!OneShot) {
            ExpressionT Expression = Parser.compile(expression, SymbolTable);
            auto        pair       = ExpressionContainer.emplace(ExpressionContainerT::value_type(key, Expression));
            LOG(INFO, "ParseExpression : {} inserted in expression {}", key, pair.second)
            return_ = pair.second;
        } else {
            ExpressionT Expression = Parser.compile(expression, SymbolTable);
            // TODO: One Event
            /*
            auto tokenContainer = depthMap;
            for (const auto& item : tokenContainer) {
                    Expression.get_symbol_table().get_variable("token")->ref() = item.first;
                    Expression.get_symbol_table().get_variable("Token")->ref() = item.first;
                    Expression.value();
            }*/
        }
        _lock.unlock();
    }
    _parserON = true;
    return return_;
}

double MathParser::GetOutput(const ExpressionT& Expression) const {
    return _parserON ? Expression.value() : 0;
}
void MathParser::setToken(double token_) {
    _token = token_;
}
