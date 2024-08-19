#include "utils/template_utils.h"
#include <duckdb.hpp>

namespace duckdb
{
    std::string replace_placeholders(const std::string &template_str, const std::vector<std::string> &args)
    {
        std::stringstream ss;
        size_t pos = 0, arg_idx = 0;

        while (pos < template_str.size() && arg_idx < args.size())
        {
            size_t start = template_str.find("{{", pos);
            if (start == std::string::npos)
            {
                ss << template_str.substr(pos);
                break;
            }
            size_t end = template_str.find("}}", start);
            if (end == std::string::npos)
            {
                ss << template_str.substr(pos);
                break;
            }

            ss << template_str.substr(pos, start - pos) << args[arg_idx++];
            pos = end + 2;
        }
        ss << template_str.substr(pos);
        return ss.str();
    }

    std::string GenerateCombinedPrompt(const inja::json &context)
    {
        inja::Environment env;
        inja::Template prompt_template = env.parse_template("src/templates/template_llm_map.txt");

        return env.render(prompt_template, context);
    }

    inja::json CreatePromptContext(DataChunk &args, size_t start_index, size_t end_index, const std::vector<std::reference_wrapper<Vector>> &args_vector)
    {
        inja::json context;
        context["prompts"] = inja::json::array();

        auto &template_vector = args.data[0];

        for (size_t i = start_index; i < end_index; ++i)
        {
            inja::json prompt_context;
            std::string template_str = template_vector.GetValue(i).ToString();
            std::vector<std::string> params;

            for (auto &arg_ref : args_vector)
            {
                Vector &arg = arg_ref.get();
                auto value = arg.GetValue(i).ToString();
                params.push_back(value);
            }

            prompt_context["number"] = i + 1;
            prompt_context["prompt"] = replace_placeholders(template_str, params);
            context["prompts"].push_back(prompt_context);
        }

        return context;
    }
}
