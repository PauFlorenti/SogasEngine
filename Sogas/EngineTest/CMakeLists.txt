
file(GLOB_RECURSE SOURCES *.h *.hpp *.c *.cpp)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

enable_testing()
add_subdirectory(${PROJECT_SOURCE_DIR}/../external/googletest ${PROJECT_SOURCE_DIR}/../output/googletest)

include_directories(${PROJECT_SOURCE_DIR}/engine/include)

add_executable(EngineTest ${SOURCES})
target_link_libraries(EngineTest PUBLIC engine gtest_main)

target_include_directories(EngineTest
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    SYSTEM
    ${PROJECT_SOURCE_DIR}/../external/
    ${PROJECT_SOURCE_DIR}/../external/googletest/googletest/include)

target_precompile_headers(EngineTest PUBLIC src/pch.h)
