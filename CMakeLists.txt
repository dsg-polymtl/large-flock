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
find_package(
  Python3
  COMPONENTS Development
  REQUIRED)

# Build the DuckDB static and loadable extensions
build_static_extension(${TARGET_NAME} ${EXTENSION_SOURCES})
build_loadable_extension(${TARGET_NAME} " " ${EXTENSION_SOURCES})

# Link libraries for the static extension
target_link_libraries(
  ${EXTENSION_NAME} CURL::libcurl nlohmann_json::nlohmann_json pantor::inja
  ${Python3_LIBRARIES})

# Link libraries for the loadable extension
target_link_libraries(
  ${LOADABLE_EXTENSION_NAME} CURL::libcurl nlohmann_json::nlohmann_json
  pantor::inja ${Python3_LIBRARIES})

# Include Python headers in both static and loadable extensions
target_include_directories(${EXTENSION_NAME} PRIVATE ${Python3_INCLUDE_DIRS})
target_include_directories(${LOADABLE_EXTENSION_NAME}
                           PRIVATE ${Python3_INCLUDE_DIRS})

file(
  COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/core/functions/scalar/get_num_tokens.py"
  DESTINATION "${CMAKE_BINARY_DIR}/extension/${TARGET_NAME}")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/templates/lf_map/prompt_template.txt"
     DESTINATION "${CMAKE_BINARY_DIR}/extension/${TARGET_NAME}")

# Install the extension
install(
  TARGETS ${EXTENSION_NAME}
  EXPORT "${DUCKDB_EXPORT_SET}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")
