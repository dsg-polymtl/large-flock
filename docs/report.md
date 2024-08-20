# DuckDB LLM Extension: Implementation and User Guide

## Table of Contents

- [DuckDB LLM Extension: Implementation and User Guide](#duckdb-llm-extension-implementation-and-user-guide)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Overview of the LLM Extension](#overview-of-the-llm-extension)
  - [Setting Up the LLM Environment](#setting-up-the-llm-environment)
    - [Installing `litellm`](#installing-litellm)
    - [Running the `litellm` Server](#running-the-litellm-server)
    - [Configuring the LLM Models](#configuring-the-llm-models)
  - [Key Functions of the LLM Extension](#key-functions-of-the-llm-extension)
    - [`set_llm_config` Function](#set_llm_config-function)
    - [`get_llm_config` Function](#get_llm_config-function)
    - [`llm` Function](#llm-function)
    - [`llm_map` Function](#llm_map-function)
      - [Batch Processing](#batch-processing)
    - [`llm_summarize` Function](#llm_summarize-function)
  - [Supported LLMs](#supported-llms)
  - [Building](#building)
    - [Managing Dependencies with VCPKG](#managing-dependencies-with-vcpkg)
    - [Build Process](#build-process)
  - [Running and Testing the Extension](#running-and-testing-the-extension)
    - [Starting DuckDB with the LLM Extension](#starting-duckdb-with-the-llm-extension)
    - [Running Tests](#running-tests)
  - [Installing the Extension from Deployed Binaries](#installing-the-extension-from-deployed-binaries)
  - [Conclusion](#conclusion)

---

## Introduction

The DuckDB LLM Extension integrates the power of large language models (LLMs) into the DuckDB environment, enabling seamless interaction between SQL queries and AI-driven natural language processing. This extension allows users to configure LLM settings, query models directly, and leverage AI capabilities for tasks like summarization and data validation—all within DuckDB.

## Overview of the LLM Extension

The LLM Extension for DuckDB introduces a series of scalar functions that provide powerful tools for integrating AI-driven language models into your data processing pipelines. These functions allow users to set configurations, interact with models using natural language prompts, and even map data fields to templates processed by the LLM.

Key functionalities include:
- **Configuration of LLM settings** such as model choice, context window, and temperature.
- **Interaction with the LLM** via direct SQL queries.
- **Mapping and summarizing data** using AI-driven analysis.

This document provides a detailed guide on setting up the environment, utilizing the extension's functions, and customizing the LLM integration according to your needs.

## Setting Up the LLM Environment

### Installing `litellm`

To interface with the LLM extension, you need to install and set up `litellm`, a lightweight LLM proxy server.

- **Install `litellm`:**

  Install the `litellm` package using pip:

  ```bash
  pip install litellm
  ```

- **Export the OpenAI API Key:**

  Ensure your environment has access to the OpenAI API by exporting your API key:

  ```bash
  export OPENAI_API_KEY=YOUR_OPENAI_API_KEY
  ```

### Running the `litellm` Server

The `litellm` server acts as the intermediary between DuckDB and the LLM. It processes requests from DuckDB and forwards them to the LLM, then returns the generated responses.

- **Run the Server:**

  Start the `litellm` server with the appropriate configuration:

  ```bash
  litellm --config litellm_proxy.config
  ```

- **Configuring Different Models:**

  You can customize the model used by editing the `litellm_proxy.config` file. The default configuration uses `gpt-3.5-turbo-instruct`, but other models supported by OpenAI can be configured as needed. Refer to the [litellm documentation](https://docs.litellm.ai/docs/simple_proxy) for more details.

### Configuring the LLM Models

Once the server is running, you can configure the LLM within DuckDB using the `set_llm_config` function. This allows you to specify the model, context window size, and temperature for generating responses.

```sql
SELECT set_llm_config('default_model', 'gpt-3.5-turbo-instruct', 'context_window', '4195', 'temperature', '0.75') AS setup;
```

Successful configuration is confirmed with the message:

```
┌─────────────────────────────┐
│            setup            │
│           varchar           │
├─────────────────────────────┤
│ Config successfully updated │
└─────────────────────────────┘
```

## Key Functions of the LLM Extension

### `set_llm_config` Function

This function configures the LLM settings, allowing you to tailor the behavior of the model to your specific needs. Parameters include the model name, context window size, and temperature.

- **Model Name:** Select the LLM model initialized via the `litellm` config.
- **Context Window:** Define the maximum input size the model can handle.
- **Temperature:** Adjust the randomness of the model’s output.

Example usage:

```sql
SELECT set_llm_config('default_model', 'gpt-4', 'context_window', '8000', 'temperature', '0.7') AS setup;
```

### `get_llm_config` Function

This function retrieves the current LLM configuration, including the model name, context window, and temperature settings. It is useful for verifying the active configuration before executing LLM-related queries.

Example usage:

```sql
SELECT get_llm_config() AS config;
```

Example output:

```
┌──────────────────────────────────────────────────────────────────────────────────────┐
│                                        config                                        │
│                                       varchar                                        │
├──────────────────────────────────────────────────────────────────────────────────────┤
│ Default Model: gpt-4 | Context Window: 8000 | Temperature: 0.700000                  │
└──────────────────────────────────────────────────────────────────────────────────────┘
```

### `llm` Function

The `llm` function allows you to interact directly with the LLM by passing a prompt and receiving a response. This is ideal for generating text, answering questions, or performing tasks based on natural language queries.

Example usage:

```sql
SELECT llm('What is the capital of France?') AS response;
```

Example output:

```
┌───────────────────────────────────────────┐
│                 response                  │
│                 varchar                   │
├───────────────────────────────────────────┤
│ The capital of France is Paris.           │
└───────────────────────────────────────────┘
```

### `llm_map` Function

The `llm_map` function applies LLM-driven analysis to columns in a database, processing them through a template. It is designed to handle complex data processing tasks where each row of data can be processed using an AI model based on a template provided by the user.

- **Template Engine:** The function uses the `inja` template engine, which is a powerful C++ templating library inspired by `jinja2`. This allows for dynamic generation of queries where placeholders within the template are replaced with actual data from the database.

Example usage:

```sql
SELECT
    email, llm_map('template', 'Is this a valid email? {{email}}', 'email', email) AS validation
FROM
    students;
```

#### Batch Processing

Given the fixed context window limitations of LLMs, especially when dealing with large datasets, the `llm_map` function includes a batch processing feature. This feature divides the data into smaller chunks, processes each chunk separately, and then combines the results.

**Key Aspects of Batch Processing:**

- **Chunk Size Management:** The function dynamically adjusts the size of the batches based on the model's context window. This ensures that the LLM can process the data without exceeding its input limits.
  
- **Efficiency:** By processing data in batches, the extension can handle large datasets more efficiently, avoiding potential issues such as timeouts or memory overflows.

- **Parallel Processing (Future Consideration):** Although not currently implemented, future versions of the extension may include parallel processing of batches to further enhance performance.

### `llm_summarize` Function

The `llm_summarize` function is designed to condense large text blocks or datasets into concise summaries, leveraging the LLM’s natural language understanding capabilities.

Example usage:

```sql
SELECT llm_summarize('text', text) AS summary FROM documents;
```

Example output:

```
┌───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                  summary                                                                                  │
│                                                                                  varchar                                                                                  │
├───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│ AI is the intelligence exhibited by machines, which is different from the natural intelligence shown by humans and animals.                                               │
│ Machine learning is a type of AI that involves creating systems that can learn from data, improve with experience, and perform tasks without being explicitly programmed. │
│ NLP is a branch of AI that focuses on enabling computers to understand, interpret, and manipulate human language. It helps bridge the gap between human language and ma…  │
└───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

## Supported LLMs

The DuckDB LLM Extension supports a wide array of models provided by OpenAI and other providers. For a comprehensive list of supported models, refer to the [litellm documentation](https://docs.litellm.ai/docs/providers).

## Building

 the DuckDB LLM Extension

### Managing Dependencies with VCPKG

The DuckDB LLM Extension relies on several C++ libraries, including `inja`, `nlohmann_json`, and `httplib`, among others. Managing these dependencies is simplified using VCPKG, a C++ package manager.

- **Install Dependencies:**

  Use the following commands to install the necessary packages:

  ```bash
  vcpkg install inja nlohmann_json httplib
  ```

- **Configure the Build System:**

  Ensure that CMake is set up to use VCPKG for dependency management by adding:

  ```bash
  set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
  ```

### Build Process

Once the dependencies are installed, you can proceed with building the extension.

- **Build the Extension:**

  Execute the following commands from the project root:

  ```bash
  mkdir build && cd build
  cmake ..
  make
  ```

- **Validate the Build:**

  Ensure the build was successful by checking for the presence of the extension binary in the build directory.

## Running and Testing the Extension

### Starting DuckDB with the LLM Extension

To start DuckDB with the LLM extension, include the path to the extension in the startup command:

```bash
./duckdb --load_extension=llm_extension.so
```

### Running Tests

Tests are included to verify the functionality of the LLM extension. To run the tests, execute:

```bash
cd build
ctest
```

Ensure that all tests pass successfully before deploying the extension.

## Installing the Extension from Deployed Binaries

For users who prefer not to build the extension from source, pre-built binaries are available for installation. These binaries can be installed using the following command:

```bash
duckdb --install_extension=llm_extension
```

## Conclusion

The DuckDB LLM extension provides powerful tools for integrating LLMs into your data processing pipelines. With it, you can easily configure, query, and leverage AI capabilities directly within DuckDB.