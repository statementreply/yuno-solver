cmake_minimum_required(VERSION 3.8)
project(yuno-solver)

#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup()

add_executable(yuno-solver
  src/yuno-solver.cpp
  src/solve.cpp
)
#target_link_libraries(yuno-solver ${CONAN_LIBS})

target_compile_features(yuno-solver PUBLIC cxx_std_17)
set_target_properties(yuno-solver PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(yuno-solver PRIVATE include/)

target_compile_options(yuno-solver PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:AppleClang>>:
    -Wall -Wextra
  >
  $<$<CXX_COMPILER_ID:MSVC>:
    /permissive- /W4
  >
)
