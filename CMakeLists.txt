cmake_minimum_required(VERSION 3.25.1)
project(Realm)

set(CMAKE_CXX_STANDARD 20)

find_package(
        Boost 1.86.0
        COMPONENTS json thread program_options random coroutine
        REQUIRED
)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/b514bdc898e2951020cbdca1304b75f5950d1f59.zip
)

set(CMAKE_CXX_FLAGS --coverage)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE tests CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc")

include_directories(src/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

enable_testing()

add_executable(Test ${tests} ${sources})

add_executable(Serve server.cpp ${sources})
add_executable(Worker worker.cpp ${sources})

target_link_libraries(Serve ${Boost_LIBRARIES})
target_link_libraries(Worker ${Boost_LIBRARIES})
target_link_libraries(Test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(Test)