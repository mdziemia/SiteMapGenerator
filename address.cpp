// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#include "main.h"
#include "address.h"

extern void str_replace(char* source, char from, char to);

void remove_blank_chars(char *string)
{
    char blank[3];
    sprintf(blank,"\n");

    if (strstr(string,blank))
    {
        char *tmp = string;
        char *good = new char[strlen(string)+1];

        int i=0, j=0;

        for (i=0; i<strlen(string); i++)
        {
            if (tmp[i] != '\n' && tmp[i] != '\r')
            {
                good[j] = tmp[i];
                ++j;
            }
        }
        good[j] = '\0';
        strcpy(string,good);
        delete [] good;

    }
}

//-------------------------------------------------------------
CAddress::CAddress(void)
{
    protocol = NULL;
    domain = NULL;
    dir = NULL;
    document = NULL;
    ext = 0;
}

//-------------------------------------------------------------
CAddress::CAddress(const CAddress &adr)
{
    this->protocol = new char[strlen(adr.protocol)+1];
    strcpy(this->protocol,adr.protocol);

    this->domain = new char[strlen(adr.domain)+1];
    strcpy(this->domain,adr.domain);

    this->dir = new char[strlen(adr.dir)+1];
    strcpy(this->dir,adr.dir);

    this->document = new char[strlen(adr.document)+1];
    strcpy(this->document,adr.document);

    this->ext = adr.ext;

}

//-------------------------------------------------------------
CAddress::CAddress(char* string, CAddress* relative)
{
    *this = CAddress();
    parse(string, relative);
}

//-------------------------------------------------------------
int CAddress::parse(char* address, CAddress *refer)
{
    char *prot, *prol, *tmp, *actual = address;
    int count = 0;
    char buffer[_BUF_SMALL] = {0};
    bool _unprot = false;

    // some special events, which we dont want
    if ((prot = strstr(address,"javascript:")) == address) {
        return 0;
    }

    // PROTOCOL NAME ******
    if ((prot = strstr(address,"://")) != NULL)
    {
        tmp = actual;
        while (tmp != prot && count < _BUF_SMALL)
        {
            buffer[count++] = *tmp++;
        }
        buffer[count] = '\0';

        // initialize mem to protocol name
        this->protocol = new char[count+1];
        // save protocol name
        strcpy(this->protocol,buffer);
        actual = tmp + 3;
    }
    else if (refer) // we take protocol name from refer
    {
        _unprot = true;
        this->protocol = new char[strlen(refer->protocol)+1];
        strcpy(this->protocol,refer->protocol);
        actual = address;
    }
    else // maybe someone forgot add protocol name to url
    {
        _unprot = true;
        this->protocol = new char[5];
        strcpy(this->protocol,"http");
        actual = address;
    }

    // DOMAIN NAME ***
    // but at first we looking for dir or script

    prot = strstr(actual,"/");
    prol = strstr(actual,"?");
    if ((prot || prol) && !_unprot)
    {

        if (prot != NULL && prol != NULL) {
            prot = (prot < prol) ? prot : prol; // which one is closer
        } else {
            prot = (prot != NULL) ? prot : prol; // maybe one of them is null
        }

        tmp = actual; count = 0;
        while (tmp != prot && count < _BUF_SMALL)
        {
            buffer[count++] = *tmp++;
        }
        buffer[count] = '\0';

        // save domain name
        this->domain = new char[count+1];
        strcpy(this->domain,buffer);

        actual = tmp;
    }
    else
    {
        // without dir or document or script
        if (refer && _unprot)
        {
            // domain name from refer
            this->domain = new char[strlen(refer->domain)+1];
            strcpy(this->domain,refer->domain);

        }
        else
        {
            // we have only domain name
            this->domain = new char[strlen(actual)+1];
            strcpy(this->domain,actual);

            this->dir = new char[2];
            strcpy(this->dir,"/");

            this->document = new char[1];
            *this->document = '\0';

            this->ext = 1; // index file

            return 1;
        }
    }


    // DIR AND DOCUMENT ***
    // if we back to higher dir or lower or we stay at actual dir i.e. href=index.html
    if (*actual != '/')
    {
        // if refer has dir, add it
        memset(buffer,_BUF_SMALL,0);
        if (refer) {
            strcat(buffer,refer->dir);
        }
        strcat(buffer,actual);

        actual = buffer;
    }
    else // absolute dir path  i.e.   /dir/...
    {
        //actual = actual;
    }

    // get dir and/or document
    sepDirDoc(actual);

    // get type of file
    getExtension(this->document);

    // looking for blank chars
    char blank[3];
    sprintf(blank,"\n");

    if (strstr(this->protocol,blank) || strstr(this->domain,blank) || strstr(this->dir,blank) || strstr(this->document,blank))
    {
        remove_blank_chars(this->protocol);
        remove_blank_chars(this->domain);
        remove_blank_chars(this->dir);
        remove_blank_chars(this->document);
    }

    return 1;

}

//-------------------------------------------------------------
void CAddress::sepDirDoc(char* text)
{
    char* docp = strstr(text,"/");
    char* docq = strstr(text,"?");
    char buffer[_BUF_SMALL] = {0};

    int count=0;

    if (docp) {

        char *tmp = text;
        // go to last slash i.e.   dir1/dir2/.../lastdir/
        while (strstr(tmp+1,"/") && (docq == NULL || strstr(tmp+1,"/") < docq))
        {
            tmp = strstr(tmp+1,"/");
        }
        tmp++; // add slash to dir , not to document

        count = 0;
        docq = text;
        while (docq != tmp && count < _BUF_SMALL)
        {
            buffer[count++] = *docq++;
        }
        buffer[count] = '\0';

        // dir
        this->dir = new char[strlen(buffer)+1];
        strcpy(this->dir,buffer);

        //document
        this->document= new char[strlen(tmp)+1];
        strcpy(this->document,tmp);
    }
    else
    {
        // without dir
        this->dir = new char[2];
        strcpy(this->dir,"/");

        // document
        this->document = new char[strlen(text)+1];
        strcpy(this->document,text);
    }
}

