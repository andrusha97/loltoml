CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(LOLTOML-EXAMPLES)

INCLUDE_DIRECTORIES(BEFORE
    ${CMAKE_SOURCE_DIR}/include
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -W -Wall -Werror -Wextra -pedantic")

ADD_EXECUTABLE(simple simple.cpp)

ADD_EXECUTABLE(roundtrip roundtrip.cpp)

ADD_EXECUTABLE(toml-test-decoder toml-test-decoder.cpp)
TARGET_LINK_LIBRARIES(toml-test-decoder kora-util)
