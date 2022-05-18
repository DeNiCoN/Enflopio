#include "app.hpp"

using namespace Enflopio;

int main(void)
{
    try {
        App::Instance().Run();
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
