cmake_minimum_required(VERSION 3.25.1)
project(Realm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(
        Boost 1.86.0
        COMPONENTS json thread program_options random coroutine
        REQUIRED
)

add_executable(Serve main.cpp)

target_link_libraries(Serve ${Boost_LIBRARIES})