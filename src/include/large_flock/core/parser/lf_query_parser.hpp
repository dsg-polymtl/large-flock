#pragma once

#include "large_flock/core/parser/lf_tokenizer.hpp"
#include "large_flock/core/parser/query/lf_create_duck_parser.hpp"
#include "large_flock/core/parser/query/lf_create_model_parser.hpp"
#include "large_flock/core/parser/query/lf_create_prompt_parser.hpp"
#include "query_statements.hpp"

#include <memory>
#include <string>
#include <vector>

class LfQueryParser {
public:
    void ParseQuery(const std::string &query);
    std::string TranslateToSQL(const std::string &query);

private:
    std::vector<std::unique_ptr<QueryStatement>> statements;

    void ParseCreateCommand(Tokenizer &tokenizer);
};
