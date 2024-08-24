cmake_minimum_required(VERSION 3.5)

# Set extension name here
set(TARGET_NAME large_flock)
set(CMAKE_CXX_STANDARD 17)

set(EXTENSION_NAME ${TARGET_NAME}_extension)
set(LOADABLE_EXTENSION_NAME ${TARGET_NAME}_loadable_extension)

project(${TARGET_NAME})

# Include directories
include_directories(src/include)
add_subdirectory(src)

# Build the DuckDB static and loadable extensions
build_static_extension(${TARGET_NAME} ${EXTENSION_SOURCES})
build_loadable_extension(${TARGET_NAME} " " ${EXTENSION_SOURCES})

# Install the extension
install(
  TARGETS ${EXTENSION_NAME}
  EXPORT "${DUCKDB_EXPORT_SET}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")

# DuckDB's extension distribution supports vcpkg. As such, dependencies can be added in ./vcpkg.json and then
# used in cmake with find_package. Feel free to remove or replace with other dependencies.
# Note that it should also be removed from vcpkg.json to prevent needlessly installing it..
find_package(CURL REQUIRED)
find_package(inja REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(${EXTENSION_NAME} CURL::libcurl)

target_link_libraries(${LOADABLE_EXTENSION_NAME} nlohmann_json::nlohmann_json)

target_link_libraries(${EXTENSION_NAME} pantor::inja)
target_link_libraries(${LOADABLE_EXTENSION_NAME} pantor::inja)
