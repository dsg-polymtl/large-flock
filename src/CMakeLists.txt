add_subdirectory(core)

set(EXTENSION_SOURCES
<<<<<<< HEAD
    ${CMAKE_CURRENT_SOURCE_DIR}/large_flock_extension.cpp ${EXTENSION_SOURCES}
    PARENT_SCOPE)
=======
    ${CMAKE_CURRENT_SOURCE_DIR}/large_flock_extension.cpp
    ${EXTENSION_SOURCES}
    PARENT_SCOPE
    )
>>>>>>> 09cb235 (update the folder structure, handle arguments and change litellm to openai-cpp)
