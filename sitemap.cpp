//-------------------------------------------------------------
//#define DEBUG
#include "main.h"
#include "sitemap.h"
#include "link_search.h"

//-------------------------------------------------------------
CSitemap::CSitemap(const CAddress &adr)
{
    _PRINTF("Debug: CSitemap(&adr);\n");

    // default values
    *this = CSitemap();

    // settings
    Settings = new char* [4];

    // only inside links
    Settings[0] = new char(0);
    Settings[1] = NULL;

    // except links
    Settings[3] = new char(0);
    Settings[4] = NULL; //(char *)(new char*[2]);

    // xml file out
    Settings[5] = new char[100];
    strcpy(Settings[5],"sitemap.xml");

    // initial address
    graph = new CSiteGraph(adr, Settings);
    graph -> first();
}

//-------------------------------------------------------------
CSitemap::~CSitemap(void)
{
    _PRINTF("Debug: ~CSitemap();\n");

    // clean graph
    if (graph)
    {
        graph->clean();
        delete graph;
    }

}

//-------------------------------------------------------------
void CSitemap::exceptLinks(int no, char **array)
{
    if (array)
    {
        Settings[3] = new char(no);
        Settings[4] = (char *)array;
    }
}

//-------------------------------------------------------------
void CSitemap::onlyLocal(char *pattern)
{
    // set to only local links
    *Settings[0] = 1;

    if (!pattern && graph)
    {
        CAddress *root_adr = ((graph->_data())->_address());
        Settings[1] = new char[root_adr->_strlen()];
        root_adr->to_string(Settings[1]);

    }
    else if (pattern)
    {
        Settings[1] = new char[strlen(pattern)+1];
        strcpy(Settings[1],pattern);
    }
}

//-------------------------------------------------------------
void CSitemap::print(void) const
{
    if (graph)
    {
        printf("-------------------------\n");
        printf("- graph print -----------\n");
        printf("-------------------------\n");

        graph -> print();
        graph -> print_graph();
    }
}

//-------------------------------------------------------------
void CSitemap::create(char *fileout)
{
    _PRINTF("Debug: CSitemap::Create()\n");

    if (graph)
    {
        // settings of searching
        //char buffer[256];
        CAddress *i_adr = (graph->_data())->_address();

        // get address from first link
        //sprintf(buffer,"%s://%s%s%s",i_adr->_protocol(),i_adr->_domain(),i_adr->_dir(),i_adr->_document());

        // get sub-links from main site
        //CLinkSearch search;
        //search._fl_local = !_fl_outside;
        //search._local_pattern = new char[strlen(buffer)+1];
        //strcpy(search._local_pattern,buffer);

        if (fileout)
        {
            delete [] Settings[5];
            Settings[5] = new char[strlen(fileout)+1];
            strcpy(Settings[5], fileout);
        }

        // get content ... for first link (root of sitegraph)
        graph -> getLink();
    }
}
