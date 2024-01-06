file(GLOB_RECURSE SOURCES *.h *.hpp *.c *.cpp)

if(MSVC)
    add_compile_options(/W4 /WX)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_library(engine STATIC ${SOURCES})

set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external)
set(SECOND_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_include_directories(engine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${EXTERNAL_DIR}
    ${EXTERNAL_DIR}/json/include
    ${SECOND_PARTY_DIR}/Logger/include
    ${SECOND_PARTY_DIR}/Pinut/include
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${EXTERNAL_DIR}/stb
    ${EXTERNAL_DIR}/tinygltf
    ${EXTERNAL_DIR}/tinyobj
)

target_link_libraries(engine PRIVATE logger pinut)

target_precompile_headers(engine PUBLIC src/pch.hpp)
