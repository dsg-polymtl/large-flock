#include <large_flock/common.hpp>
#include <large_flock/core/functions/scalar.hpp>
#include <large_flock/core/parser/scalar.hpp>
#include <large_flock_extension.hpp>

namespace large_flock
{

namespace core
{

nlohmann::json CoreScalarParsers::LfMapScalarParser(DataChunk &args)
{

    // check if the args size is at least 4: template, model, input_key,
    // input_value
    if (args.ColumnCount() < 4)
    {
        throw std::runtime_error("LfMapScalarParser: At least 4 arguments are required: template, "
                                 "model, input_key, input_value.");
    }

    // get the inputs from the args
    nlohmann::json args_map;
    std::unordered_map<std::string, int> optional_keys = {{"max_tokens", -1}, {"temperature", -1}};

    // check if template and model are strings
    if (args.data[0].GetType() != LogicalType::VARCHAR)
    {
        throw std::runtime_error("LfMapScalarParser: Template must be a string.");
    }
    if (args.data[1].GetType() != LogicalType::VARCHAR)
    {
        throw std::runtime_error("LfMapScalarParser: Model must be a string.");
    }

    // store the template and the model name
    args_map["template"] = 0;
    args_map["model"] = 1;

    // parse the other args that would follow the key value pairs if key starts
    // with "lf:" you should add it in the first level else add it in the inputs
    for (idx_t i = 2; i < args.ColumnCount(); i += 2)
    {
        const auto &key_type = args.data[i].GetType();
        if (key_type != LogicalType::VARCHAR)
        {
            throw std::runtime_error("LfMapScalarParser: Key must be a string.");
        }
        std::string key = args.data[i].GetValue(0).ToString();
        if (key.find("lf:") == 0)
        {
            args_map[key.substr(3)] = i + 1;
        }
        else
        {
            args_map["inputs"][key] = i + 1;
        }
    }

    // check if the args_map doesn't contains the temperature and max_tokens
    for (const auto &el : optional_keys)
    {
        if (args_map.find(el.first) == args_map.end())
        {
            args_map[el.first] = el.second;
        }
    }

    return args_map;
}

} // namespace core

} // namespace large_flock