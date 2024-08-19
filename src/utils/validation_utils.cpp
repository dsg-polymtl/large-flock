#include "utils/validation_utils.h"

namespace duckdb
{
    void InputsValidator(DataChunk &args)
    {
        if (args.ColumnCount() < 2)
        {
            throw InvalidInputException("llm_map requires at least two arguments: the template and at least one data column.");
        }

        // The first argument should be the prompt template (string)
        if (args.data[0].GetType() != LogicalType::VARCHAR)
        {
            throw InvalidInputException("The first argument must be a prompt template.");
        }
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
    size_t CalculateTotalTokens(DataChunk &args)
    {
        // 1. Calculate tokens for the template
        size_t total_tokens = CountWords(args.data[0].GetValue(0).ToString());

        // 2. Add tokens for each input based on its type
        for (size_t i = 1; i < args.ColumnCount(); ++i)
        {
            total_tokens += EstimateTokensForInput(args.data[i].GetType());
        }

        return total_tokens;
    }

    // TODO: Improve the chank size calculation instead of this naive one
    size_t DetermineChunkSize(DataChunk &args)
    {
        int context_size = 4191;

        int num_tokens = CalculateTotalTokens(args);
        int chunk_size = context_size / num_tokens;
        return chunk_size;
    }

}