#include "large_flock/core/parser/query/lf_create_prompt_parser.hpp"

#include <stdexcept>

void LfCreatePromptParser::Parse(Tokenizer &tokenizer, std::vector<std::unique_ptr<QueryStatement>> &statements) {
    Token token = tokenizer.NextToken();
    if (token.type != TokenType::PARENTHESIS || token.value != "(") {
        throw std::runtime_error("Expected opening parenthesis '(' after 'PROMPT'.");
    }

    token = tokenizer.NextToken();
    if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
        throw std::runtime_error("Expected non-empty string literal for prompt name.");
    }
    std::string prompt_name = token.value;

    token = tokenizer.NextToken();
    if (token.type != TokenType::SYMBOL || token.value != ",") {
        throw std::runtime_error("Expected comma ',' after prompt name.");
    }

    token = tokenizer.NextToken();
    if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
        throw std::runtime_error("Expected non-empty string literal for task.");
    }
    std::string task = token.value;

    token = tokenizer.NextToken();
    if (token.type != TokenType::PARENTHESIS || token.value != ")") {
        throw std::runtime_error("Expected closing parenthesis ')' after task.");
    }

    token = tokenizer.NextToken();
    if (token.type == TokenType::END_OF_FILE) {
        auto statement = std::make_unique<CreatePromptStatement>();
        statement->prompt_name = prompt_name;
        statement->task = task;
        statements.push_back(std::move(statement));
    } else {
        throw std::runtime_error("Unexpected characters after the closing "
                                 "parenthesis. Only a semicolon is allowed.");
    }
}
