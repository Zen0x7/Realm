cmake_minimum_required(VERSION 3.25.1)
project(Realm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(
        Boost 1.86.0
        COMPONENTS json thread program_options random coroutine
        REQUIRED
)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(Serve main.cpp ${sources})

target_link_libraries(Serve ${Boost_LIBRARIES})