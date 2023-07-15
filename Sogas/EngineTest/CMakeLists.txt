
file(GLOB_RECURSE SOURCES *.h *.hpp *.c *.cpp)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(EngineTest ${SOURCES})
target_link_libraries(EngineTest PUBLIC engine)

target_precompile_headers(EngineTest PUBLIC src/pch.hpp)
