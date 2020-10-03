cmake_minimum_required(VERSION 3.1)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -Wall")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG}  -fsanitize=address")

set(CMAKE_C_STANDARD 11)

project(stack)
include_directories(src include)

set(STACK_SRC
    )

add_executable(runstack src/main.c ${STACK_SRC})
target_link_libraries(runstack m)
