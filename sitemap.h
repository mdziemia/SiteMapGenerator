// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#ifndef _CLASS_CSITEMAP
#define _CLASS_CSITEMAP

#include "address.h"
#include "sitegraph.h"

class CSitemap
{
    private:

        // initial address
        //CAddress address;

        // graph of sitemap
        CSiteGraph *graph;

        // * * * Options * * *

        // table of sitemap settings
        char ** Settings;

    public:

        CSitemap(void): graph(NULL)  { }
        CSitemap(const CAddress &adr);

        ~CSitemap(void);

        // * * * routines * * *
        void print(void) const;           // output graph of links to screen
        void create(char * = NULL);       // Create Sitemap - create graph of site with out to xml file

        void onlyLocal(char * = NULL);    // set to get only local links
        void exceptLinks(int no, char **array);   // except links from sitemap

};

#endif
