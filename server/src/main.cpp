#include <iostream>
#include <boost/asio.hpp>
#include <App.h>
#include "server.hpp"

using namespace std;
using namespace Enflopio;

int main(int argc, char *argv[]) {
    std::cout << "Lets go!!\n";

    //Start server with default options
    Server server({});
    server.Run();
    return 0;
}
