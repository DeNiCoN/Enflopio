cmake_minimum_required(VERSION 3.16)

project(Enflopio)

set(CMAKE_CXX_STANDARD 20)

if (EMSCRIPTEN)
  set(CMAKE_CXX_COMPILER_VERSION 12.0.0)
endif()

if (EMSCRIPTEN)
  option(client_ENABLED "Build client" ON)
  option(backend_ENABLED "Build backend" ON)
  option(server_ENABLED "Build server" OFF)
else()
  option(client_ENABLED "Build client" ON)
  option(backend_ENABLED "Build backend" OFF)
  option(server_ENABLED "Build server" ON)
endif()

add_subdirectory(libs)
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/libs)

option(DEPENDENCIES_ONLY "Do not configure targets" OFF)
if (DEPENDENCIES_ONLY)
  return()
endif()

add_compile_options(-Wall -Wextra -pedantic)
add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(backend)


option(ENABLE_TESTING "Enable testing" OFF)
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
