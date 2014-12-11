// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//#define DEBUG
#include "main.h"
#include "doc_html.h"

//-------------------------------------------------------------
CDoc_Html::CDoc_Html (void):title(NULL), body(NULL), head(NULL)
{
     _PRINTF("Debug: CDoc_Html();\n");
}

//-------------------------------------------------------------
CDoc_Html::CDoc_Html (char *buffer)
{
    _PRINTF("Debug: CDoc_Html(char *);\n");

    *this = CDoc_Html();
    body = buffer;
}

//-------------------------------------------------------------
CDoc_Html::~CDoc_Html (void)
{
    clean();
}

//-------------------------------------------------------------
void CDoc_Html::clean(void)
{
    _PRINTF("Debug: CDoc_Html::clean();\n");

    if (this->title) delete this->title;
    if (this->head) delete this->head;
    if (this->body) delete this->body;
}

//-------------------------------------------------------------
int CDoc_Html::getHttpCode(void) const
{
    if (head)
    {
        // go to first space char
        char *tmp = head;
        while (*tmp++ != ' ');

        char code[4] = {0};
        memcpy(code,tmp,3);
        return atoi(code);
    }
    return 0;
}

char* CDoc_Html::getHttpHead(const char *headline) const
{
    if (head)
    {
        char *tmp = strstr(head, headline);
        if (tmp)
        {
            // go to ':' char
            while (*tmp++ != ':');
            // pass blank chars
            while (*++tmp == ' ');

            int count = 0;

            // value of headline
            while (*tmp != ' ' && *tmp != '\r' && *tmp != '\n')
            {
                ++count;
                tmp ++;
            }

            //printf("%d < ",count);
            char *httpHead = new char[count+1];
            memcpy(httpHead,tmp-count,count);
            httpHead[count] = '\0';

            return httpHead;
        }

    }
    return NULL;
}

//-------------------------------------------------------------
void CDoc_Html::getFromUrl(const CAddress &adr)
{
    // connect by socket and get data

    // buffer to html data
    body = new char [1000000];
    body[0] = '\0';

    CSocket socket;

    _PRINTF("CDoc_Hhtml::getFromUrl( ");

    socket.get2dest(adr,body);

    //printf("buffer=%s\n",buffer);

    // copy header
    if (socket.headerBuffer)
    {
        head = new char[strlen(socket.headerBuffer)+1];
        strcpy(head,socket.headerBuffer);
        //printf("head=%s\n",head);
    }
}



//-------------------------------------------------------------
void CDoc_Html::print(void) const
{
    _PRINTF("CDocHtml: ");

    if (title)
        _PRINTF(" title=%s",title);

    _PRINTF("\n");

}
