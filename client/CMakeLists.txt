
file(GLOB client_SRC
  "src/*.hpp"
  "src/*.cpp"
  )

add_executable(client
  ${client_SRC}
  )

find_package(glfw3)
find_package(glad)
find_package(glm)

target_link_libraries(client
  glfw::glfw
  glad::glad
  glm::glm
  )
