file(GLOB_RECURSE SOURCES *.h *.cpp)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../output/${CMAKE_BUILD_TYPE})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../output/${CMAKE_BUILD_TYPE})

add_library(SogasEngine STATIC ${SOURCES})

target_include_directories(SogasEngine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_precompile_headers(SogasEngine PUBLIC src/pch.hpp)