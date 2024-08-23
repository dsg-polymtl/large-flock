# `llm_map` Function

The `llm_map` function allows you to apply a template to a single column of data and generate responses using OpenAI's API. It is designed for dynamically transforming or analyzing data based on a specified template, with optional configuration for model selection, token limit, and temperature.

## Syntax

```sql
SELECT llm_map('template', 'template_text', 'input_name', column, ['model', 'model_name'], ['max_tokens', token_count], ['temperature', temp_value]) AS result FROM table;
```

## Parameters

- **template**: A string indicating the template keyword, followed by the actual template text. The template should contain placeholders in the format `{{placeholder}}`. The placeholder will be replaced with the value from the specified column.
- **input_name**: The placeholder in the template that will be replaced with values from the specified column.
- **column**: The column from the table whose values will replace the placeholder in the template.
- **model** (optional): The OpenAI model to use. Supported models include `gpt-4o`, `gpt-4o-mini`, `gpt-4-turbo`, `gpt-4`, and `gpt-3.5-turbo`. If not specified, a default model will be used.
- **max_tokens** (optional): The maximum number of tokens to generate in the completion. This value controls the length of the generated response. If not specified, a default value will be used.
- **temperature** (optional): A value between 0 and 1 that controls the randomness of the output. Lower values make the output more deterministic, while higher values introduce more randomness. If not specified, a default value will be used.

## Example Usage

**Example: Validating Email Addresses**

Given a table `students` with a column `email`, you can use `llm_map` to validate email addresses and provide feedback.

**SQL Queries**:

1. **Create and Insert Data**:

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

2. **Apply `llm_map`** with only required parameters:

    ```sql
    SELECT
        email,
        llm_map('template', 'is this a valid email? {{email}}', 'email', email) AS verification
    FROM
        students;
    ```

3. **Apply `llm_map`** with all parameters:

    ```sql
    SELECT
        email,
        llm_map('template', 'is this a valid email? {{email}}', 'email', email, 'model', 'gpt-4o-mini', 'max_tokens', 100, 'temperature', 0.7) AS verification
    FROM
        students;
    ```

**Output**:

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

## Notes

- **Ensure the `OPENAI_API_KEY` environment variable is set**. The `llm_map` function relies on this key to authenticate with the OpenAI API.
- The `llm_map` function requires only the `template`, `input_name`, and `column` parameters. The `model`, `max_tokens`, and `temperature` parameters are optional, with default values used if not provided.
- Supported models include `gpt-4o`, `gpt-4o-mini`, `gpt-4-turbo`, `gpt-4`, and `gpt-3.5-turbo`. If an unsupported model is specified, the function will raise an error.
- Adjust the `max_tokens` and `temperature` according to your needs to control the length and randomness of the output.