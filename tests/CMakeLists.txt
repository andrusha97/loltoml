CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(LOLTOML-TESTS)

ADD_DEFINITIONS(-DGTEST_USE_OWN_TR1_TUPLE=1)
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/foreign/gtest ${CMAKE_BINARY_DIR}/gtest)

INCLUDE_DIRECTORIES(BEFORE
    ${CMAKE_SOURCE_DIR}/include
)

INCLUDE_DIRECTORIES(SYSTEM
    ${CMAKE_SOURCE_DIR}/foreign/gtest/include
)

LINK_DIRECTORIES(
    ${CMAKE_BINARY_DIR}/gtest
)

ADD_DEFINITIONS(-DTESTS_ROOT="${CMAKE_SOURCE_DIR}/tests/")
ADD_EXECUTABLE(loltoml-unittests
    array.cpp
    array_table.cpp
    basic_string.cpp
    boolean.cpp
    comments.cpp
    complex.cpp
    datetime.cpp
    empty.cpp
    float.cpp
    inline_table.cpp
    integer.cpp
    key.cpp
    literal_string.cpp
    multiline_string.cpp
    multiline_literal_string.cpp
    table.cpp
)

TARGET_LINK_LIBRARIES(loltoml-unittests
    gtest_main
    gtest
)

SET_TARGET_PROPERTIES(loltoml-unittests PROPERTIES
    COMPILE_FLAGS "-std=c++0x -W -Wall -Werror -Wextra -pedantic"
)

ADD_TEST(NAME loltoml-unittests COMMAND loltoml-unittests)

ADD_CUSTOM_COMMAND(
    TARGET loltoml-unittests
    COMMENT "Running unit tests"
    POST_BUILD COMMAND ctest --output-on-failure -R loltoml-unittests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
