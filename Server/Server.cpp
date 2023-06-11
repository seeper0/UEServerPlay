#include <iostream>
#include <WS2tcpip.h>
#include <rpc.h>
#pragma comment (lib, "ws2_32.lib")
#pragma comment(lib, "Rpcrt4.lib")

#include "ServerNetwork.h"

using std::cout;
using std::cerr;
using std::endl;


int main(int argc, char const* argv[])
{
    ServerNetwork Server;
    Server.Run();

    return 0;
}
