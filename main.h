#ifndef _MAIN
#define _MAIN


#define __WINDOWS
//#define __LINUX

#ifdef __WINDOWS
#include <winsock.h>
#endif
#ifdef __LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
void closesocket(int socket) { close(socket); }
#endif

#ifdef DEBUG
#define _PRINTF printf
#define _STOP getchar();
#else
#define _PRINTF if (0) printf
#define _STOP
#endif



#include <stdio.h>
#include <string.h>
#include <ctype.h>

#endif

