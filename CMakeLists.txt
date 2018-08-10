cmake_minimum_required(VERSION 3.0)
project(Sparse CXX)
set (CMAKE_CXX_STANDARD 14)

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/boost_1_60_0)
set(LIBS )

set(COMPILE_FLAGS
  ${COMPILE_FLAGS}
  -Wall
  -Werror=implicit-function-declaration
  -Wno-unused-function
  -Wno-unused-result
)
set(COMPILE_FLAGS_DEBUG   ${COMPILE_FLAGS} -O0 -g -DDEBUG)
set(COMPILE_FLAGS_RELEASE ${COMPILE_FLAGS} -O3 -march=native)

add_executable(
  Sparse
  ${CMAKE_CURRENT_SOURCE_DIR}/sparse-matrix/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sparse-matrix/unit_test.cpp
)
target_compile_options(
    Sparse
    PRIVATE "$<$<CONFIG:DEBUG>:${COMPILE_FLAGS_DEBUG}>"
    PRIVATE "$<$<CONFIG:RELEASE>:${COMPILE_FLAGS_RELEASE}>"
)
target_include_directories(
    Sparse
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/sparse-matrix"
    PRIVATE ${INCLUDE_DIRS}
)
target_link_libraries(
    Sparse
    ${LIBS}
)
set_target_properties(
    Sparse
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)