#include "utils/template_utils.h"
#include <duckdb.hpp>

namespace duckdb
{
    std::string GenerateCombinedPrompt(const inja::json &context)
    {
        inja::Environment env;
        inja::Template prompt_template = env.parse_template("src/templates/template_llm_map.txt");

        return env.render(prompt_template, context);
    }

    // TODO: change replace placeholders with another one that uses INJA instead
    inja::json CreatePromptContext(KeyValueMap &data_map, size_t start_index, size_t end_index)
    {
        inja::json context;
        context["prompts"] = inja::json::array();

        auto &template_vector = data_map["template"];

        for (size_t i = start_index; i < end_index; ++i)
        {
            inja::json prompt_context;
            std::string template_str = template_vector->GetValue(i).ToString();
            inja::json params;

            for (const auto &pair : data_map)
            {
                const std::string &key = pair.first;
                const auto &value_ptr = pair.second;

                if (key != "template")
                {
                    params[key] = value_ptr->GetValue(i).ToString();
                }
            }

            inja::Environment env;

            prompt_context["number"] = i + 1;
            prompt_context["prompt"] = env.render(template_str, params);
            context["prompts"].push_back(prompt_context);
        }

        return context;
    }
}
