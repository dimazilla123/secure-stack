cmake_minimum_required(VERSION 3.1)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra")

set(CMAKE_C_STANDARD 11)

# the name of the target operating system
if (CMAKE_SYSTEM_NAME STREQUAL WindowsCrossCompile)
    SET(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
    SET(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
    SET(CMAKE_RC_COMPILER i686-w64-mingw32-windres)

    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__USE_MINGW_ANSI_STDIO=1")

    # here is the target environment located
    SET(CMAKE_FIND_ROOT_PATH  /usr/x86_64-w64-mingw32)

    # adjust the default behaviour of the FIND_XXX() commands:
    # search headers and libraries in the target environment, search
    # programs in the host environment
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
else()
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
endif()


project(stack)
include_directories(src include)

set(STACK_SRC
    )

add_executable(runstack src/main.c ${STACK_SRC})
add_executable(rapestack src/rape.c ${STACK_SRC})
target_link_libraries(runstack)
