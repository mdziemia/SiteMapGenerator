#ifndef _CLASS_CDOC_HTML
#define _CLASS_CDOC_HTML

#include "address.h"
#include "socket.h"


class CDoc_Html
{
    private:

        char *title;
        char *body;
        char *head;

    public:

        CDoc_Html (void);

        // create object from string
        CDoc_Html (char *buffer);
        ~CDoc_Html (void);

        // * * * routines * * *
        void getFromUrl(const CAddress &adr);       // connect by socket and get html data
        int  getHttpCode(void) const;               // get http code from head (i.e. 404)
        char* getHttpHead(const char *) const;      // get value of certain head line

        void clean(void);                           // wash & clean
        void print(void) const;

        char *_body(void) const { return body; }
};

#endif
