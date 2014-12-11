#ifndef _CLASS_CADDRESS
#define _CLASS_CADDRESS

#define _TO_FILE    1
#define _TO_SCREEN  2
#define _TO_BUFFER  3

#define _BUF_LARGE  524288
#define _BUF_RECV   524288
#define _BUF_SMALL  256


// id of extensions :
// 0 - no define
// 1 - index file
// 2 - htm, html
// 3 - php, php3, php4, ...
// 4 - cgi
// 5 - asp
// 6 - url

class CAddress {

    private:

        // strings of addres
        char *protocol;                 // i.e. http
        char *domain;                   // domain.com
        char *dir;                      // /dir/
        char *document;                 // index.html

        unsigned char ext;              //type of file i.e. .html

    public:

        CAddress(void);
        CAddress(const CAddress &adr);

        CAddress(char* string, CAddress* = NULL);

        ~CAddress(void)
        {
            clean();
        }



    private:

        void sepDirDoc(char* text);
        void getExtension(char* document);

    public:

        char *_protocol (void) const { return protocol; }
        char *_domain (void) const { return domain; }
        char *_dir (void) const { return dir; }
        char *_document (void) const { return document; }
        unsigned char _ext (void) const { return ext; }

        operator char*();

        bool operator ==(const CAddress &adr);
        CAddress * operator=(const CAddress &adr);

        char* to_string(char *dest) const;

        bool localAddress(char *adr) const;
        bool ishttp(void) const;

        int  parse(char* address, CAddress* = NULL);
        void print(short num = _TO_SCREEN, char* = NULL) const;
        void clean(void);
        int _strlen(void);


};
#endif
