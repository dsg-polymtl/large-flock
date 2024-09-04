#pragma once

#include "large_flock/common.hpp"
#include "large_flock/core/parser/lf_tokenizer.hpp"
#include "large_flock/core/parser/query_statements.hpp"

#include <memory>
#include <string>
#include <vector>

namespace large_flock {

namespace core {

// Specific statements
class CreateModelStatement : public QueryStatement {
public:
    CreateModelStatement() {
        type = StatementType::CREATE_MODEL;
    }
    std::string model_name;
    std::string model;
    int max_tokens;
};

class DeleteModelStatement : public QueryStatement {
public:
    DeleteModelStatement() {
        type = StatementType::DELETE_MODEL;
    }
    std::string model_name;
};

class UpdateModelStatement : public QueryStatement {
public:
    UpdateModelStatement() {
        type = StatementType::UPDATE_MODEL;
    }
    std::string model_name;
    std::string model;
    int new_max_tokens;
};

class GetModelStatement : public QueryStatement {
public:
    GetModelStatement() {
        type = StatementType::GET_MODEL;
    }
    std::string model_name;
};

class GetAllModelStatement : public QueryStatement {
public:
    GetAllModelStatement() {
        type = StatementType::GET_ALL_MODEL;
    }
};

class LfModelParser {
public:
    void Parse(const std::string &query, std::unique_ptr<QueryStatement> &statement);
    std::string ToSQL(const QueryStatement &statement) const;

private:
    void ParseCreateModel(Tokenizer &tokenizer, std::unique_ptr<QueryStatement> &statement);
    void ParseDeleteModel(Tokenizer &tokenizer, std::unique_ptr<QueryStatement> &statement);
    void ParseUpdateModel(Tokenizer &tokenizer, std::unique_ptr<QueryStatement> &statement);
    void ParseGetModel(Tokenizer &tokenizer, std::unique_ptr<QueryStatement> &statement);
};
} // namespace core

} // namespace large_flock