cmake_minimum_required(VERSION 3.5)

# Set extension name here
set(TARGET_NAME llm)
set(CMAKE_CXX_STANDARD 17)


# DuckDB's extension distribution supports vcpkg. As such, dependencies can be added in ./vcpkg.json and then
# used in cmake with find_package. Feel free to remove or replace with other dependencies.
# Note that it should also be removed from vcpkg.json to prevent needlessly installing it..

set(EXTENSION_NAME ${TARGET_NAME}_extension)
set(LOADABLE_EXTENSION_NAME ${TARGET_NAME}_loadable_extension)

project(${TARGET_NAME})

# Include directories
include_directories(src/include)

# Define the source files
set(EXTENSION_SOURCES
    src/llm_extension.cpp
    src/utils/response_utils.cpp
    src/utils/template_utils.cpp
    src/utils/validation_utils.cpp
    src/utils/litellm.cpp
)
# Build the DuckDB static and loadable extensions
build_static_extension(${TARGET_NAME} ${EXTENSION_SOURCES})
build_loadable_extension(${TARGET_NAME} " " ${EXTENSION_SOURCES})

# Install the extension
install(
    TARGETS ${EXTENSION_NAME}
    EXPORT "${DUCKDB_EXPORT_SET}"
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
    )
    
# Find and include CURL
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(${EXTENSION_NAME} ${CURL_LIBRARIES})

# Optionally include INJA if needed
# find_package(inja REQUIRED)
# target_link_libraries(${EXTENSION_NAME} pantor::inja)