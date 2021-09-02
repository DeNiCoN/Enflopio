if (server_ENABLED)
  file(GLOB server_SRC
    "src/*.hpp"
    "src/*.cpp"
    )

  add_executable(server
    ${server_SRC}
    )

  find_package(uwebsockets)
  find_package(Boost COMPONENTS REQUIRED system)
  find_package(spdlog)
  find_package(replxx)

  target_link_libraries(server
    enflopio::core
    replxx::replxx
    uwebsockets::uwebsockets
    Boost::Boost
    spdlog::spdlog
    )

  target_include_directories(server PUBLIC src/)

  include(CPack)

  install(TARGETS server
    DESTINATION .
    )
endif()
