#ifndef DEBUG_H
#define DEBUG_H

#include "server_tools.h"

/*!
 * prints socket descriptors in fd_set to the console
 *
 * /param[in]  fdSet - fd_set structure
 */
void printFdSet (fd_set fdSet);

/*!
 * prints socket descriptors in array of sockets to the console
 *
 * /param[in]  clientSocketDescs - pointer to array of sockets
 */
void printSockDescs (SOCKET *clientSockDescs);

/*!
 * prints localhost name, ip and binded port to the console
 */
void printServerInfo (void);

#endif // DEBUG_H
