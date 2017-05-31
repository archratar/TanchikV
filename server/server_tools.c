#include "server_tools.h"
//--------------------------------------------------------
/*
 * Initialized serverInfo structure with needed values
 * wsa - windows structure used in socket programming
 * creating tcp/ipv4 socket
 * also, call the bindListen function
 * and initialized fdSet structure used in select function
 */
ServerInfo serverInit (void)
{
    ServerInfo serverInfo;
    unsigned int isOk = 0;

    isOk = WSAStartup(MAKEWORD(2,2), &serverInfo.wsa);
    if (isOk != 0)
    {
        printf("wsa error: %i\n", isOk = WSAGetLastError());
        exit(WSAGetLastError());
    }

    isOk = serverInfo.serverSocket = socket (IPV4, TCP, 0);
    if (isOk == INVALID_SOCKET)
    {
        printf("socket error: %i\n", isOk = WSAGetLastError());
        exit(isOk);
    }

    serverInfo.server.sin_family      = IPV4;
    serverInfo.server.sin_port        = htons(PORT);
    serverInfo.server.sin_addr.s_addr = INADDR_ANY;

    serverInfo.addrlen = sizeof (struct sockaddr_in);

    serverInfo.timeOut.tv_sec  = TIMEVAL_SEC;
    serverInfo.timeOut.tv_usec = TIMEVAL_USEC;

    bindListen (&serverInfo);

    FD_ZERO (&serverInfo.fdServer);
    FD_SET (serverInfo.serverSocket, &serverInfo.fdServer);

    return serverInfo;
}
//--------------------------------------------------------
/*
 * Binds server socket on defined PORT
 * Sets server socket in listen mode
 * returns 0 if success, else - wsa error code
 */
int bindListen (ServerInfo *serverInfo)
{
    int isOk = 0;

    isOk = bind (serverInfo->serverSocket, (struct sockaddr*)&serverInfo->server, sizeof(serverInfo->server));
    if (isOk != 0)
        return WSAGetLastError();

    isOk = listen (serverInfo->serverSocket, MAXCLIENTS);
    if (isOk != 0)
        return WSAGetLastError();

    return isOk;
}
//--------------------------------------------------------
/*
 * Initializing clientInfo structure with needed values
 */
void clientInit (ClientInfo *clientInfo)
{
    FD_ZERO (&clientInfo->fdClient);
    for (unsigned int i = 0; i < MAXCLIENTS; i++)
        clientInfo->clientSocket[i] = 0;

    for (unsigned int i = 0; i < MAXCLIENTS; i++)
        for (unsigned int j = 0; j < NICKLEN; j++)
            clientInfo->nickname[i][j] = '\0';

    clientInfo->timeOut.tv_sec = TIMEVAL_SEC;
    clientInfo->timeOut.tv_usec= TIMEVAL_USEC;
    clientInfo->addrlen = sizeof(struct sockaddr_in);
}
//--------------------------------------------------------
/*
 * accepts new connection
 * check ready to read sockets on server main socket
 * if there is any, accept connection, copy new socket descriptor in clientInfo structure
 * and sends GREETINGS message
 */
void acceptConnection (ServerInfo *serverInfo, ClientInfo *clientInfo)
{
    SOCKET tmpSock = {0};
    int isClient = 0;

    FD_ZERO (&serverInfo->fdServer);
    FD_SET (serverInfo->serverSocket, &serverInfo->fdServer);

    isClient = select (0, &serverInfo->fdServer, NULL, NULL, &serverInfo->timeOut);

    if (isClient > 0 && clientInfo->fdClient.fd_count <= MAXCLIENTS)
    {
        tmpSock = accept (serverInfo->serverSocket, (struct sockaddr*)&serverInfo->server, &serverInfo->addrlen);

        for (unsigned int i = 0; i < MAXCLIENTS; i++)
        {
            if (clientInfo->clientSocket[i] == 0)
            {
                clientInfo->clientSocket[i] = tmpSock;

                getpeername (clientInfo->clientSocket[i], (struct sockaddr*)&clientInfo->client, &clientInfo->addrlen);
                printf("new client : %s %i\n", inet_ntoa (clientInfo->client.sin_addr), ntohs (clientInfo->client.sin_port));
                send (tmpSock, GREETINGS, strlen(GREETINGS), 0);

                isClient = 0;
                return;
            }
        }
    }
    return;
}
//--------------------------------------------------------
/*
 * "main" server function
 */
