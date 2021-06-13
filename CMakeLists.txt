cmake_minimum_required(VERSION 3.20)

project(Enflopio)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(libs)
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/libs)

#add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(server)

enable_testing()
#if(BUILD_TESTING)
#  add_subdirectory(tests)
#endif()
