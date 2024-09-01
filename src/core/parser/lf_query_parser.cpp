#include "large_flock/core/parser/lf_query_parser.hpp"

#include <sstream>
#include <stdexcept>

void LfQueryParser::ParseQuery(const std::string &query) {
    Tokenizer tokenizer(query);
    ParseCreateCommand(tokenizer);
}

void LfQueryParser::ParseCreateCommand(Tokenizer &tokenizer) {
    Token token = tokenizer.NextToken();
    if (token.type != TokenType::KEYWORD || token.value != "CREATE") {
        throw std::runtime_error("Expected 'CREATE' keyword.");
    }

    token = tokenizer.NextToken();
    if (token.type == TokenType::KEYWORD && token.value == "DUCK") {
        LfCreateDuckParser duck_parser;
        duck_parser.Parse(tokenizer, statements);
    } else if (token.type == TokenType::KEYWORD && token.value == "MODEL") {
        LfCreateModelParser model_parser;
        model_parser.Parse(tokenizer, statements);
    } else if (token.type == TokenType::KEYWORD && token.value == "PROMPT") {
        LfCreatePromptParser prompt_parser;
        prompt_parser.Parse(tokenizer, statements);
    } else {
        throw std::runtime_error("Unknown command after 'CREATE'.");
    }
}

std::string LfQueryParser::TranslateToSQL(const std::string &query) {
    // Clear previous statements
    statements.clear();

    // Parse the input query
    ParseQuery(query);

    // Generate SQL for each parsed statement
    std::ostringstream sql_queries;
    for (const auto &stmt : statements) {
        if (const auto *duck_stmt = dynamic_cast<const CreateDuckStatement *>(stmt.get())) {
            sql_queries << "SELECT '" << duck_stmt->duck_string << "' AS duck_says;\n";
        } else if (const auto *model_stmt = dynamic_cast<const CreateModelStatement *>(stmt.get())) {
            sql_queries << "INSERT INTO models (model_name, max_tokens) VALUES ('" << model_stmt->model_name << "', "
                        << model_stmt->max_tokens << ");\n";
        } else if (const auto *prompt_stmt = dynamic_cast<const CreatePromptStatement *>(stmt.get())) {
            sql_queries << "INSERT INTO prompts (prompt_name, task) VALUES ('" << prompt_stmt->prompt_name << "', '"
                        << prompt_stmt->task << "');\n";
        } else {
            throw std::runtime_error("Unknown statement type for SQL translation.");
        }
    }

    return sql_queries.str();
}
