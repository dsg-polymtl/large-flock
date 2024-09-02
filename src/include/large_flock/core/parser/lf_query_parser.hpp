#pragma once

#include "large_flock/common.hpp"
#include "large_flock/core/parser/lf_tokenizer.hpp"
#include "large_flock/core/parser/query/lf_model_parser.hpp"
#include "large_flock/core/parser/query/lf_prompt_parser.hpp"
#include "large_flock/core/parser/query_statements.hpp"

#include <memory>
#include <string>
#include <vector>

namespace large_flock {
namespace core {
class LfQueryParser {
public:
    std::string ParseQuery(const std::string &query);

private:
    std::unique_ptr<QueryStatement> statement;
};
} // namespace core
} // namespace large_flock
