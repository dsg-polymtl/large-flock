#include "large_flock/core/parser/query/lf_create_duck_parser.hpp"

#include <stdexcept>

void LfCreateDuckParser::Parse(Tokenizer &tokenizer, std::vector<std::unique_ptr<QueryStatement>> &statements) {
    Token token = tokenizer.NextToken();
    if (token.type != TokenType::STRING_LITERAL || token.value.empty()) {
        throw std::runtime_error("Expected non-empty string literal after 'DUCK'.");
    }
    std::string duck_string = token.value;

    token = tokenizer.NextToken();
    if (token.type == TokenType::SYMBOL && token.value == ";") {
        auto statement = std::make_unique<CreateDuckStatement>();
        statement->duck_string = duck_string;
        statements.push_back(std::move(statement));
    } else {
        throw std::runtime_error("Unexpected characters after the string. Only a semicolon is allowed.");
    }
}
