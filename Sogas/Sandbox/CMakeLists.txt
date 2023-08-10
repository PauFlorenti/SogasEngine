# Sandbox executable

file(GLOB_RECURSE SOURCES *.h *.hpp *.c *.cpp)

include_directories(${PROJECT_SOURCE_DIR}/engine/include)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(sandbox ${SOURCES})
target_link_libraries(sandbox PUBLIC engine)

# Make linker to export all symbols from the library.
if(MSVC)
    set_target_properties(sandbox PROPERTIES LINK_FLAGS "/WHOLEARCHIVE:engine")
else(MSVC)
endif(MSVC)
