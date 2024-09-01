add_subdirectory(llm_response)
add_subdirectory(scalar)
add_subdirectory(query)


set(EXTENSION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/lf_query_parser.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/lf_tokenizer.cpp
    ${EXTENSION_SOURCES}
    PARENT_SCOPE)
