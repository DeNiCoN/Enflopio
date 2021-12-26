if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake"
    EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
    TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

set(packages "")
set(build_requires "")
set(options "")
set(env "")
conan_cmake_autodetect(settings)

# core
list(APPEND packages
  glm/[>=0.9.9]
  spdlog/[>=1.8.5]
  cereal/[>=1.3.0]
  )

list(APPEND options
  spdlog:header_only=True
  fmt:header_only=True
  )

if (client_ENABLED)
  list(APPEND packages
    imgui/[>=1.84.2]
    )

  if (EMSCRIPTEN)
    list(APPEND build_requires
      emsdk_installer/1.38.29@bincrafters/stable
      )

    list(APPEND settings "os=Emscripten" "arch=wasm")

    list(APPEND env
      CC=/usr/bin/clang
      CXX=/usr/bin/clang++
      )

    list(APPEND options
      )
  else()
    list(APPEND packages
      glfw/[>=3.3.4]
      glad/[>=0.1.34]
      boost/[>=1.76.0]
      )

    list(APPEND options
      glad:gl_profile=core
      glad:spec=gl
      glad:gles2_version=3.2
      glad:no_loader=False
      )
  endif()
endif()

if (server_ENABLED)
  list(APPEND packages
    uwebsockets/[>=19.2.0]
    boost/[>=1.76.0]
    spdlog/[>=1.8.5]
    replxx/[>=0.0.3]
    )
endif()

conan_cmake_configure(
  REQUIRES ${packages}
  BUILD_REQUIRES ${build_requires}
  OPTIONS ${options}
  GENERATORS cmake_find_package
  IMPORTS "res/bindings, imgui_impl_glfw.* -> ${CMAKE_SOURCE_DIR}/client/bindings"
  IMPORTS "res/bindings, imgui_impl_opengl3.* -> ${CMAKE_SOURCE_DIR}/client/bindings"
  IMPORTS "res/bindings, imgui_impl_opengl3_loader.* -> ${CMAKE_SOURCE_DIR}/client/bindings"
  )

conan_cmake_install(PATH_OR_REFERENCE .
  BUILD missing
  SETTINGS ${settings}
  ENV ${env}
  )
