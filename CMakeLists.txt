cmake_minimum_required(VERSION 3.5)

# Set extension name here
set(TARGET_NAME large_flock)

set(EXTENSION_NAME ${TARGET_NAME}_extension)
set(LOADABLE_EXTENSION_NAME ${TARGET_NAME}_loadable_extension)

project(${TARGET_NAME})

# Include directories
include_directories(src/include)
add_subdirectory(src)

# Find dependencies
find_package(CURL REQUIRED)
find_package(inja REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Build the DuckDB static and loadable extensions
build_static_extension(${TARGET_NAME} ${EXTENSION_SOURCES})
build_loadable_extension(${TARGET_NAME} " " ${EXTENSION_SOURCES})

# Link libraries for the static extension
target_link_libraries(${EXTENSION_NAME} CURL::libcurl
                      nlohmann_json::nlohmann_json pantor::inja)

# Link libraries for the loadable extension
target_link_libraries(${LOADABLE_EXTENSION_NAME} CURL::libcurl
                      nlohmann_json::nlohmann_json pantor::inja)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/templates/lf_map_prompt_template.txt"
     DESTINATION "${CMAKE_BINARY_DIR}/extension/${TARGET_NAME}")

file(
  COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/templates/lf_generate_prompt_template.txt"
  DESTINATION "${CMAKE_BINARY_DIR}/extension/${TARGET_NAME}")

file(
  COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/templates/lf_filter_prompt_template.txt"
  DESTINATION "${CMAKE_BINARY_DIR}/extension/${TARGET_NAME}")

# Install the extension
install(
  TARGETS ${EXTENSION_NAME}
  EXPORT "${DUCKDB_EXPORT_SET}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")
