add_subdirectory(core)

set(EXTENSION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/large_flock_extension.cpp ${EXTENSION_SOURCES}
    PARENT_SCOPE)
