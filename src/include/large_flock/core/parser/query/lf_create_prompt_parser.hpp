#pragma once

#include "large_flock/core/parser/lf_tokenizer.hpp"
#include "large_flock/core/parser/query_statements.hpp"

#include <memory>
#include <vector>

class LfCreatePromptParser {
public:
    void Parse(Tokenizer &tokenizer, std::vector<std::unique_ptr<QueryStatement>> &statements);
};
