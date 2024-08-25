# `lf_map` Function Documentation

The `lf_map` is a mapping function from a row in a relation to a new set of attributes that are appended to the row. It does so using an LLM call.

## Syntax

```sql
SELECT lf_map('template_text', 'model_name', 'input_name_1', column_value_1, 'input_name_2', column_value_2, ..., 'input_name_n', column_value_n, ['lf:max_tokens', token_count], ['lf:temperature', temp_value]) AS result 
FROM table;
```

### Parameters

- **template_text** (`VARCHAR`): 
  - A string containing the text template. 
  - Placeholders in the template are defined using the format `{{placeholder}}`, which will be replaced by actual data from the specified columns.

- **model_name** (`VARCHAR`): 
  - The name of the LLM to be used.
  - Supported models include various LLMs like `gpt-4`, `gpt-4-mini`, `gpt-4-turbo`, `gpt-3.5-turbo`, or any other integrated model.

- **input_name_1, input_name_2, ..., input_name_n** (`VARCHAR`): 
  - The names of the placeholders within the template that correspond to the column values.
  
- **column_value_1, column_value_2, ..., column_value_n** (`VARCHAR`): 
  - The columns in the table whose values will replace the specified placeholders in the template.
  
- **lf:max_tokens** (`INTEGER`, optional): 
  - The maximum number of tokens allowed in the response. 
  - If not specified, a default token limit set by the model is used.
  
- **lf:temperature** (`FLOAT`, optional): 
  - Controls the randomness of the output, ranging from `0` (deterministic output) to `1` (maximum randomness).
  - If not specified, the default value set by the model is used.

## Example Usage

### Example 1: Validating Email Addresses

This example demonstrates how to validate email addresses using the `lf_map` function with an LLM.

**Table Schema:**

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

**SQL Query:**

```sql
SELECT email, lf_map('is this a valid email? {{email}}', 'gpt-4', 'email', email) AS verification 
FROM students;
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

### Example 2: Summarizing Product Descriptions

This example shows how to generate summaries for product descriptions in an e-commerce database using an LLM.

**Table Schema:**

```sql
CREATE TABLE products (
    product_id INTEGER PRIMARY KEY,
    name VARCHAR,
    description TEXT
);

INSERT INTO products (product_id, name, description) VALUES
    (1, 'Smartphone', 'A smartphone with a 6.5-inch display, 128GB storage, and a 48MP camera.'),
    (2, 'Laptop', 'A powerful laptop with an Intel i7 processor, 16GB RAM, and 512GB SSD.'),
    (3, 'Wireless Earbuds', 'Compact wireless earbuds with noise cancellation and 24-hour battery life.');
```

**SQL Query:**

```sql
SELECT name, lf_map('Please summarize the following product description: {{description}}', 'gpt-4', 'description', description, 'lf:max_tokens', 50) AS summary 
FROM products;
```

**Output:**

```
┌─────────────────────┬───────────────────────────────────────────────────────────────┐
│        name         │                          summary                               │
│       varchar       │                          varchar                               │
├─────────────────────┼───────────────────────────────────────────────────────────────┤
│ Smartphone          │ A smartphone with a large display, ample storage, and a high-  │
│                     │ quality camera.                                                │
│ Laptop              │ A powerful laptop featuring a fast processor, sufficient RAM,  │
│                     │ and ample storage.                                             │
│ Wireless Earbuds    │ Compact earbuds with noise cancellation and long battery life. │
└─────────────────────┴───────────────────────────────────────────────────────────────┘
```

### Example 3: Generating Personalized Recommendations

This example demonstrates how to generate personalized movie recommendations based on user preferences using an LLM.

**Table Schema:**

```sql
CREATE TABLE users (
    user_id INTEGER PRIMARY KEY,
    name VARCHAR,
    favorite_genre VARCHAR
);

INSERT INTO users (user_id, name, favorite_genre) VALUES
    (1, 'Alice', 'Science Fiction'),
    (2, 'Bob', 'Action'),
    (3, 'Charlie', 'Romance');
```

**SQL Query:**

```sql
SELECT name, lf_map('Recommend a movie in the {{favorite_genre}} genre.', 'gpt-4', 'favorite_genre', favorite_genre) AS recommendation 
FROM users;
```

**Output:**

```
┌─────────┬─────────────────────────────────────────────┐
│  name   │               recommendation                │
│ varchar │                 varchar                     │
├─────────┼─────────────────────────────────────────────┤
│ Alice   │ I recommend "Interstellar" for Science      │
│         │ Fiction enthusiasts.                        │
│ Bob     │ You might enjoy "Mad Max: Fury Road," a     │
│         │ thrilling Action movie.                     │
│ Charlie │ "The Notebook" is a classic choice for      │
│         │ Romance lovers.                             │
└─────────┴─────────────────────────────────────────────┘
```

### Example 4: Classifying Customer Feedback

This example shows how to classify customer feedback into categories such as positive, negative, or neutral using an LLM.

**Table Schema:**

```sql
CREATE TABLE feedback (
    feedback_id INTEGER PRIMARY KEY,
    customer_name VARCHAR,
    feedback_text TEXT
);

INSERT INTO feedback (feedback_id, customer_name, feedback_text) VALUES
    (1, 'David', 'The service was excellent, and the food was delicious!'),
    (2, 'Emma', 'The delivery was late, and the product was damaged.'),
    (3, 'Liam', 'The experience was okay, but nothing special.');
```

**SQL Query:**

```sql
SELECT customer_name, lf_map('Classify the following feedback as positive, negative, or neutral: {{feedback_text}}', 'gpt-4', 'feedback_text', feedback_text) AS classification 
FROM feedback;
```

**Output:**

```
┌───────────────┬────────────────┐
│ customer_name │ classification │
│    varchar    │    varchar     │
├───────────────┼────────────────┤
│ David         │ Positive       │
│ Emma          │ Negative       │
│ Liam          │ Neutral        │
└───────────────┴────────────────┘
```

## Notes

- The `lf_map` function supports integration with multiple LLMs, making it highly versatile for different tasks and contexts.
- The flexibility of `lf_map` allows it to be adapted for various use cases, including but not limited to validation, summarization, classification, and personalized recommendations.
- Ensure that the necessary API keys or configurations are properly set up for the LLM being used.