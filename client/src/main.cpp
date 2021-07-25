#include <functional>
#include <iostream>
#include <vector>
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

#include "test.hpp"
#include "World.hpp"
#include "App.hpp"

typedef struct {
  char *buffer;
  int length;
} msg_t;

using namespace Enflopio;

int main(void)
{
    spdlog::set_level(spdlog::level::debug);
    App::Instance().Run();

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
    return 0;
}
