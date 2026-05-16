cmake_minimum_required(VERSION 3.16)

project(RohinishTask3SmartBank LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(smartbank_lib
    src/SmartBank.cpp
)

target_include_directories(smartbank_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(smartbank_demo
    src/main.cpp
)

target_link_libraries(smartbank_demo
    PRIVATE
        smartbank_lib
)
