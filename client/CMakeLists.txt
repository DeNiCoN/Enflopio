
file(GLOB client_SRC
  "src/*.hpp"
  "src/*.cpp"
  )

add_executable(client
  ${client_SRC}
  )

if (EMSCRIPTEN)
  find_package(glm)

  target_link_libraries(client
    glm::glm
    )

  set_target_properties(client PROPERTIES LINK_FLAGS
    "-s USE_WEBGL2=1 -s USE_GLFW=3 --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/emscripten.html"
    OUTPUT_NAME "index"
    SUFFIX ".html"
    )
else()
  find_package(glfw3)
  find_package(glad)
  find_package(glm)

  target_link_libraries(client
    glfw::glfw
    glad::glad
    glm::glm
    )
endif()
