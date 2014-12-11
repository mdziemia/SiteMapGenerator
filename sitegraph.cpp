// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//#define DEBUG
#include "main.h"
#include "sitegraph.h"
#include "link_search.h"

//-------------------------------------------------------------
CSiteGraph::CSiteGraph(const CSiteLink &source, char ** settings)
{
    _PRINTF("Debug: CSiteGraph(CSiteLink &);\n");
    *this = CSiteGraph();
    this->data = new CSiteLink(source);
    this->Settings = settings;
}

//-------------------------------------------------------------
CSiteGraph::CSiteGraph(const CAddress &adr, char ** settings)
{
    _PRINTF("Debug: CSiteGraph(CAddress &);\n");

    *this = CSiteGraph();
    this->data = new CSiteLink(adr);
    this->Settings = settings;
}

//-------------------------------------------------------------
void CSiteGraph::clean(void)
{
    _PRINTF("Debug: CSiteGraph::clean();\n");
    if (this->data) data->clean();

    // NOT COMPLETE !!!!!
    // we must write clean for graph
    //if (this->next) next->clean();
}


//-------------------------------------------------------------
CSiteGraph* CSiteGraph::add(const CSiteLink &data)
{
    CSiteGraph *last;

    if (!list)
    {
        // first vertex on sublist
        list = new CSiteGraph(data, Settings);
        last = list;
    }
    else
    {
        // we must go to last vertex on sublist
        last = list;
        while (last->next)
        {
            last = last->next;
        }

        last->next = new CSiteGraph(data, Settings);
        last = last->next;
    }

    // next level
    last -> level = this -> level + 1;
    last -> root = this -> root;

    //data.print();

    return last;
}



