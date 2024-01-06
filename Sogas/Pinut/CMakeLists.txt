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

set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external)
set(IMGUI_DIR ${EXTERNAL_DIR}/imgui)

AUX_SOURCE_DIRECTORY(${IMGUI_DIR} SRC_IMGUI)

add_library(imgui STATIC ${SRC_IMGUI}
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp)

target_include_directories(imgui PRIVATE ${IMGUI_DIR} ${Vulkan_INCLUDE_DIR})

target_include_directories(pinut
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Vulkan_INCLUDE_DIR}
    ${IMGUI_DIR}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../logger/include
    ${EXTERNAL_DIR}
)

target_link_libraries(pinut PRIVATE
    ${Vulkan_LIBRARY}
    logger
    imgui
)

target_precompile_headers(pinut PUBLIC src/pch.hpp)
