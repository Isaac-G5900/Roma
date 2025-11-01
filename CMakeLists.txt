cmake_minimum_required(VERSION 3.28)
project(routingEngine)

add_library(routingEngine src/core.cpp)

target_compile_features(routingEngine PUBLIC cxx_std_20)
set(CMAKE_BUILD_TYPE Debug)
target_include_directories(routingEngine
PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>)

#Main executable
add_executable(main apps/main.cpp)
target_link_libraries(main PRIVATE routingEngine)

#Test executables
add_executable(graphtests tests/graphtests.cpp)
target_link_libraries(graphtests PRIVATE routingEngine)
enable_testing()
add_test(NAME graphtests COMMAND graphtests)
configure_file(tests/test_json.json ${CMAKE_BINARY_DIR}/test_json.json COPYONLY)