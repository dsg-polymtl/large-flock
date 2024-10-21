# FlockMTL Extension

This repository provides a DuckDB extension that integrates language model (LLM) capabilities directly into your queries and workflows. This experimental extension enables DuckDB users to add semantic analysis (classification, filtering, completion, all w/ structured output) and embeddings using GPT models—all from within SQL commands. Following the tradition of declarativity, we introduce an administrative view of `MODEL`(s) and `PROMPT`(s) akin to `TABLE`(s).

---

### Installation

Install the extension as a [Community Extension](https://community-extensions.duckdb.org/).

Run:
  ```sql
  INSTALL flockmtl FROM community;
  LOAD flockmtl;
  ```

---

### Documentation

Next, we demonstrate how to use the **DuckDB LLM Extension** and an example we would like to analyze product reviews. Specifically, we’ll focus on generating text, classifying reviews as positive or negative, and working with text embeddings using a `product_reviews` table with attributes `review_text`, `review_id`, and `customer_name`.

#### **1. Text Generation with `llm_complete`**

`llm_complete` generates text based on a given prompt and LLM model. You can customize the output with parameters like `max_tokens` or `temperature`.

##### **Examples**:
- **Basic Text Generation**:  
  Generate a simple greeting message from a predefined prompt.
  ```sql
  SELECT llm_complete('hello-world', 'default') AS salutation;
  ```

- **Sentiment Classification – Positive Review**:  
  Use a predefined `is_positive` prompt to classify whether a review is positive.
  ```sql
  SELECT llm_complete('is_positive', 'default', {'text': review_text}, {'max_tokens': 100}) AS results 
  FROM product_reviews;
  ```

- **Sentiment Classification – Negative Review**:  
  Similarly, classify whether a review is negative using the `is_negative` prompt.
  ```sql
  SELECT llm_complete('is_negative', 'default', {'text': review_text}, {'max_tokens': 100}) AS results 
  FROM product_reviews;
  ```

- **Without Additional Parameters**:  
  Use the default model and settings to check if a review is positive without specifying extra parameters.
  ```sql
  SELECT llm_complete('is_positive', 'default', {'text': review_text}) AS results 
  FROM product_reviews;
  ```

#### **2. JSON Output with `llm_complete_json`**

`llm_complete_json` is used to return structured JSON output. This is useful when you need more detailed or structured responses from the LLM.

##### **Examples**:
- **Classify Review and Return JSON**:  
  Classify whether a review is positive or negative and return the result in JSON format.
  ```sql
  SELECT review_id, llm_complete_json('is_positive', 'default', {'text': review_text}, {'max_tokens': 100}) AS results 
  FROM product_reviews;
  ```

#### **3. Filtering with `llm_filter`**

`llm_filter` can be used to filter reviews based on LLM-based classification, such as identifying whether a review is positive or negative.

##### **Examples**:
- **Filter Positive Reviews**:  
  Filter product reviews and only return those classified as positive.
  ```sql
  SELECT review_id, customer_name, review_text 
  FROM product_reviews 
  WHERE llm_filter('is_positive', 'default', {'text': review_text});
  ```

- **Filter Negative Reviews**:  
  Similarly, filter product reviews to return only negative reviews.
  ```sql
  SELECT review_id, customer_name, review_text 
  FROM product_reviews 
  WHERE llm_filter('is_negative', 'default', {'text': review_text});
  ```

#### **4. Text Embedding with `llm_embedding`**

`llm_embedding` generates vector embeddings for text, which can be used for tasks like semantic similarity, clustering, or advanced search.

##### **Example**:
- **Generate Embeddings for Review Text**:  
  Generate embeddings for each review in the `product_reviews` table, which could be used later for similarity searches or clustering.
  ```sql
  SELECT review_id, llm_embedding({'review_text': review_text}, 'text-embedding-3-small') AS embeddings 
  FROM product_reviews;
  ```

---

### **Prompt and Model Management**

You can manage LLM prompts and models dynamically in DuckDB using the following commands:

#### **Prompt Management**:
- **Get All Prompts**:  
  ```sql
  GET PROMPTS;
  ```

- **Get Specific Prompt**:  
  Retrieve the content of a specific prompt by name.
  ```sql
  GET PROMPT <prompt_name>;
  ```

- **Create a New Prompt**:  
  ```sql
  CREATE PROMPT(<prompt_name>, <prompt_text>);
  ```

- **Update an Existing Prompt**:  
  ```sql
  UPDATE PROMPT(<prompt_name>, <new_prompt_text>);
  ```

- **Delete a Prompt**:  
  ```sql
  DELETE PROMPT <prompt_name>;
  ```

#### **Model Management**:
- **Get All Models**:  
  ```sql
  GET MODELS;
  ```

- **Get Specific Model**:  
  Retrieve the details of a specific model by name.
  ```sql
  GET MODEL <model_name>;
  ```

- **Create a New Model**:  
  ```sql
  CREATE MODEL(<model_name>, <model_type>, <max_tokens>);
  ```

- **Update a Model**:  
  ```sql
  UPDATE MODEL(<model_name>, <model_type>, <max_tokens>);
  ```

- **Delete a Model**:  
  ```sql
  DELETE MODEL <model_name>;
  ```