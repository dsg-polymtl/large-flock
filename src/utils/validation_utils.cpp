#include "utils/validation_utils.h"
#include "llm_extension.hpp"
namespace duckdb
{
    void InputsValidator(DataChunk &args)
    {
        if (args.ColumnCount() < 4)
        {
            throw InvalidInputException("llm_map requires at least two arguments: the template and at least one data column.");
        }

        // The first argument should be the prompt template (string)
        if (args.data[0].GetType() != LogicalType::VARCHAR || args.data[1].GetType() != LogicalType::VARCHAR)
        {
            throw InvalidInputException("The first arguments must be related to the prompt template.");
        }
    }

    // Function to parse arguments and return a key-value map
    KeyValueMap ParseKeyValuePairs(duckdb::DataChunk &args)
    {
        // Ensure that the number of columns is even (keys and values)
        if (args.ColumnCount() % 2 != 0) throw InvalidInputException("You have a missing key or record");

        // Create a map to store key-value pairs
        KeyValueMap data_map;

        bool no_template = true;

        // Iterate over the columns in pairs (key, value)
        for (idx_t i = 0; i < args.ColumnCount() / 2; i++)
        {
            // Extract the key from the current column
            std::string key = args.data[i * 2].GetValue(0).ToString();

            if (key == "template") {
                no_template = false;
            }

            // Extract the value vector from the next column
            // Note: Creating a new unique_ptr for the vector
            std::unique_ptr<duckdb::Vector> value_vector = std::make_unique<duckdb::Vector>(args.data[i * 2 + 1]);

            // Store the key-value pair in the map
            data_map[key] = std::move(value_vector);
        }

        // if (no_template) throw InvalidInputException("No template provided, ensure that you inserted the right name.");

        return data_map;
    }

    size_t CountWords(const std::string &str)
    {
        std::istringstream stream(str);
        return std::distance(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>());
    }

    size_t EstimateTokensForInput(LogicalType type)
    {
        switch (type.id())
        {
        case LogicalTypeId::VARCHAR:
            return 5; // Assuming average of 5 tokens per VARCHAR input
        case LogicalTypeId::DATE:
            return 2; // Assuming 2 tokens for DATE
        // Add other types as needed
        default:
            return 1; // Fallback, treat as 1 token
        }
    }

    // TODO: strong method to calculate the number of tokens
    size_t CalculateTotalTokens(KeyValueMap &data_map)
    {
        // 1. Calculate tokens for the template
        size_t total_tokens = CountWords(data_map["template"]->GetValue(0).ToString());

        // 2. Add tokens for each input based on its type
        for (const auto &pair : data_map)
        {
            const auto &value_ptr = pair.second;

            total_tokens += EstimateTokensForInput(value_ptr->GetType());
        }

        return total_tokens;
    }

    // TODO: Improve the chank size calculation instead of this naive one
    size_t DetermineChunkSize(KeyValueMap &data_map)
    {
        int context_size = LlmExtension::GetContextWindow();

        int num_tokens = CalculateTotalTokens(data_map);
        int chunk_size = context_size / num_tokens;
        return chunk_size;
    }

}