void atYourService (ClientInfo *clientInfo, char *message)
{
    int activity = 0;
    int valread  = 0;

    // setting valid users socket descriptors to fdSet structure
    fileDescsSet (clientInfo);

    // check ready to read users sockets
    activity = select (0, &clientInfo->fdClient, NULL, NULL, &clientInfo->timeOut);

    // if there is any action, then its message from users
    if (activity > 0)
    {
        for (unsigned int i = 0; i < MAXCLIENTS; i++)
        {
            // if the client exists in fdSet structure
            if (FD_ISSET (clientInfo->clientSocket[i], &clientInfo->fdClient))
            {
                // get user information
                getpeername (clientInfo->clientSocket[i], (struct sockaddr*)&clientInfo->client, &clientInfo->addrlen);

                // set socket to nonblocking mode
                ioctlsocket (clientInfo->clientSocket[i], FIONBIO, (unsigned long*)TRUE);
                // reading socket income information
                valread = recv (clientInfo->clientSocket[i], message, BUFLEN, 0);

                // if client disconnected - call onClientDisconnect() function
                if (WSAGetLastError() == WSAECONNRESET || valread == SOCKET_ERROR || valread == 0)
                    onClientDisconnect (clientInfo, i);
                else
                {
                    // handling enters and other message junk
                    if (*(message+0) == 0 || *(message+0) == 13 || *(message+0) == '\n')
                        continue;

                    *(message+valread) = '\0';

                    // if it is first client message, then it is nickname
                    // sets nickname and notify users about new member
                    if (clientInfo->nickname[i][0] == 0)
                    {
                        clientNickSet (clientInfo, i, message);
                        newChatMemberNotifier (clientInfo, message);

                        valread = 0;
                        continue;
                    }

                    // log information
                    printf("%s:%i:%s", inet_ntoa(clientInfo->client.sin_addr), ntohs (clientInfo->client.sin_port), clientInfo->nickname[i]);
                    printf("%s", message);
                    // echo message to valid clients
                    echoMsg (clientInfo, message, i);

                    memset (message, 0, strlen(message));
                    valread = 0;
                }
            }
        }
    }
}
//--------------------------------------------------------
/*
 * Echo client message to all valid clients
 * first sends nickname, then message
 */
void echoMsg (ClientInfo *clientInfo, char *message, unsigned int clIndex)
{
    for (unsigned int i = 0; i < MAXCLIENTS; i++)
    {
        if (clientInfo->clientSocket[i] != 0)
        {
            send (clientInfo->clientSocket[i], clientInfo->nickname[clIndex], strlen(clientInfo->nickname[clIndex]), 0);
            send (clientInfo->clientSocket[i], message, strlen(message), 0);
        }
    }
}
//--------------------------------------------------------
/*
 * Copy valid client sockets descriptors to fdSet structure
 */
void fileDescsSet (ClientInfo *clientInfo)
{
    FD_ZERO (&clientInfo->fdClient);

    for (unsigned int i = 0; i < MAXCLIENTS; i++)
        if (clientInfo->clientSocket[i] != 0)
            FD_SET (clientInfo->clientSocket[i], &clientInfo->fdClient);
}
//--------------------------------------------------------
/*
 * Disconnects client socket
 * setting socket descriptor and nickname to 0
 * also, inform existed registered users about client disconnection
 */
void onClientDisconnect (ClientInfo *clientInfo, int clIndex)
{
    printf("host lost, ip: %s, port: %i, nick: %s\n", inet_ntoa (clientInfo->client.sin_addr), ntohs (clientInfo->client.sin_port), clientInfo->nickname[clIndex]);
    closesocket (clientInfo->clientSocket[clIndex]);
    clientInfo->clientSocket[clIndex] = 0;

    for (unsigned int i = 0; i < MAXCLIENTS; i++)
        if (clientInfo->clientSocket[i] != 0 && clientInfo->nickname[clIndex][0] != 0)
        {
            clientInfo->nickname[clIndex][strlen(clientInfo->nickname[clIndex])-1] = ' ';
            send (clientInfo->clientSocket[i], clientInfo->nickname[clIndex], strlen(clientInfo->nickname[clIndex]), 0);
            send (clientInfo->clientSocket[i], MEMQUIT, strlen(MEMQUIT), 0);
        }

    memset (clientInfo->nickname[clIndex], 0, NICKLEN);
}
//--------------------------------------------------------
/*
 * setting user first message as a nickname
 * format - <nick>
 * also, prints information about client
 */
void clientNickSet (ClientInfo *clientInfo, int clIndex, char *nickname)
{
    for (unsigned int i = 0; i < NICKLEN; i++)
    {
        if (i == 0)
        {
            clientInfo->nickname[clIndex][0] = '<';
            continue;
        }
        else if (*nickname == '\n')
        {
            clientInfo->nickname[clIndex][i] = '>';
            clientInfo->nickname[clIndex][i+1] = '\0';
            printf("nick set: ip: %s port: %i nick: %s\n", inet_ntoa (clientInfo->client.sin_addr), ntohs (clientInfo->client.sin_port), clientInfo->nickname[clIndex]);
            break;
        }
        else
            clientInfo->nickname[clIndex][i] = *nickname++;
    }
}
//--------------------------------------------------------
/*
 * Echo valid users about new client success registration
 * sends <new member>nickname
 */
void newChatMemberNotifier (ClientInfo *clientInfo, char *nickname)
{
    for (unsigned int i = 0; i < MAXCLIENTS; i++)
        if (clientInfo->clientSocket[i] != 0)
        {
            send (clientInfo->clientSocket[i], NEWMEM, strlen(NEWMEM), 0);
            send (clientInfo->clientSocket[i], nickname, strlen(nickname), 0);
        }

    memset (nickname, 0, strlen(nickname));
}
