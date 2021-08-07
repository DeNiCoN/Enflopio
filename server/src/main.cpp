#include <iostream>
#include <boost/asio.hpp>
#include <App.h>
#include "server.hpp"
#include <spdlog/spdlog.h>

using namespace std;
using namespace Enflopio;

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::debug);

    //Start server with default options
    Server server({});
    server.Run();
    return 0;
}
