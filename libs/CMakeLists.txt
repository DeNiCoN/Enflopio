if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake"
    EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
    TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

if (EMSCRIPTEN)
  conan_cmake_configure(REQUIRES
    glm/[>=0.9.9]

    BUILD_REQUIRES
    emsdk_installer/1.38.29@bincrafters/stable

    GENERATORS
    cmake_find_package
    )

  conan_cmake_autodetect(settings)

  list(APPEND settings "os=Emscripten" "arch=wasm")
else()
  conan_cmake_configure(REQUIRES
    glfw/[>=3.3.4]
    glad/[>=0.1.34]
    glm/[>=0.9.9]
    uwebsockets/[>=19.2.0]
    boost/[>=1.76.0]
    spdlog/[>=1.8.5]

    OPTIONS
    glad:gl_profile=core
    glad:spec=gl
    glad:gles2_version=3.2
    glad:no_loader=False

    GENERATORS
    cmake_find_package
    )

  conan_cmake_autodetect(settings)
endif()

conan_cmake_install(PATH_OR_REFERENCE .
  BUILD missing
  REMOTE conan-center
  SETTINGS ${settings})
