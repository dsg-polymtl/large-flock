#include "large_flock/core/parser/lf_query_parser.hpp"

#include "large_flock/common.hpp"

#include <sstream>
#include <stdexcept>

namespace large_flock {

namespace core {
std::string LfQueryParser::ParseQuery(const std::string &query) {
    Tokenizer tokenizer(query);

    Token token = tokenizer.NextToken();
    std::string value = StringUtil::Upper(token.value);
    if (token.type != TokenType::KEYWORD ||
        (value != "CREATE" && value != "DELETE" && value != "UPDATE" && value != "GET")) {
        throw std::runtime_error("Unknown keyword: " + token.value);
    }

    token = tokenizer.NextToken();
    value = StringUtil::Upper(token.value);
    if (token.type == TokenType::KEYWORD && value == "MODEL") {
        LfModelParser model_parser;
        model_parser.Parse(query, statement);
        return model_parser.ToSQL(*statement);
    } else if (token.type == TokenType::KEYWORD && value == "PROMPT") {
        LfPromptParser prompt_parser;
        prompt_parser.Parse(query, statement);
        return prompt_parser.ToSQL(*statement);
    } else {
        throw std::runtime_error("Unknown keyword: " + token.value);
    }
}

} // namespace core

} // namespace large_flock