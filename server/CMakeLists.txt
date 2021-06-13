
file(GLOB server_SRC
  "src/*.hpp"
  "src/*.cpp"
  )

add_executable(server
  ${server_SRC}
  )

find_package(uwebsockets)
find_package(Boost COMPONENTS REQUIRED system)

target_link_libraries(server
  uwebsockets::uwebsockets
  Boost::Boost
  )
