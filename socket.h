#ifndef _CLASS_CSOCKED
#define _CLASS_CSOCKED


#include "address.h"



class CSocket
{
    private:
        static WSAData wsaData;

        // ID of connection
        int ConnectionID;

        // buffer
        char* Buffer;

        // * * * private routines * * * *

        // create connection with server_name on port
        int connectX(unsigned short port, const char* server_name);

        // receive data from soket and put it to buffer
        int recvStr(char *buffer, unsigned long size);

        // send data to socket
        int sendStr(char *str, unsigned long size);

        // only to inicialize winsocket, we use this only once
        // to load dlls ...
        int initWSocket(void);

        // get number of blank chars to delete
        int cblank(char *buffer);
        // get number of chunked chars to delete
        int cchunked(char *buffer);

        // check if transfer-encoding is chunked
        bool ischunked(void);

    public:

        // buffer to header
        char* headerBuffer;
        // buffer to content?
        char* phraseBuffer;


        CSocket(void);
        ~CSocket(void) { }

        // public routines


        // get html and put into buffer
        int get2dest(const CAddress &adr, char* dest = NULL);

        // get header of document from buffer
        int getHeader(char* buffer);

        // check if we have to go to another url
        // >> redirected
        int headerAction(void);


};

#endif
