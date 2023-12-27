file(GLOB_RECURSE SOURCES *.h *.hpp *.c *.cpp)

if(MSVC)
    add_compile_options(/W4 /WX)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_library(engine STATIC ${SOURCES})

target_include_directories(engine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Pinut/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/json/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/stb
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../Logger/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/tinyobj
    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/tinygltf
)

target_link_libraries(engine PUBLIC logger PRIVATE pinut)

target_precompile_headers(engine PUBLIC src/pch.hpp)
