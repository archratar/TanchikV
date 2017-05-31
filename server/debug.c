#include "debug.h"
//--------------------------------------------------------
void printFdSet (fd_set fdSet)
{
    printf("sock descs in fd_set :\n");
    for (unsigned int i = 0; i < fdSet.fd_count; i++)
        printf("%i %i %i\n",i, fdSet.fd_count, fdSet.fd_array[i]);
}
//--------------------------------------------------------
void printSockDescs (SOCKET *clientSockDescs)
{
    printf("sock descs in SOCKET[]: \n");
    for (int i = 0; i < MAXCLIENTS && *(clientSockDescs+i) != 0; i++)
        printf("%i %i\n", i, *(clientSockDescs+i));
}
//--------------------------------------------------------
void printServerInfo (void)
{
    struct hostent *pHost;
    struct in_addr **addressList;
    char hostName[MINBUF] = {0};

    gethostname (hostName, MINBUF);
    pHost = gethostbyname (hostName);
    addressList = (struct in_addr**)pHost->h_addr_list;

    printf ("server name  : %s\nserver ip    : %s\nport         : %i\n", hostName, inet_ntoa (*(*(addressList)+0)), PORT);
}
//--------------------------------------------------------
