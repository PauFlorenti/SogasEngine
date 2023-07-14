
# Sandbox executable

file(GLOB_RECURSE SRC_FILES *.h *.cpp)

foreach(FILE in ${SRC_FILES})
    message(STATUS ${FILE})
endforeach()

include_directories()

add_executable(sandbox ${SRC_FILES})
target_link_libraries(sandbox PUBLIC engine)