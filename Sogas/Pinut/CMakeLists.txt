file(GLOB_RECURSE SOURCES *.h *.hpp *.c *.cpp)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

message("Looking for installed Vulkan SDK")
find_package(Vulkan)

if(Vulkan_FOUND)
    message("Vulkan found!")
    message("Vulkan include directory: " ${Vulkan_INCLUDE_DIR})
    message("Vulkan libs: " ${Vulkan_LIBRARY})
else()
    message("Automatic vulkan fetch failed. Trying manually again.")
    if(NOT DEFINED ENV{VULKAN_SDK})
        message(FATAL_ERROR "Failed to locate Vulkan SDK.")
    endif()

    set(VULKAN_SDK "$ENV{VULKAN_SDK}")
    message("Vulkan SDK path: " ${VULKAN_SDK})

    message("${VULKAN_SDK}/Include")

    find_path(
        Vulkan_INCLUDE_DIR vulkan/vulkan.h
        PATHS "${VULKAN_SDK}/Include" "${VULKAN_SDK}/include")

    message("Vulkan include directory: " ${Vulkan_INCLUDE_DIR})

    set(VULKAN_LIBRARY_DIRS "${VULKAN_SDK}/lib" "${VULKAN_SDK}/Lib")

    find_library(
        Vulkan_LIBRARY
        NAMES vulkan vulkan-1
        PATHS ${VULKAN_LIBRARY_DIRS}
    )

    message("Vulkan libs: " ${Vulkan_LIBRARY})
endif(Vulkan_FOUND)

add_library(pinut STATIC ${SOURCES})

target_include_directories(pinut
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Vulkan_INCLUDE_DIR}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../logger/include
)

target_link_libraries(pinut PRIVATE
    ${Vulkan_LIBRARY}
    logger
)

target_precompile_headers(pinut PUBLIC src/pch.hpp)
