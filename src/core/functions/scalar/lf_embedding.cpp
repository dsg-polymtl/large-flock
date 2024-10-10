#include <large_flock/common.hpp>
#include <large_flock/core/functions/scalar.hpp>
#include <large_flock/core/model_manager/model_manager.hpp>
#include <large_flock/core/parser/llm_response.hpp>
#include <large_flock/core/parser/scalar.hpp>
#include <large_flock_extension.hpp>

namespace large_flock {
namespace core {

static void LfEmbeddingScalarFunction(DataChunk &args, ExpressionState &state, Vector &result) {
    Connection con(*state.GetContext().db);
    CoreScalarParsers::LfEmbeddingScalarParser(args);

    auto model = args.data[1].GetValue(0).ToString();
    auto query_result = con.Query(
        "SELECT model, max_tokens FROM lf_config.LARGE_FLOCK_MODEL_INTERNAL_TABLE WHERE model_name = '" + model + "'");

    if (query_result->RowCount() == 0) {
        throw std::runtime_error("Model not found");
    }

    auto model_name = query_result->GetValue(0, 0).ToString();
    auto inputs = CoreScalarParsers::Struct2Json(args.data[0], args.size());

    auto embeddings = nlohmann::json::array();
    for (auto &input : inputs) {
        nlohmann::json input_embedding;
        for (auto &item : input.items()) {
            auto element_embedding = ModelManager::CallEmbedding(item.value(), model_name);
            input_embedding[item.key()] = element_embedding;
        }
        embeddings.push_back(input_embedding);
    }

    auto index = 0;
    Vector vec(LogicalType::VARCHAR, args.size());
    UnaryExecutor::Execute<string_t, string_t>(vec, result, args.size(), [&](string_t _) {
        return StringVector::AddString(result, embeddings[index++].dump());
    });
}

void CoreScalarFunctions::RegisterLfEmbeddingScalarFunction(DatabaseInstance &db) {
    ExtensionUtil::RegisterFunction(db, ScalarFunction("lf_embedding", {}, LogicalType::VARCHAR, LfEmbeddingScalarFunction, nullptr,
                                                       nullptr, nullptr, nullptr, LogicalType::ANY));
}

} // namespace core
} // namespace large_flock
