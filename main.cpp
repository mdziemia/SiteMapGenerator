// -------------------------------------------------------------------------
// SiteMap Generator
// Date: 11.12.2014
// Version: none
// Author: Maciej Dziemianczuk
// -------------------------------------------------------------------------
#include "main.h"
#include "sitemap.h"



// -------------------------------------------------------------------------
int main(int argc, char *argv[])
{

    CSitemap map("http://localhost/ug/");

    // only local links
    map.onlyLocal();

    // without specified links
    char ** exceptLinks = new char*[1];
    exceptLinks[0] = new char[256];
    strcpy(exceptLinks[0],"http://localhost/ug/ProfAKK/");

    // add exception links
    map.exceptLinks(1,exceptLinks);


    // create a sitemap
    map.create("sitemap.txt");

    WSACleanup();

    return 0;
}
