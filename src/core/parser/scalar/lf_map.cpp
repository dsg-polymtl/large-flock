#include <large_flock/common.hpp>
#include <large_flock/core/functions/scalar.hpp>
#include <large_flock/core/parser/scalar.hpp>
#include <large_flock_extension.hpp>

namespace large_flock {
namespace core {

std::vector<nlohmann::json> CoreScalarParsers::Struct2Json(Vector &struct_vector, int size) {
    vector<nlohmann::json> vector_json;
    for (auto i = 0; i < size; i++) {
        nlohmann::json json;
        for (auto j = 0; j < StructType::GetChildCount(struct_vector.GetType()); j++) {
            auto setting_key = StructType::GetChildName(struct_vector.GetType(), j);
            auto setting_value = StructValue::GetChildren(struct_vector.GetValue(i))[j].ToString();
            json[setting_key] = setting_value;
        }
        vector_json.push_back(json);
    }
    return vector_json;
}

void CoreScalarParsers::LfMapScalarParser(DataChunk &args) {
    if (args.ColumnCount() < 3 || args.ColumnCount() > 4) {
        throw std::runtime_error("LfMapScalarParser: Invalid number of arguments.");
    }

    // check if template and model are strings
    if (args.data[0].GetType() != LogicalType::VARCHAR) {
        throw std::runtime_error("LfMapScalarParser: Template must be a string.");
    }
    if (args.data[1].GetType() != LogicalType::VARCHAR) {
        throw std::runtime_error("LfMapScalarParser: Model must be a string.");
    }
    if (args.data[2].GetType().id() != LogicalTypeId::STRUCT) {
        throw std::runtime_error("LfMapScalarParser: Inputs must be a struct.");
    }
    if (args.ColumnCount() == 4) {
        if (args.data[3].GetType().id() != LogicalTypeId::STRUCT) {
            throw std::runtime_error("LfMapScalarParser: Settings value must be a struct.");
        }
    }
}

} // namespace core
} // namespace large_flock