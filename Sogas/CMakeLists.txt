
cmake_minimum_required(VERSION 3.24.2)

project(SogasEngine
VERSION 1.0.0
DESCRIPTION "Sogas Engine"
LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT DEFINED CMAKE_BUILD_TYPE)

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:msvcrt")

    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../output)

add_subdirectory(Logger)
add_subdirectory(Pinut)
add_subdirectory(Engine)
add_subdirectory(Sandbox)

if(${USE_TESTS})
    add_subdirectory(EngineTest)
endif(${USE_TESTS})

set(CMAKE_SUPPRESS_REGENERATION true)
