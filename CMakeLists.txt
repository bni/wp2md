cmake_minimum_required(VERSION 3.22)
project(wp2md)

set(CMAKE_CXX_STANDARD 98)

add_executable(wp2md wp2md.c)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp ../example1.wp .
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND wp2md example1.wp | iconv -f ISO-8859-1 -t UTF-8 > example1.md
)
