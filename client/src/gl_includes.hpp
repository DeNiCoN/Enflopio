#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GL/gl.h>
#include <GLES2/gl2.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else
#include <glad/glad.h>
#endif
