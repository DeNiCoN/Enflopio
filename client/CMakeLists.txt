if (client_ENABLED)
  file(GLOB client_SRC
    "src/*.hpp"
    "src/*.cpp"
    "bindings/*.hpp"
    "bindings/*.cpp"
    )

  add_executable(client
    ${client_SRC}
    )

  find_package(imgui)

  target_link_libraries(client
    enflopio::core
    imgui::imgui
    )

  if (EMSCRIPTEN)
    set_target_properties(client PROPERTIES LINK_FLAGS
      "-s ASYNCIFY -lwebsocket.js -s USE_WEBGL2=1 -s MAX_WEBGL_VERSION=2 -s USE_GLFW=3 --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/emscripten.html"
      OUTPUT_NAME "index"
      SUFFIX ".html"
      )
  else()
    find_package(glfw3)
    find_package(glad)
    find_package(Boost COMPONENTS REQUIRED system)

    target_link_libraries(client
      glfw::glfw
      glad::glad
      Boost::Boost
      )
  endif()

  target_include_directories(client PUBLIC src/ bindings/)
endif()