//-------------------------------------------------------------
void CAddress::getExtension(char* document)
{
    char buffer[10];
    int count = 0;
    char* actual = document;
    char* docq = strstr(actual,"?");
    char* docd = strstr(actual,".");

    // do we have any dot?
    if (!docd || (docq && docq < docd))
    {
        this->ext = 1;      // 1 - index file
    }
    else
    {
        // we have dot and/or sript, go to last dot before '?'
        actual = docd+1;
        while (*actual != '\0' && *actual != '?')
        {
            if (*actual == '.') {
                docd = actual;
            }
            actual++;
        }

        // get string between dot and '?' or end of string
        actual = docd+1;
        while (*actual != '\0' && *actual != '?' && count < 10)
        {
            buffer[count++] = tolower(*actual++);
        }
        buffer[count] = '\0';

        if (strcmp(buffer, "htm") == 0 || strcmp(buffer, "html") == 0) {
            this->ext = 2;
            return;
        }
        if (strlen(buffer) <= 4 && strstr(buffer,"php") || strcmp(buffer, "phtm") == 0 || strcmp(buffer, "phtml") == 0) {
            this->ext = 3;
            return;
        }
        if (strcmp(buffer, "cgi") == 0)
        {
            this->ext = 4;
            return;
        }
        if (strcmp(buffer, "asp") == 0)
        {
            this->ext = 5;
            return;
        }
        if (strcmp(buffer, "url") == 0)
        {
            this->ext = 6;
            return;
        }

    }
}

//-------------------------------------------------------------
bool CAddress::ishttp(void) const
{

    if (this->protocol && strcmp(this->protocol,"http") == 0 && this->domain && this->ext > 0)
        return true;
        else return false;
}

//-------------------------------------------------------------
void CAddress::print(short num, char *filename) const
{
    FILE *f;
    switch (num)
    {
        case _TO_FILE: // write to file
            if ((f = fopen(filename,"a")) != NULL)
            {
                if (!ishttp()) fprintf(f,"###### ");
                fprintf(f,"%s - %s - %s - %s \n", this->protocol, this->domain, this->dir, this->document);
                fclose(f);
            }
            break;
        case _TO_SCREEN: // print on the screen
            if (!ishttp()) printf("###### ");
            printf("%s%s%s type=%d \n", this->domain, this->dir, this->document, this->ext);

            break;
    }


}

//-------------------------------------------------------------
void CAddress::clean(void)
{
    if (this->protocol) delete [] this->protocol;
    this->protocol = NULL;

    if (this->domain) delete [] this->domain;
    this->domain = NULL;

    if (this->dir) delete [] this->dir;
    this->dir = NULL;

    if (this->document) delete [] this->document;
    this->document = NULL;

}

//-------------------------------------------------------------
bool CAddress::operator==(const CAddress &adr)
{

    if (strcmp(this->domain,adr.domain) != 0) return false;
    if (strcmp(this->dir,adr.dir) != 0) return false;
    if (strcmp(this->document,adr.document) != 0) return false;
    return true;
}

//-------------------------------------------------------------
CAddress * CAddress::operator=(const CAddress &adr)
{

    if (adr.protocol)
    {
        this->protocol = new char[strlen(adr.protocol)];
        strcpy(this->protocol,adr.protocol);
    }
    if (adr.domain)
    {
        this->domain = new char[strlen(adr.domain)];
        strcpy(this->domain,adr.domain);
    }
    if (adr.dir)
    {
        this->dir = new char[strlen(adr.dir)];
        strcpy(this->dir,adr.dir);
    }
    if (adr.document)
    {
        this->document = new char[strlen(adr.document)];
        strcpy(this->document,adr.document);
    }

    this->ext = adr.ext;

    return this;

}

//-------------------------------------------------------------
CAddress::operator char*()
{
    int len = 0;
    if (this->protocol) len += strlen(this->protocol);
    if (this->domain) len += strlen(this->domain);
    if (this->dir) len += strlen(this->dir);
    if (this->document) len += strlen(this->document);

    char *tmp = new char [len+1];

    sprintf(tmp,"%s://%s%s%s",this->protocol, this->domain, this->dir, this->document);
    return tmp;

}

//-------------------------------------------------------------
char * CAddress::to_string(char *dest) const
{
    sprintf(dest,"%s://%s%s%s",this->protocol, this->domain, this->dir, this->document);
    return dest;
}

//-------------------------------------------------------------
bool CAddress::localAddress(char *adr) const
{

    // we can also compare only domain to check if adress is local
    // but now we compare strings
    //if (strcmp(this->domain, adr.domain) == 0) return true;

    char adr_actual[256];
    this->to_string(adr_actual);

    if (strstr(adr_actual, adr)) return true;

    return false;
}

//-------------------------------------------------------------
int CAddress::_strlen(void)
{
    int ret = 4;  // for  ://  and '\0' char
    if (this->protocol) ret += strlen(this->protocol);
    if (this->domain) ret += strlen(this->domain);
    if (this->dir) ret += strlen(this->dir);
    if (this->document) ret += strlen(this->document);

    return ret;
}
