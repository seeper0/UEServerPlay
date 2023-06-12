#include "ServerNetwork.h"
#pragma comment (lib, "ws2_32.lib")

using std::cout;
using std::cerr;
using std::endl;


int main(int argc, char const* argv[])
{
    ServerNetwork Server;
    Server.Run();

    return 0;
}
