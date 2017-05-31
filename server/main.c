#include "server_tools.h"
#include "debug.h"

int main ()
{
    ServerInfo serverInfo;
    ClientInfo clientInfo;
    char message[BUFLEN] = {0};

    // initialize server information structre
    serverInfo = serverInit ();
    // Initialize client information structure
    clientInit (&clientInfo);
    // prints server name,ip,port
    printServerInfo ();

    // server is running
    for (;;)
    {
        // connect/disconnect users
        acceptConnection (&serverInfo, &clientInfo);
        // proccess client messages
        atYourService (&clientInfo, message);
    }

    // Deinitialize server
    closesocket (serverInfo.serverSocket);
    WSACleanup ();
    return 0;
}