//-------------------------------------------------------------
CSiteGraph* CSiteGraph::addUniqueLinks(CSiteGraph* list)
{
    // go through each links from searching
    CSiteGraph *act = list;
    CSiteGraph *ptr;
    CSiteGraph *new_link;
    CSiteGraph *first_new_link = NULL;
    bool fl_add_link, fl_go_through_link;

    // at first we add to graph new, unique links,
    // and after we by RECURRENCY go through each of new links (which was just added)

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // * * * * * WRITE TO XML FILE
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    while (act)
    {
        fl_add_link = true;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // actual link doesn't exists , so save it, or do nothing
        if (root->existsLink((act->_data())->_address()))
        {
            fl_add_link = false;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // check if actual link isn't on the except links
        if (*Settings[3] && fl_add_link)
        {
            fl_add_link = !act->exceptLink(Settings);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (fl_add_link)
        {
            // add new link to actual vertex of graph
            CSiteLink newLink(*(act->_data())->_address());
            new_link = add(newLink);

            //(*(act->_data())->_address()).print();
            // mark first new link, if we'll add all new links,
            // we started by this first_new_link reccurency

            if (!first_new_link) first_new_link = new_link;

            // write actual link to XML file
            new_link -> print2file();
        }

        // and delete from list, to free mem and bla bla bla
        ptr = act;
        act = act->next;
        ptr -> clean();
    }

    // ok now, when we added all new unique links,
    // we can enter to this new links by RECURRENCY


    char buffer[256];
    ((root->_data())->_address())->to_string(buffer);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // * * * * * RECURRENCY  ENTER
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    act = first_new_link;
    while (act)
    {
        fl_go_through_link = true;

        // - - - - - - - - - - - - - - -
        // go to only local links
        if (!((act->_data())->_address())->localAddress(buffer))
        {
            fl_go_through_link = false;
        }

        // - - - - - - - - - - - - - - -
        // we can enter to actual link
        if (fl_go_through_link)
        {
            act -> getLink();
        }

        act = act->next;
    }

    return NULL;
}

//-------------------------------------------------------------
void CSiteGraph::getLink(void)
{
    _PRINTF("Debug: CSiteGraph::getLink();\n");

    // new html object
    CDoc_Html doc_html;

    _PRINTF("CSiteGraph::getLink(); \n");

    // get content of link
    doc_html.getFromUrl(*data->address);

    // get http code
    data->code = doc_html.getHttpCode();

    // print to screen
    data->print();

    //char **s = (char **)(Settings[4]);
    //printf(" string = %s\n",s[0]);

    switch (data->code)
    {
        case 200: // ok

            // let's find links
            {
                CLinkSearch links(doc_html._body(), root->data->_address());

                // get settings of sitemap
                links._fl_local = *Settings[0];
                links._local_pattern = Settings[1];

                // get list of links
                CSiteGraph *list_of_links = links.SearchLinks();

                // add only unique links, which don't exist on the graph yet
                addUniqueLinks(list_of_links);

            }
            break;

        case 302: // location:

            // set new address and do it again
            data->address = new CAddress(doc_html.getHttpHead("location"), root->data->_address());
            this->getLink();

            break;

        case 404: // not found

            break;
    }

}

//-------------------------------------------------------------
void CSiteGraph::print(void) const
{
    _PRINTF("i'm=%p  level=%d  next=%p  list=%p  root=%p \n",this, level, next, list, root);
    data->print();
}

//-------------------------------------------------------------
void CSiteGraph::print_graph(void) const
{
    CSiteGraph *tmp = list;

    while (tmp)
    {
        tmp -> print();
        tmp -> print_graph();
        tmp = tmp->next;
    }
}

//-------------------------------------------------------------
void CSiteGraph::print2file(void) const
{
    CAddress *adr = data->_address();

    // function to obtain value of priority
    double priority = 0.022917*level*level*level*level - 0.2875*level*level*level + 1.22708*level*level - 2.2125*level + 2.25;

    if (level > 5)
        priority = 0.20;


    FILE *f;
    f = fopen(Settings[5],"a");
    fprintf(f,"<url>\n");
    fprintf(f,"  <loc>%s://%s%s%s</loc>\n",adr->_protocol(),adr->_domain(),adr->_dir(),adr->_document());
    fprintf(f,"  <priority>%.2f</priority>\n",priority);
    fprintf(f,"</url>\n");
    fclose(f);

}

//-------------------------------------------------------------
bool CSiteGraph::exceptLink(char **Settings)
{
    char **except_string =(char **)Settings[4];

    char actual[256];
    (data->_address())->to_string(actual);

    for (int i=0; i<*Settings[3]; ++i)
    {
        _PRINTF("string compares %s == %s \n",actual, except_string[i]);

        if (strcmp(actual, except_string[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------
bool CSiteGraph::existsLink(const CAddress *templ)
{
    CSiteGraph *act;

    //if (strstr(templ->_document(),"fotografia.html"))
    {
     //   templ->print();
     //   getchar();
    }


    if (this->data)
    {
        // check if actual element is equal to template
        if (*(this->data->_address()) == *templ)
        {
            return true;
        }
        else
        {
            // if not, check with each elments of list
            act = this->list;
            while (act)
            {
                if (act->existsLink(templ)) return true;
                act = act -> next;
            }
        }
    }
    return false;

}



//-------------------------------------------------------------
///////////////////////// SITELINK /////////////////////////////
//-------------------------------------------------------------

//-------------------------------------------------------------
CSiteLink::CSiteLink(const CSiteLink &source)
{
    _PRINTF("Debug: CSiteGraph(CSiteLink &);\n");

    address = new CAddress(*source.address);
    code = source.code;
}

//-------------------------------------------------------------
CSiteLink::CSiteLink(const CAddress &adr)
{
    _PRINTF("Debug: CSiteLink(CAddress &);\n");

    address = new CAddress(adr);
    code = 0;

}

//-------------------------------------------------------------
void CSiteLink::clean(void)
{
    _PRINTF("Debug: CSiteLink:clean();\n");

    if (this->address) delete this->address;
}

//-------------------------------------------------------------
void CSiteLink::print(void) const
{

    printf("CSiteLink (%d): ", code);
    address->print();
}
