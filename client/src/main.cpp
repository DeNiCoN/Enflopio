#include <functional>
#include <iostream>
#include <vector>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <thread>
#include <cassert>
#include <malloc.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    {-0.6f, -0.4f, 1.f, 0.f, 0.f},
    {0.6f, -0.4f, 0.f, 1.f, 0.f},
    {0.f, 0.6f, 0.f, 0.f, 1.f}
};

static const char *vertex_shader_text =
    "uniform mat4 MVP;\n"
    "attribute vec3 vCol;\n"
    "attribute vec2 vPos;\n"
    "varying vec3 color;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
    "    color = vCol;\n"
    "}\n";

static const char *fragment_shader_text =
    "precision mediump float;\n"
    "varying vec3 color;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = vec4(color, 1.0);\n"
    "}\n";

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

std::function<void()> loop;
void main_loop() { loop(); }

typedef struct {
  char *buffer;
  int length;
} msg_t;

int do_msg_read(int sockfd, msg_t *msg, int offset, int length, struct sockaddr *addr, socklen_t *addrlen) {
  int res;

  if (!msg->length) {
    // read the message length
    res = recvfrom(sockfd, &msg->length, sizeof(int), 0, addr, addrlen);
    if (res == -1) {
      assert(errno == EAGAIN);
      return res;
    } else if (res == 0) {
      return 0;
    }

    printf("do_msg_read: allocating %d bytes for message\n", msg->length);

    msg->buffer = (char*) malloc(msg->length);
  }
  // read the actual message
  int max = msg->length - offset;
  if (length && max > length) {
    max = length;
  }
  res = recvfrom(sockfd, msg->buffer + offset, max, 0, addr, addrlen);
  if (res == -1) {
    assert(errno == EAGAIN);
    return res;
  }

  printf("do_msg_read: read %d bytes\n", res);

  return res;
}

void check_error(GLuint shader)
{
    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        GLint log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<GLchar> log(log_length);

        GLsizei length;
        glGetShaderInfoLog(shader, log.size(), &length, log.data());

        error_callback(0, log.data());
    }
}

int main(void)
{
    std::cout << "Hello biggers\n";
    /*std::cout << "Connecting to server...\n";

    struct sockaddr_in sa;
    int res;
    int SocketFD;

    SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFD == -1) {
        std::cout << "Cannot create socket\n";
      return EXIT_FAILURE;
    }
    fcntl(SocketFD, F_SETFL, O_NONBLOCK);

    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;

#ifdef __EMSCRIPTEN__
    sa.sin_port = htons(25326);
#else
    sa.sin_port = htons(25325);
#endif

    std::cout << "Connecting to port " << sa.sin_port;
    if (inet_pton(AF_INET, "93.74.188.74", &sa.sin_addr) != 1)
    {
        std::cout << "inte_pton failed" << std::endl;
    }

    if (connect(SocketFD, (struct sockaddr *)&sa, sizeof sa) == -1 && errno != EINPROGRESS) {
      std::cout << "connect failed\n";
      close(SocketFD);
      return EXIT_FAILURE;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); //good design

    char buf[6];

    msg_t msg;
    msg.buffer = buf;
    msg.length = 6;
    while (do_msg_read(SocketFD, &msg, 0, 6, NULL, NULL) == -1 && errno == EAGAIN);

    std::cout << "Server says: " << buf << "\n";
    std::cout << "Disconnecting\n";
    close(SocketFD);
    */
    GLint mvp_location, vpos_location, vcol_location;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    auto window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
#ifdef __EMSCRIPTEN__
#else
    gladLoadGL();
#endif
    glfwSwapInterval(1);
    // NOTE: OpenGL error checks have been omitted for brevity
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
    check_error(vertex_shader);

    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    check_error(fragment_shader);

    auto program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)(sizeof(float) * 2));

    loop = [&] {
        float ratio;
        int width, height;
        mat4x4 m, p, mvp;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float)glfwGetTime());
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)mvp);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    };

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
#else
    while (!glfwWindowShouldClose(window))
        main_loop();
#endif

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
