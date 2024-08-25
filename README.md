# Large Flock Extension

This repository provides a DuckDB extension for integrating LLM usage, allowing you to use a scalar function that processes prompts and returns responses from an LLM models. It is based on the [DuckDB extension template](https://github.com/duckdb/extension-template).

---

## Table of Contents

- [Large Flock Extension](#large-flock-extension)
  - [Table of Contents](#table-of-contents)
  - [Getting Started](#getting-started)
    - [Clone the Repository](#clone-the-repository)
    - [Managing Dependencies](#managing-dependencies)
    - [Build the Extension](#build-the-extension)
  - [Running the Extension](#running-the-extension)
    - [Set Up Environment Variables](#set-up-environment-variables)
    - [Start DuckDB](#start-duckdb)
    - [Use the Extension](#use-the-extension)
      - [Syntax](#syntax)
      - [Parameters](#parameters)
      - [Example Usage](#example-usage)
  - [Running the Tests](#running-the-tests)
  - [Installing the Deployed Binaries](#installing-the-deployed-binaries)

---

## Getting Started

### Clone the Repository

Clone the repository with the following command:

```bash
git clone --recurse-submodules https://github.com/dsg-polymtl/large-flock-extension.git
```

> **Note:** The `--recurse-submodules` flag ensures that DuckDB is included, which is necessary for building the extension.

### Managing Dependencies

DuckDB extensions use VCPKG for dependency management. Set it up by running:

```bash
sh scripts/setup_vcpkg.sh
```

### Build the Extension

To build the extension, use one of the following methods:

1. **Using `make`:**

   ```bash
   make
   ```

2. **If `make` is not available, you can use the provided script:**

   ```bash
   sh scripts/build_project.sh
   ```

These commands generates the following binaries:

- `./build/release/duckdb`: The DuckDB shell with the extension code preloaded.
- `./build/release/test/unittest`: The test runner for DuckDB, with the extension linked.
- `./build/release/extension/llm/llm.duckdb_extension`: The loadable binary for distribution.

---

## Running the Extension

### Set Up Environment Variables

Configure your OpenAI API keys by updating the `env.sh` script and running:

```bash
sh scripts/setup_env.sh
```

### Start DuckDB

Launch the DuckDB shell with the extension using:

```bash
./build/release/duckdb
```

### Use the Extension

The core feature of this extension is the `lf_map` scalar function, which is a mapping function from a row in a relation to a new set of attributes that are appended to the row. It does so using an LLM call.

#### Syntax

```sql
SELECT lf_map('template_text', 'model_name', 'input_name', column, ['lf:max_tokens', token_count], ['lf:temperature', temp_value]) AS result FROM table;
```

#### Parameters

- **template_text**: A string with the template text, including placeholders in the format `{
  { placeholder }}`.
- **model_name**: The OpenAI model to use (e.g., `gpt-4`, `gpt-3.5-turbo`).
- **input_name**: The placeholder in the template replaced by values from the specified column.
- **column**: The column containing values to replace the placeholder.
- **lf:max_tokens** (optional): Maximum number of tokens for the response.
- **lf:temperature** (optional): Controls output randomness (0 to 1).

#### Example Usage

**Validating Email Addresses**

Given a `students` table:

```sql
CREATE TABLE students (
    student_id INTEGER PRIMARY KEY,
    first_name VARCHAR,
    last_name VARCHAR,
    email VARCHAR,
    enrollment_date DATE
);

INSERT INTO students (student_id, first_name, last_name, email, enrollment_date) VALUES
    (1, 'John', 'Doe', 'john.doe@example.com', '2022-09-01'),
    (2, 'Jane', 'Smith', 'jane.smithexample.com', '2022-09-01'),
    (3, 'Emily', 'Jones', 'emily.jones@example.com', '2023-01-15');
```

Apply `lf_map`:

```sql
SELECT email, lf_map('is this a valid email? {{email}}', 'gpt-4', 'email', email) AS verification FROM students;
```

With all parameters:

```sql
SELECT email, lf_map('is this a valid email? {{email}}', 'gpt-4o-mini', 'email', email, 'lf:max_tokens', 100, 'lf:temperature', 0.7) AS verification FROM students;
```

**Output:**

```
┌─────────────────────────┬───────────────────────────────────────────────────────────────┐
│          email          │                          verification                         │
│         varchar         │                            varchar                            │
├─────────────────────────┼───────────────────────────────────────────────────────────────┤
│ john.doe@example.com    │ Yes, this appears to be a valid email address.                │
│ jane.smithexample.com   │ No, this is not a correct email.                              │
│ emily.jones@example.com │ Yes, this appears to be a valid email address.                │
└─────────────────────────┴───────────────────────────────────────────────────────────────┘
```

**Notes:**

- The `lf_map` function requires the `template`, `model_name`, `input_name`, and `column` parameters.
- Optional parameters include `lf:max_tokens` and `lf:temperature`. Supported models are `gpt-4`, `gpt-4-mini`, `gpt-4-turbo`, `gpt-4`, and `gpt-3.5-turbo`.

---

## Running the Tests

To run SQL tests for the DuckDB extension, use:

```bash
make test
```

---

## Installing the Deployed Binaries

To install extension binaries from S3:

1. **Enable Unsigned Extensions:**

   Launch DuckDB with the `allow_unsigned_extensions` option:

   - CLI: `duckdb -unsigned`
   - Python: `con = duckdb.connect(':memory:', config={'allow_unsigned_extensions': 'true'})`
   - NodeJS: `db = new duckdb.Database(':memory:', {"allow_unsigned_extensions": "true"});`

2. **Set Repository Endpoint:**

   Set the repository endpoint in DuckDB to your bucket's HTTP URL:

   ```sql
   SET custom_extension_repository='bucket.s3.eu-west-1.amazonaws.com/llm_extension/latest';
   ```

   Use the `/latest` path for the latest version, or replace `latest` with a specific version.

3. **Install and Load the Extension:**

   ```sql
   INSTALL llm;
   LOAD llm;
   ```
