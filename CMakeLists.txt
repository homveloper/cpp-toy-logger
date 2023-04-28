cmake_minimum_required(VERSION 3.0.0)
project(logger VERSION 0.1.0)


find_package(Threads REQUIRED)

include(CTest)
enable_testing()

add_executable(logger main.cpp)
target_link_libraries(logger Threads::Threads)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 14)



include(CPack)
