file(GLOB core_SRC
  "src/*.hpp"
  "src/*.cpp"
  )

add_library(core
  ${core_SRC}
  )

find_package(glm)
find_package(spdlog)

target_link_libraries(core PUBLIC
  glm::glm
  spdlog::spdlog
  )

target_include_directories(core PUBLIC src/)

add_library(enflopio::core ALIAS core)
