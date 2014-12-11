//#define DEBUG

#include "main.h"
#include "socket.h"
// static members

// load socket module ones
WSAData CSocket::wsaData;


//-------------------------------------------------------------
CSocket::CSocket(void):headerBuffer(NULL), phraseBuffer(NULL)
{
    // if it is first use, we need to load wsocket dlls
    if (!wsaData.wVersion)
    {
        if (initWSocket() == SOCKET_ERROR) {
            printf("** Couldn't Start Up Winsock!\n");
        }
        else
        {
            printf("++ Winsock started!\n");
        }
    }
}

//-------------------------------------------------------------
int CSocket::initWSocket(void)
{
	return WSAStartup(MAKEWORD(2, 2), &wsaData);
}

//-------------------------------------------------------------
int CSocket::connectX(unsigned short port, const char* server_name)
{
    struct hostent *host_entry;
    struct sockaddr_in server;

    // does socket inite
     if ((ConnectionID = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR) {
        printf("** Socket doesn't exists!\n");
        _STOP
        return -1;
    }

    _PRINTF("CSocket::connectX(%d, %s);\n",port, server_name);


    // setup the host entry
	if ((host_entry = gethostbyname(server_name)) == NULL) {
		printf("** Couldn't find host %s !\n",server_name);
        _STOP
		return -1;
	}

	// fill in the server socket info
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = *(unsigned long*) host_entry->h_addr;

	// connect to the server
	if (connect(ConnectionID, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("** Error connecting to server!\n");
		_STOP
		return -1;
	}

	return 1;

}

//-------------------------------------------------------------
int CSocket::sendStr(char *str, unsigned long size)
{
    return send(ConnectionID, str, size, 0);
}

//-------------------------------------------------------------
int CSocket::recvStr(char *buffer, unsigned long size)
{
    return recv(ConnectionID, buffer, size, 0);
}

//-------------------------------------------------------------
int CSocket::cblank(char *buffer)
{
    int ret = 0;
    if (buffer)
    {
        while (buffer[ret] == '\n' || buffer[ret] == '\r')
        {
            ++ret;
        }
    }
    return ret;
}

//-------------------------------------------------------------
int CSocket::cchunked(char *buffer)
{
    int ret = 0;

    if (buffer)
    {
        // pass chunk
        while (buffer[ret] != '\n' && buffer[ret] != '\r')
        {
            ++ret;
        }

        // pass blank chars
        while (buffer[ret] == '\n' || buffer[ret] == '\r')
        {
            ++ret;
        }
    }
    return ret;
}

//-------------------------------------------------------------
bool CSocket::ischunked(void)
{
    //Transfer-Encoding: chunked
    if (strstr(headerBuffer, "chunked")) return true;
    else return false;
}


//-------------------------------------------------------------
int CSocket::get2dest(const CAddress &adr, char *dest)
{
    // temporary size of send/recv packet or error
    unsigned long nBytes = 0;
    // total size of recv data
    unsigned long cBytes = 0;
    // where data begins
    unsigned long dBytes = 0;

    // bytes to pass
    unsigned long passBytes = 0;

    char outMessage[1024] = {0};
    char buffer[_BUF_RECV] = {0};
    bool _fl_chunked;

    if (dest == NULL)
    {
        dest = this->Buffer;
    }


    // connecting
	if (!adr.ishttp() || connectX(80,adr._domain()) < 1)
	{
        // close our socket
        closesocket(ConnectionID);
        return 1;
    }


    // message to http server
    sprintf(outMessage,"GET %s%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", adr._dir(), adr._document(), adr._domain());
    //printf("** %s",outMessage);

    // send the message to the server
    nBytes = sendStr(outMessage,strlen(outMessage)*sizeof(char));

    // check for errors
	if (nBytes == SOCKET_ERROR) {
		printf("** Send Failed!\n");
		_STOP
		return -1;
	}

	// get first packet with header
	nBytes = recvStr(buffer,_BUF_RECV-1);
	if (nBytes != SOCKET_ERROR)
	{
        // recv succesfull
        dBytes = getHeader(buffer);
        cBytes += dBytes;

        _fl_chunked = ischunked();
    }


    //printf("data begins from %d\n",dBytes);
    //printf("all data %d\n",nBytes);
    //printf("blank chars %d\n",cblank(buffer+dBytes));

    if (_fl_chunked)
        passBytes = cchunked(buffer+dBytes+cblank(buffer+dBytes)) + cblank(buffer+dBytes);
    else
        passBytes = cblank(buffer+dBytes);

    //printf("passed chars %d\n",passBytes);

    // copy to dest only data, without blank chars, without header
    memcpy(dest, buffer+dBytes+passBytes, nBytes-dBytes-passBytes);



    // get rest of data
    while(nBytes > 0 && nBytes != SOCKET_ERROR && cBytes < _BUF_LARGE)
	{
        nBytes = recvStr(buffer,_BUF_RECV-1);
        // if buffer is full
        if (cBytes + nBytes >= _BUF_LARGE) {
            buffer[_BUF_LARGE - cBytes] = '\0';
        }
        cBytes += nBytes;
        buffer[nBytes] = '\0';

        if (_fl_chunked)
            passBytes = cchunked(buffer);
        else
            passBytes = cblank(buffer);

        if (passBytes > 10) passBytes = 0;

        //printf("passed chars %d\n",passBytes);

        strcat(dest,buffer + passBytes - 1);
        //printf("\n------ %d ----------\n", strlen(buffer));
        //printf("%s",buffer);

        //printf(":: %ld \n",nBytes);

    }



    // close our socket
    closesocket(ConnectionID);
}

//-------------------------------------------------------------
int CSocket::getHeader(char* buffer)
{
    char *endOfBuffer,*find;
    int count=0;

    if ((endOfBuffer = strstr(buffer,"\r\n\r\n")) != NULL)
    {
        find = buffer;
        while (find++ != endOfBuffer) count++;

        this->headerBuffer = new char[count+1];
        if (this->headerBuffer)
        {
            find = buffer;
            for (int i=0; i<count; i++)
            {
                this->headerBuffer[i] = tolower(buffer[i]);
            }
            this->headerBuffer[count] = '\0';
        }
        else
        {
            this->headerBuffer = NULL;
            count = 0;
        }

    }
    return count;
}

//-------------------------------------------------------------
int CSocket::headerAction(void)
{
    char *phrase, *tmp;
    int count=0;

    if (this->headerBuffer)
    {

        // location action
        if (this->headerBuffer && (phrase = strstr(this->headerBuffer,"location:")) != NULL)
        {

            if (this->phraseBuffer) delete [] this->phraseBuffer;

            // pass any space
            tmp = phrase+9;
            while (isspace(*tmp++));
            tmp--;

            count = strlen(tmp);
            this->phraseBuffer = new char[count+1];

            for (int i=0; i<count; i++)
            {
                this->phraseBuffer[i] = tmp[i];
            }
            this->phraseBuffer[count] = '\0';


            return 1;
        }

    }
    return 0;
}
