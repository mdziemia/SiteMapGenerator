#ifndef _CLASS_CLINKSEARCH
#define _CLASS_CLINKSEARCH

#include "address.h"
#include "sitegraph.h"



class CLinkSearch {

    // Searching by KMP method
    // thanks http://luq.sloneczna.one.pl/udst_pliki/wyszukiwanie_wzorca.pdf
    // to explain me this method

    private:
        char *Buffer;
        int* FunPi;                 //
        char* T;                    // text
        long n;                     // length of text
        char* P;                    // pattern
        int m;                      // length of pattern

        CSiteGraph *graph;          // graph structure
        CAddress *adr_relative;     // to get relative address

    public:


        // * * * Options * * *
        bool _fl_local;
        char* _local_pattern;

        // * * * routines * * * *

        CLinkSearch(void) { *this =  CLinkSearch(NULL); }       // only for settings!

        CLinkSearch(char *buffer, CAddress* adr_relative = NULL);
        ~CLinkSearch(void);

        void CreateFunPi(void);
        void Search();
        int  GetAddress(char* text);
        long File2Str(char* file, char* buffer);
        CSiteGraph* SearchLinks(void);
        void OutputData(CAddress* adr);

        void clean(void);           // wash & clean

};



#endif
