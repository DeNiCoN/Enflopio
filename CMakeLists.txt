cmake_minimum_required(VERSION 3.16)

project(Enflopio)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_COMPILER_VERSION 12.0.0)
message(${CMAKE_CXX_COMPILER_VERSION})

add_subdirectory(libs)
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/libs)

#add_subdirectory(core)
add_subdirectory(client)

if (EMSCRIPTEN)
  add_subdirectory(backend)
else()
  add_subdirectory(server)

  enable_testing()
  #if(BUILD_TESTING)
  #  add_subdirectory(tests)
  #endif()
endif()
