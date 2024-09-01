#include "large_flock/core/parser/query/lf_create_model_parser.hpp"

#include <stdexcept>

void LfCreateModelParser::Parse(Tokenizer &tokenizer, std::vector<std::unique_ptr<QueryStatement>> &statements) {
    Token token = tokenizer.NextToken();
    if (token.type != TokenType::PARENTHESIS || token.value != "(") {
        throw std::runtime_error("Expected opening parenthesis '(' after 'MODEL'.");
    }

    token = tokenizer.NextToken();
    if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
        throw std::runtime_error("Expected non-empty string literal for model name.");
    }
    std::string model_name = token.value;

    token = tokenizer.NextToken();
    if (token.type != TokenType::SYMBOL || token.value != ",") {
        throw std::runtime_error("Expected comma ',' after model name.");
    }

    token = tokenizer.NextToken();
    if (token.type != TokenType::NUMBER || token.value.empty()) {
        throw std::runtime_error("Expected integer value for max_tokens.");
    }
    int max_tokens = std::stoi(token.value);

    token = tokenizer.NextToken();
    if (token.type != TokenType::PARENTHESIS || token.value != ")") {
        throw std::runtime_error("Expected closing parenthesis ')' after max_tokens.");
    }

    token = tokenizer.NextToken();
    if (token.type == TokenType::END_OF_FILE) {
        auto statement = std::make_unique<CreateModelStatement>();
        statement->model_name = model_name;
        statement->max_tokens = max_tokens;
        statements.push_back(std::move(statement));
    } else {
        throw std::runtime_error("Unexpected characters after the closing "
                                 "parenthesis. Only a semicolon is allowed.");
    }
}
