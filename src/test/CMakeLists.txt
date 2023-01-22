cmake_minimum_required(VERSION 3.10)

project(logging)

add_executable(logging-test)

target_sources(logging-test
               PRIVATE
               test.cc)

target_include_directories(logging-test
                           PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../include)