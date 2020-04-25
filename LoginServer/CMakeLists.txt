cmake_minimum_required(VERSION 3.15)
project(LoginServer)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED COMPONENTS
system
filesystem
)

include_directories(${Boost_INCLUDE_DIRS})
add_executable(LoginServer main.cpp server.cpp server.h)
target_link_libraries(LoginServer ${Boost_LIBRARIES} pthread)