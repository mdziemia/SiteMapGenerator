#ifndef _CLASS_CSITEGRAPH
#define _CLASS_CSITEGRAPH

//
// class which contains data of each graph's vertex
//

#include "address.h"
#include "doc_html.h"





class CSiteLink
{
    private:

        CAddress *address;       // address of web document

        int code;               // code of http document i.e. 404, 200 and so on...
                                // code = 0 means we didnt check this link yet

    public:

        CSiteLink(void): code(0), address(NULL) { }
        CSiteLink(const CSiteLink &source);
        CSiteLink(const CAddress &adr);

        ~CSiteLink(void) { }

        // * * * routines * * *

        void print(void) const;
        CAddress* _address(void) const { return address; }


        void clean(void);       // wash & clean

        // * * * friends
        friend class CSiteGraph;
};

//
// class which describes graph of sitemap
//

class CSiteGraph
{
    private:

        CSiteGraph *next;   // next element of list on the same level
        CSiteGraph *list;   // ptr to first element of sublist

        CSiteGraph *root;   // ptr to first element of graph (initial link)

        int level;          // level of reccurency - initial = 0

        CSiteLink *data;

        char ** Settings;   // table of Sitemap settings

    public:

        CSiteGraph(void):next(NULL), list(NULL), root(NULL), level(-1), data(NULL), Settings(NULL) { }
        CSiteGraph(const CSiteLink &source, char ** settings);
        CSiteGraph(const CAddress &adr, char ** settings);

        ~CSiteGraph(void) { }

        // * * routines * *

        // set actual vertex as root
        CSiteGraph* first(void) { root = this; level = 0; return this; }

        // add new vertex to sublist
        CSiteGraph* add(const CSiteLink &data);

        // delete duplicate links
        CSiteGraph* addUniqueLinks(CSiteGraph* list);
        // check if link exists in graph of sitemap
        bool existsLink(const CAddress *templ);
        // check if link is on except list
        bool exceptLink(char **Settings);


        // output members of class
        void print(void) const;
        void print_graph(void) const;
        void print2file(void) const;

        int _level(void) const { return level; }
        CSiteLink* _data(void) const { return data; }
        char **_Settings(void) const { return Settings; }
        CSiteGraph *_list(void) const { return list; }
        void _setlist(CSiteGraph *ptr) { list = ptr; }


        // wash & clean
        void clean(void);

        // * * * * * network

        // Create SiteMAP
        void getLink(void);

};

#endif
