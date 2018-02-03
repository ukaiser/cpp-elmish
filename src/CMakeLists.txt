

set( SRCS
main.cpp 
)

add_executable(cpp-elmish ${SRCS})
target_compile_features(cpp-elmish PUBLIC cxx_std_17)


