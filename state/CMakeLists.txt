cmake_minimum_required(VERSION 3.25.1)
project(Realm)

set(CMAKE_CXX_STANDARD 20)

find_package(
        Boost 1.86.0
        COMPONENTS json thread program_options random coroutine
        REQUIRED
)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

include_directories(src/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(Serve main.cpp ${sources})

target_link_libraries(Serve ${Boost_LIBRARIES})