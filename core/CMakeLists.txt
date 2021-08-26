file(GLOB core_SRC
  "src/*.hpp"
  "src/*.cpp"
  )

add_library(core
  ${core_SRC}
  )

find_package(glm)
find_package(spdlog)
find_package(cereal)

target_link_libraries(core PUBLIC
  glm::glm
  spdlog::spdlog
  cereal::cereal
  )

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  find_library(SEGFAULT_LIBRARY NAMES SegFault REQUIRED)
  target_link_libraries(core PUBLIC ${SEGFAULT_LIBRARY})
  target_compile_options(core PUBLIC "-Wl" "-no-as-needed")
endif()

target_include_directories(core PUBLIC src/)

add_library(enflopio::core ALIAS core)
