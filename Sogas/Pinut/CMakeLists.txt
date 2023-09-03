file(GLOB_RECURSE SOURCES *.h *.hpp *.c *.cpp)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

find_package(Vulkan)

if(NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
    set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
    STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
endif(NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")

if(NOT Vulkan_FOUND)
    message(STATUS "Failed to locate Vulkan SDK.")
endif(NOT Vulkan_FOUND)

add_library(pinut STATIC ${SOURCES})

target_include_directories(pinut
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Vulkan_INCLUDE_DIR}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(pinut PRIVATE
    ${Vulkan_LIBRARY}
)

target_precompile_headers(pinut PUBLIC src/pch.hpp)
