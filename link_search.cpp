// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//#define DEBUG

#include "main.h"
#include "link_search.h"


//-------------------------------------------------------------
void str_replace(char* source, char from, char to)
{
    while (*source != '\0')
    {
        if (*source == from)
        {
            *source = to;
        }
        source ++;

    }
}

//-------------------------------------------------------------
CLinkSearch::CLinkSearch(char *buffer, CAddress* adr_relative)
{
     _PRINTF("Debug: CLinkSearch(char *, CSiteGraph* );\n");

    this->FunPi = NULL;
    this->P = NULL;
    this->adr_relative = adr_relative;
    this->graph = NULL;
    //this->mode_print = 1; // to file
    this->Buffer = buffer;

    this->_fl_local = false;
    this->_local_pattern = NULL;

}


//-------------------------------------------------------------
void CLinkSearch::OutputData(CAddress* adr)
{
    FILE *f =  NULL;

    char tmp[_BUF_SMALL] = {0};
    char filename[_BUF_SMALL] = {0};

    sprintf(tmp,"%s_%s%s%s",adr->_protocol(),adr->_domain(),adr->_dir(),adr->_document());

    str_replace(tmp,'/','_');
    str_replace(tmp,'?','!');
    str_replace(tmp,'*','+');
    str_replace(tmp,':','.');
    str_replace(tmp,'\n','_');
    str_replace(tmp,'\r','_');
    str_replace(tmp,'\t','_');

    sprintf(filename,"data/%s",tmp);

    if ((f = fopen(filename,"a")) != NULL)
    {
        //fprintf(f,"FROM(%d): %s://%s%s%s \n",this->father->_level(),this->father->adr->protocol,this->father->adr->domain,this->father->adr->dir,this->father->adr->document);
        //printf("FILE : %s\n",filename);
        fclose(f);
    } else {
        printf("Error: cant create file %s .\n",filename);
    }

}
//-------------------------------------------------------------
int CLinkSearch::GetAddress(char* text)
{
    //get url from .. href = "U R L"  or href=URL
    char* act = text;
    bool flagEnd = false;
    bool flagP = false;
    bool flagA = false;
    char buffer[_BUF_SMALL] = {0};
    char* tmp = buffer;
    int count = 0;

    // pass = char
    while (*act != '=' && *act != '>') {
        if (!isspace(*act)) { // pass only white chars
            return 0;
        }
        act++;
    }
    act++;

    // pass any white char
    while (isspace(*act)) act++;

    // start loop after = char
    while (*act != '>' && count < _BUF_SMALL-1)
    {

        if (*act != '"' && !flagA && !flagP) flagA = true;

        if (flagA && isspace(*act)) flagEnd = true;
        if (*act == '"' && flagP) flagEnd = true;

        if (flagA || flagP)
        {
            if (!flagEnd)
            {
                buffer[count++] = *act;
            }
        }

        if (*act == '"' && !flagP && !flagA) flagP = true;
        act++;

    }

    // new addres obj
    CAddress nextAdr;

    // get initial link as relative to obtain sub links
    // for example. /photography.html is relative

    _PRINTF("CLinkSearch::GetAddress: we found=%s\n",buffer);


    if (!nextAdr.parse(buffer,adr_relative)) {
        return 0;
    }

    if (nextAdr.ishttp())
    {
        //FILE *f = fopen("out.url","a");
        //fprintf(f,"(%d) %s  --> ",this->father->level,buffer);
        //fprintf(f,"%s_%s_%s_%s \n",nextAdr.protocol,nextAdr.domain,nextAdr.dir,nextAdr.document);
        //fclose(f);

        // if we add only local links
        if (_fl_local && _local_pattern)
        {

            if (nextAdr.localAddress(_local_pattern))
            {

                // if it is a first links as result of searchig
                if (!this->graph)
                {
                    this->graph = new CSiteGraph(nextAdr, NULL);
                    // we create only list,  without first rooted link
                    // because function add() at first adds to list member
                    // next to next member of list
                    this->graph->_setlist(this->graph);
                }
                else
                {
                    this->graph->add(nextAdr);
                }

            }
        }
        else
        {
            // we're adding local and global (outside) links
            if (!this->graph)
            {
                this->graph = new CSiteGraph(nextAdr, NULL);
                // we create only list,  without first rooted link
                this->graph->_setlist(this->graph);
            }
            else
            {
                this->graph->add(nextAdr);
            }

        }

    }

    return 0;
}
//-------------------------------------------------------------
void CLinkSearch::CreateFunPi(void)
{
    FunPi[1] = 0;
    int k = 0;
    for (int q = 2; q <= m; q++)
    {
        while (k > 0 && P[k] != P[q-1]) k = FunPi[k];
        if (P[k] == P[q-1]) k++;
        FunPi[q] = k;
    }
}
//-------------------------------------------------------------
void CLinkSearch::Search()
{
    int q=0;
    for (int i=1; i <= n; i++)
    {
        while (q > 0 && P[q] != toupper(T[i-1])) q = FunPi[q];

        if (P[q] == toupper(T[i-1])) q++;

        if (q == m)
        {
            //printf("\npattern found at %d \n",i-m);

            // get addres from string (separate and so on...)
            GetAddress(T+i-m+strlen(P));

            q = FunPi[q];
        }
    }

}
//-------------------------------------------------------------
// it open file and copy to buffer
long CLinkSearch::File2Str(char* file, char* buffer)
{
    FILE *f;
    char smallbuff[10] = {0};
    long size=0;

    if ((f = fopen(file,"r")) == NULL) {
        printf("File open error!\n");
        return -1;
    }

    while (EOF)
    {
        if(!fgets(smallbuff, sizeof(smallbuff), f)) break;
        strcat(buffer,smallbuff);
        size += strlen(smallbuff);
    }
    fclose(f);

    return size;
}
//-------------------------------------------------------------
CSiteGraph* CLinkSearch::SearchLinks(void)
{
    _PRINTF("Debug: CLinkSearch::SearchLinks()\n");

    char pattern[] = "HREF";

    this->FunPi = new int[strlen(pattern)+1];
    this->T = this->Buffer;
    this->n = strlen(this->T);
    this->P = new char[strlen(pattern)+1];
    this->m = strlen(pattern);

    strcpy(P,pattern);

    CreateFunPi();

    Search();

    return graph;
}
//-------------------------------------------------------------
void CLinkSearch::clean(void)
{
    _PRINTF("Debug: CLinkSearch::clean();\n");

    if (this->FunPi) delete [] this->FunPi;
    this->FunPi = NULL;
    if (this->P) delete [] this->P;
    this->P = NULL;

}

//-------------------------------------------------------------
CLinkSearch::~CLinkSearch(void)
{
    clean();
}
