#pragma once

#include "duckdb.hpp"
namespace duckdb
{

	class LlmExtension : public Extension
	{
	public:
		void Load(DuckDB &db) override;
		std::string Name() override;
		std::string Version() const override;

		static void SetDefaultModel(const std::string &model_name);
		static void SetContextWindow(int context_window);
		static void SetTemperature(double temperature);

		static std::string GetDefaultModel();
		static int GetContextWindow();
		static double GetTemperature();

	private:
		static std::string default_model;
		static int context_window;
		static double temperature;
	};

} // namespace duckdb
