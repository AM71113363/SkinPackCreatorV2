#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define YES         1
#define NO          0


HINSTANCE ins;
HWND hWnd,B_go; 
HWND SKPname;         WNDPROC OldNameEditProc;
HWND SKPdescription;  WNDPROC OldDescEditProc;


typedef struct MYSKINS_
{
    UCHAR *name;  //the Name Of Skin ( can contain SPACE)
    UCHAR *path; //path of the SKIN to import(just COPY,no MOVE)
    UCHAR flag;  //if the SKIN fails to READ   
    struct MYSKINS_ *next;
}MYSKINS;

#define ISCHECKED(_h_)           SNDMSG(_h_,BM_GETCHECK,0,0)==BST_CHECKED

HWND GEOMETRY_NONE;       UCHAR ADD_NONE;
HWND GEOMETRY_HEROIS;     UCHAR ADD_HEROIS;
HWND GEOMETRY_CUSTOM;     UCHAR ADD_CUSTOM;
HWND GEOMETRY_CUSTOMSLIM; UCHAR ADD_SLIM;
HWND hStatus[8];

//arg=   %s(name) %s(name) %s(uiid) %s(new_uuid)
const static char MANIFEST_JSON[]=
"{\n"
"\x20\x20\x20\x20\"format_version\": 1,\n"
"\x20\x20\x20\x20\"header\": {\n"
"\x20\x20\x20\x20\x20\x20\x20\x20\"description\": \"%s SKinPack\",\n"
"\x20\x20\x20\x20\x20\x20\x20\x20\"name\": \"%s\",\n"
"\x20\x20\x20\x20\x20\x20\x20\x20\"uuid\": \"%s\",\n"
"\x20\x20\x20\x20\x20\x20\x20\x20\"version\": [1, 0, 0],\n"
"\x20\x20\x20\x20\x20\x20\x20\x20\"min_engine_version\": [1, 2, 6]\n"
"\x20\x20\x20\x20},\n"
"\x20\x20\x20\x20\"modules\": [\n"
"\x20\x20\x20\x20{\n"
"\x20\x20\x20\x20\x20\x20\x20\x20\"type\": \"skin_pack\",\n"
"\x20\x20\x20\x20\x20\x20\x20\x20\"uuid\": \"%s\",\n"
"\x20\x20\x20\x20\x20\x20\x20\x20\"version\": [1, 0, 0]\n"
"\x20\x20\x20\x20\x20\x20\x20\x20}\n"
"\x20\x20\x20\x20]\n"
"}\0";

static UCHAR G_none[]="\n";
static UCHAR G_herois[]="\n\"geometry\" : \"geometry.humanoid.herois\",\n";
static UCHAR G_custom[]="\n\"geometry\" : \"geometry.humanoid.custom\",\n";
static UCHAR G_slim[]="\n\"geometry\" : \"geometry.humanoid.customSlim\",\n";

static UCHAR G_first[]="{";
static UCHAR G_next[]=",\n{";


const static char GEOMETRY_JSON_HEADER[]=
"{\n"
"\x20\x20\x20\x20\"skins\":[\n"
"\x20\x20\x20\x20\x20\x20\x20\0";


//ext            "\n"     or "\n"geometry" : "geometry.humanoid.custom",\n"
//arg %s(G_first|G_next) %d(num) %s(an|ah|ac|as)  %s(ext) %d(num)
const static char GEOMETRY_JSON_BODY[]=    
"%s\n"
"\"localization_name\": \"%d%s\",%s"
"\"texture\": \"%dam.png\",\n"
"\"type\": \"free\"\n"
"}\0";


//arg   %s(name)   %s(name)
const static char SKINS_JSON_END[]=      
"\n\x20\x20\x20\x20],\n"
"\x20\x20\x20\x20\"serialize_name\": \"%s\",\n"
"\x20\x20\x20\x20\"localization_name\": \"%s\"\n"
"}\0";


