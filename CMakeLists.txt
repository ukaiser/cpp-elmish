
cmake_minimum_required(VERSION 3.5)
project(cpp-elmish VERSION 0.0.1 LANGUAGES CXX)


#Make sure that custom modules like are found
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

if(MSVC)
    add_compile_options(/W3 /WX)
else()
    add_compile_options(-W -Wall -Werror)

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-Wno-unused-lambda-capture)
    endif()
endif()

add_subdirectory(third_party)
add_subdirectory(src)