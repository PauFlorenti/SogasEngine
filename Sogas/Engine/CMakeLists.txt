file(GLOB_RECURSE SOURCES *.h *.hpp *.c *.cpp)

if(MSVC)
    add_compile_options(/W4 /WX)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# set(CMAKE_CXX_FLAGS_RELEASE "/MT")
# set(CMAKE_CXX_FLAGS_DEBUG "/MTd")

add_library(engine STATIC ${SOURCES})

target_include_directories(engine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/json/include
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../pinut/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../external
)

target_link_libraries(engine PRIVATE pinut)

target_precompile_headers(engine PUBLIC src/pch.hpp)