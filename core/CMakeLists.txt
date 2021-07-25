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

target_include_directories(core PUBLIC src/)

add_library(enflopio::core ALIAS core)
