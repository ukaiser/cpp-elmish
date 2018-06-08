

add_executable(cpp-elmish main.cpp)
set_property(TARGET cpp-elmish PROPERTY CXX_STANDARD 17)

find_package (Threads)

target_link_libraries(cpp-elmish 
    PRIVATE 

        ${CMAKE_THREAD_LIBS_INIT}
        fmt
        pybind11::embed
)

target_include_directories(cpp-elmish PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

