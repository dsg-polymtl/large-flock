#pragma once

#include <memory>
#include <string>
#include <vector>

// Base class for different query statements
class QueryStatement {
public:
    virtual ~QueryStatement() = default;
};

// Statement for 'CREATE DUCK' command
class CreateDuckStatement : public QueryStatement {
public:
    std::string duck_string;
};

// Statement for 'CREATE MODEL' command
class CreateModelStatement : public QueryStatement {
public:
    std::string model_name;
    int max_tokens;
};

// Statement for 'CREATE PROMPT' command
class CreatePromptStatement : public QueryStatement {
public:
    std::string prompt_name;
    std::string task;
};
