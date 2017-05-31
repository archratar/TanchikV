/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * No warranty is given;
 * author: Hlukhenkyi Vasil.
 */

#ifndef SERVER_TOOLS_H
#define SERVER_TOOLS_H

#include <winsock2.h>
#include <stdio.h>
//--------------------------------------------------------
#define TCP SOCK_STREAM //! protocol tcp
#define IPV4 AF_INET //! protocol ip ver 4
#define PORT 666 //! server listening port
//--------------------------------------------------------
//! value used in "select" function
#define TIMEVAL_SEC 0
#define TIMEVAL_USEC 100000
//--------------------------------------------------------
#define BUFLEN 666 //! maximum length of message
#define MINBUF 30 //! used in "printServerInfo"
#define MAXCLIENTS 63 //! maximum number of clients, that can be served by server at one time
#define TRUE 1 //! used in "select" function
#define NICKLEN 20 //! maximum nickname length
#define GREETINGS "Hello, to start chating, input your nickname, max chars in nick - 20\n" //! server greetings to new client
#define NEWMEM "<new member>" //! regular expression, used when new client connected in chat room
#define MEMQUIT "quit>\n" //! regular expression, used when client quit chat room

//--------------------------------------------------------
/*! struct stored server information in one place
 * wsa          - windows socket API structure for geting everything work
 * serverSocket - server socket
 * fdServer     - socket descriptor structure
 * addrlen      - length of address, used in "getpeername" function
 * server       - structure stored information about ip and port
 * timeOut      - structure used in "select" function
 */
typedef struct SERVER
{
    WSADATA wsa;
    SOCKET serverSocket;
    fd_set fdServer;
    int addrlen;
    struct sockaddr_in server;
    struct timeval timeOut;
} ServerInfo;
//--------------------------------------------------------
/*! struct stored client information in one place
 * clientSocket - array of clients sockets.
 * nickname     - array of clients nicknames.
 * fdClient     - sockets descriptors structure
 * addrlen      - length of address, used in "getpeername" function
 * client       - structure stored information about ip and port
 * timeOut      - structure used in "select" function
 */
typedef struct CLIENT_INFO_TABLE
{
    SOCKET clientSocket[MAXCLIENTS];
    char nickname[MAXCLIENTS][NICKLEN];
    fd_set fdClient;
    int addrlen;
    struct sockaddr_in client;
    struct timeval timeOut;
} ClientInfo;
//--------------------------------------------------------

/*! serverInit
 * Initializing serverInfo structure.
 * If some error - exit
 *
 * /return [if okay ] initialized serverInfo structure
 *         [if error] exit with error code
 */
ServerInfo serverInit(void);
//--------------------------------------------------------
/*! bindListen
 * binds and set port to listening mode
 *
 * /param[in]  serverInfo - pointer to serverInfo array
 *
 * /return     [0] - okay
 *             [ ] - last error
 */
int bindListen (ServerInfo *serverInfo);
//--------------------------------------------------------
/*! acceptConnection
 * If new client is knocking accepts the connection
 * If there are already maximum clients in room, does nothing
 *
 * /param[in]  serverInfo - pointer to serverInfo structure
 * /param[in]  clientInfo - pointer to clientInfo structure
 */
void acceptConnection(ServerInfo *serverInfo, ClientInfo *clientInfo);
//--------------------------------------------------------
/*! atYourService
 * main chat service, handle clients messages
 *
 * /param[in] clientInfo - pointer to clientInfo structure
 * /param[in] message    - pointer to incoming message
 */
void atYourService (ClientInfo *clientInfo, char *message);
//--------------------------------------------------------
/*! echoMsg
 * echo message to exists clients
 *
 * /param[in] clientInfo - pointer to clientInfo structure
 * /param[in] message    - pointer to incoming message
 * /param[in] clIndex    - index of client socket sended message
 */
void echoMsg (ClientInfo *clientInfo, char *message, unsigned int clIndex);
//--------------------------------------------------------
/*! clientInit
 * initializing clientInfo structure
 *
 * /param[in] clientInfo - pointer to clientInfo structure
 */
void clientInit (ClientInfo *clientInfo);
//--------------------------------------------------------
/*! fileDescsSet
 * initializing existing clients sockets descriptors to fd_set structure
 *
 * /param[in] clientInfo - pointer to clientInfo structure
 */
void fileDescsSet (ClientInfo *clientInfo);
//--------------------------------------------------------
/*! onClientDisconnect
 * deinitializing disconnected clients info
 *
 * /param[in] clientInfo - pointer to clientInfo structure
 * /param[in] clIndex    - client index in sockets array
 */
void onClientDisconnect (ClientInfo *clientInfo, int clIndex);
//--------------------------------------------------------
/*! clientNickSet
 * initializing client nickname
 *
 * /param[in] clientInfo - pointer to clientInfo structure
 * /param[in] clIndex    - client index in sockets array
 * /param[in] nickname   - pointer to string with nickname
 */
void clientNickSet (ClientInfo *clientInfo, int clIndex, char *nickname);
//--------------------------------------------------------
/*! newChatMemberNotifier
 * informs clients about new client
 *
 * /param[in] clientInfo - pointer to clientInfo structure
 * /param[in] nickname   - pointer to string with nickname
 */
void newChatMemberNotifier (ClientInfo *clientInfo, char *nickname);
//--------------------------------------------------------
#endif // SERVER_TOOLS_H
