
cmake_minimum_required(VERSION 3.24.2)

project(SogasEngine
    VERSION 1.0.0
    DESCRIPTION "Sogas Engine"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT DEFINED CMAKE_BUILD_TYPE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../output/${CMAKE_BUILD_TYPE})

add_subdirectory(Engine)
add_subdirectory(EngineTest)
add_subdirectory(Sandbox)

set(CMAKE_SUPPRESS_REGENERATION true)