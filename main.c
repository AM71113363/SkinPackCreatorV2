#include "main.h"
#include "myzip.h"

char szClassName[ ] = "WindowsAppSkinPack";
void CenterOnScreen();

#define SMS(_x_)  MessageBox(hWnd,_x_,"#Error",MB_OK |MB_ICONERROR)
#define OPR(_i_)  EnableWindow(hStatus[_i_],1)

MYSKINS *myskins=NULL;
MYSKINS *PM;              //pointer to head list of "myskins"
UINT myskinsLen=0;
UCHAR Status[32];


DWORD AddJsonGeometry(UCHAR *p,UCHAR *first,DWORD i, UCHAR *ext, UCHAR *geo )
{
   DWORD len;
   len = sprintf(p,GEOMETRY_JSON_BODY, first,i, ext, geo, i);

return len;
}

void FreeList()
{
    MYSKINS *s,*prev; 
    for(s=PM;s!=NULL;)
    {
        if(s->name) free(s->name);
        if(s->path) free(s->path);                       
        
        prev = s;
        s=s->next;
        free(prev);                       
    } 
    myskins=NULL;
}

void AddToList(UCHAR *path, UCHAR *name)
{
    MYSKINS *s=(MYSKINS*)malloc(sizeof(MYSKINS));
    if(s==NULL)
     return;
    s->name=(UCHAR*)malloc(strlen(name)+1);
    if(s->name==NULL)
    {
      free(s);
      return;
    }
    s->path=(UCHAR*)malloc(strlen(path)+1);
    if(s->path==NULL)
    {
      free(s->name);
      free(s);
      return;
    }
    sprintf(s->path,"%s\0",path);
    sprintf(s->name,"%s\0",name);
    s->flag=NO; 
    if(myskins==NULL)
    {
        myskins=s;
        myskins->next=NULL;
        if(myskinsLen==0)
            PM = myskins;
    }
    else
    {           
       s->next=NULL;       
       myskins->next=s;
       myskins = s;
    }
    myskinsLen++;
}

UCHAR AddSkin(UCHAR *filename)
{
    UCHAR *p,*e; UINT i; UCHAR temp[MAX_PATH];
    e=strstr(filename,".png");
    if(!e)
     return NO;
    p=strrchr(filename,'\\');
    if(!p)
     return NO;
    p++; 
    e[0]=0; //delete .png
    sprintf(temp,"%s\0",p);
    e[0]='.'; //restore .png
    AddToList(filename, temp);
    sprintf(Status,"Imported Skins[ %d ]\0",myskinsLen);
    SetWindowText(hWnd,Status);
    if(myskinsLen==1)
     EnableWindow(B_go,1);  //no point of creating a skinPack with only 1 skin 
    return YES;
}

void MakeMyPack()
{
	UCHAR RootFolder[100]; //the Pack will use this NAME
	UCHAR ThisDescription[100]; //this is used only in en_US.lang,DISPLAY pack name
	UCHAR Temp[512];
	UCHAR UUID1[36]; UCHAR UUID2[36]; ULONG crc;

	ZIP A; UCHAR Ret; UCHAR *buffer; MYSKINS *s;
	DWORD len; UINT i; UCHAR *p;
//get Geometry Settings
    if(ISCHECKED(GEOMETRY_NONE))      { ADD_NONE = 1;   }else{ ADD_NONE = 0; }
    if(ISCHECKED(GEOMETRY_HEROIS))    { ADD_HEROIS = 1; }else{ ADD_HEROIS = 0; }
    if(ISCHECKED(GEOMETRY_CUSTOM))    { ADD_CUSTOM = 1; }else{ ADD_CUSTOM = 0; }
    if(ISCHECKED(GEOMETRY_CUSTOMSLIM)){ ADD_SLIM = 1;   }else{ ADD_SLIM = 0;   }
    
    if( (ADD_NONE+ADD_HEROIS+ADD_CUSTOM+ADD_SLIM) == 0)
    {
        SMS("You Must Select At Least 1 Geometry Option!!!!");
        EnableWindow(B_go,1);
        return;
    } 
//Get INFO     
    memset(RootFolder,0,100);
    if(GetWindowText(SKPname,RootFolder,99)<1)
    {
        SMS("SkinPack Name : Is Empty!!!!");
        EnableWindow(B_go,1);
        return;
    }
    memset(ThisDescription,0,100);
    if(GetWindowText(SKPdescription,ThisDescription,99)<1)
    { 
        //is empty use RootFolder name
        strcpy(ThisDescription,RootFolder);
        SetWindowText(SKPdescription,ThisDescription);
    }
//calculate buffer size to hold data
    //size of each entry
    len=0;
    //assume adding 8888 skins
    //length of (8888am + 8888am.png) = 16
    if(ADD_NONE)  { len+=(16 + sizeof(G_none) +   sizeof(GEOMETRY_JSON_BODY)); }
    if(ADD_HEROIS){ len+=(16 + sizeof(G_herois) + sizeof(GEOMETRY_JSON_BODY)); }
    if(ADD_CUSTOM){ len+=(16 + sizeof(G_custom) + sizeof(GEOMETRY_JSON_BODY)); }
    if(ADD_SLIM)  { len+=(16 + sizeof(G_slim) +   sizeof(GEOMETRY_JSON_BODY)); }
    
    len = len * myskinsLen; //multiply for all skins
    len+=sizeof(GEOMETRY_JSON_HEADER);
    len+=sizeof(SKINS_JSON_END) + 100; //100 = PackName+Description (LENGTH)
    len+=50;  //add some extra,who knows
    buffer = (UCHAR*)malloc(len);
    if(buffer==NULL)
    {  
        SMS("Malloc Failed!!!!");
        exit(0);
        return;
    }  	
OPR(0);     
//create Zip File
    sprintf(Temp,"%s.mcpack\0",RootFolder);
    Ret=ZipInit(&A,Temp);
    if(Ret==NO)
    {  
        SMS("Failed To Create SkinPack!!!!");
        exit(0);
        return;
    }  	
OPR(1);
//add PNG to ZIP, maybe some files are not for READ
    len=0;
    i=0;
    for(s=PM;s!=NULL;s=s->next)
    {
	    sprintf(Temp,"%dam.png\0",i);	
	    Ret=FileToZip(&A,s->path,Temp);
	    if(Ret==NO)
	    {
           if(A.flag==YES) //cand read file
              continue;
           free(buffer);            
           FreeList();
           FreeZip(&A);
           SMS("Error: Add(PNG to MCPACK)\nDelete the created .mcpack");
           SMS(A.err);
           exit(0);
           return;
        }
        i++;                              
	    s->flag=YES; 
    }
    //just checking
    if(i==0)
    {
        FreeList();    
        FreeZip(&A); 
        free(buffer);     
	    SMS("SkinPack Failed To COPY!!!!\nDelete the created .mcpack");
        exit(0);
	    return;
    } 
OPR(2);    
 //create LANG file
 	
   	len=sprintf(buffer,"skinpack.%s=%s\n\0",RootFolder,ThisDescription); 
	
	i=0; myskinsLen = 0;
    for(s=PM;s!=NULL;s=s->next)
    {
		if(s->flag!=YES)
				continue;
		 if(ADD_NONE){
	     len+=sprintf(&buffer[len],"skin.%s.%dan=%s\n\0",RootFolder,i,s->name); }
         if(ADD_HEROIS){
	     len+=sprintf(&buffer[len],"skin.%s.%dah=%s (HEROIS)\n\0",RootFolder,i,s->name);}
		 if(ADD_CUSTOM){
	     len+=sprintf(&buffer[len],"skin.%s.%dac=%s (CUSTOM)\n\0",RootFolder,i,s->name);}
		 if(ADD_SLIM){
	     len+=sprintf(&buffer[len],"skin.%s.%das=%s (SLIM)\n\0",RootFolder,i,s->name);}
	     i++;
	     myskinsLen++;
    }
    buffer[len]=0;
OPR(3);
 //   Ret = BufferToZip(&A,"texts/",NULL,0);
    Ret = BufferToZip(&A,"texts/en_US.lang",buffer,len);
    if(Ret==NO)
    {
        FreeList();
        FreeZip(&A); 
        free(buffer);     
	    SMS("Adding en_US.lang Failed!!!!\nDelete the created .mcpack");
	    SMS(A.err);
        exit(0);
	    return;
    }
OPR(4);
//create manifest.json 
    crc=0;
    //generate UUIDs
    GenerateUUID(UUID1,&crc);
    GenerateUUID(UUID2,&crc);
    len=sprintf(buffer,MANIFEST_JSON,RootFolder,RootFolder,UUID1,UUID2);
	buffer[len]=0;
	Ret = BufferToZip(&A,"manifest.json",buffer,len);
    if(Ret==NO)
    {
        FreeList();
        FreeZip(&A); 
        free(buffer);     
	    SMS("Adding manifest.json Failed!!!!\nDelete the created .mcpack");
	    SMS(A.err);
        exit(0);
	    return;
    }
OPR(5);
//create skins.json  
    p=G_first;   
	len=sprintf(buffer,GEOMETRY_JSON_HEADER," "); //fflush
    if(ADD_NONE){
    len+=AddJsonGeometry(&buffer[len],p, 0, "an",G_none); p=G_next; }
    if(ADD_HEROIS){
    len+=AddJsonGeometry(&buffer[len],p, 0, "ah",G_herois); p=G_next; }
    if(ADD_CUSTOM){
    len+=AddJsonGeometry(&buffer[len],p, 0, "ac",G_custom); p=G_next; }
    if(ADD_SLIM){
    len+=AddJsonGeometry(&buffer[len],p, 0, "as",G_slim); }

    //write other skins
	for(i=1;i<myskinsLen;i++)
	{
        if(ADD_NONE){
        len+=AddJsonGeometry(&buffer[len],",\n{", i, "an",G_none);   }
        if(ADD_HEROIS){
        len+=AddJsonGeometry(&buffer[len],",\n{", i, "ah",G_herois); }
        if(ADD_CUSTOM){
        len+=AddJsonGeometry(&buffer[len],",\n{", i, "ac",G_custom); }
        if(ADD_SLIM){
        len+=AddJsonGeometry(&buffer[len],",\n{", i, "as",G_slim);   }
	}		
    //write END
    len+=sprintf(&buffer[len],SKINS_JSON_END,RootFolder,RootFolder);
    buffer[len]=0;
OPR(6);
	Ret = BufferToZip(&A,"skins.json",buffer,len);
    if(Ret==NO)
    {
        FreeList();
        FreeZip(&A); 
        free(buffer);     
	    SMS("Adding skins.json Failed!!!!\nDelete the created .mcpack");
	    SMS(A.err);
        exit(0);
	    return;
    } 
OPR(7);
//close ZIP, Write Final DATA
	Ret=EndZip(&A);
    if(Ret==NO)
    {
        FreeList();
        FreeZip(&A); 
        free(buffer);     
	    SMS("Adding Zip Extra DATA Failed!!!!\nDelete the created .mcpack");
	    SMS(A.err);
        exit(0);
	    return;
    } 		

    FreeList();
    free(buffer);
	myskinsLen=0;  
   SetWindowText(hWnd,"DONE"); 
   _sleep(2000); 
   exit(0);                    
}



LRESULT CALLBACK NameEditProc(HWND hnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
    	case WM_CHAR: 
        {
             if(wParam == VK_BACK)
             break;
			if(!(isalnum((char)wParam))) return 0;
	     }break;		
    }
	return CallWindowProc(OldNameEditProc, hnd, message, wParam, lParam);
}

LRESULT CALLBACK DescEditProc(HWND hnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
    	case WM_CHAR: 
        {
             if((wParam == VK_BACK) || (wParam == VK_SPACE) || 
                (wParam == '_') || (wParam == '-') ||
                (wParam == '(') || (wParam == ')'))
             break;
			if(!(isalnum((char)wParam))) return 0;
	     }break;		
    }
	return CallWindowProc(OldDescEditProc, hnd, message, wParam, lParam);
}


LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {
             hWnd = hwnd;
             CreateWindow("BUTTON","Geometry",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,2,2,117,105,hwnd,NULL,ins,NULL);
             GEOMETRY_NONE = CreateWindow("BUTTON","None",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,9,21,100,19,hwnd,NULL,ins,NULL);	
             GEOMETRY_HEROIS = CreateWindow("BUTTON","Herois",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,9,42,100,19,hwnd,NULL,ins,NULL);	
             GEOMETRY_CUSTOM = CreateWindow("BUTTON","Custom",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,9,63,100,19,hwnd,NULL,ins,NULL);	
             GEOMETRY_CUSTOMSLIM = CreateWindow("BUTTON","CustomSlim",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,9,84,100,19,hwnd,NULL,ins,NULL);	
           
           
             CreateWindow("BUTTON","SkinPack Name",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,122,2,217,50,hwnd,NULL,ins,NULL);
             SKPname= CreateWindowEx(WS_EX_CLIENTEDGE,"edit","",WS_CHILD|WS_VISIBLE,131,22,200,22,hwnd,NULL,ins,NULL);
             OldNameEditProc=(WNDPROC)SetWindowLong(SKPname, GWL_WNDPROC, (LPARAM)NameEditProc);
                 
             CreateWindow("BUTTON","Description",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,122,57,217,50,hwnd,NULL,ins,NULL);
             SKPdescription= CreateWindowEx(WS_EX_CLIENTEDGE,"edit","",WS_CHILD|WS_VISIBLE,131,77,200,22,hwnd,NULL,ins,NULL);
    		 OldDescEditProc=(WNDPROC)SetWindowLong(SKPdescription, GWL_WNDPROC, (LPARAM)DescEditProc);
             
             CreateWindow("BUTTON","STATUS",WS_CHILD|WS_VISIBLE|BS_GROUPBOX|BS_CENTER,2,110,274,47,hwnd,NULL,ins,NULL);
             hStatus[0] = CreateWindow("BUTTON","1",WS_CHILD|WS_VISIBLE|WS_DISABLED,8,129,30,22,hwnd,NULL,ins,NULL);
             hStatus[1] = CreateWindow("BUTTON","2",WS_CHILD|WS_VISIBLE|WS_DISABLED,41,129,30,22,hwnd,NULL,ins,NULL);
             hStatus[2] = CreateWindow("BUTTON","3",WS_CHILD|WS_VISIBLE|WS_DISABLED,74,129,30,22,hwnd,NULL,ins,NULL);
             hStatus[3] = CreateWindow("BUTTON","4",WS_CHILD|WS_VISIBLE|WS_DISABLED,107,129,30,22,hwnd,NULL,ins,NULL);
             hStatus[4] = CreateWindow("BUTTON","5",WS_CHILD|WS_VISIBLE|WS_DISABLED,140,129,30,22,hwnd,NULL,ins,NULL);
             hStatus[5] = CreateWindow("BUTTON","6",WS_CHILD|WS_VISIBLE|WS_DISABLED,173,129,30,22,hwnd,NULL,ins,NULL);
             hStatus[6] = CreateWindow("BUTTON","7",WS_CHILD|WS_VISIBLE|WS_DISABLED,206,129,30,22,hwnd,NULL,ins,NULL);
             hStatus[7] = CreateWindow("BUTTON","8",WS_CHILD|WS_VISIBLE|WS_DISABLED,239,129,30,22,hwnd,NULL,ins,NULL);
             
    
   // hStatus
                   
             B_go = CreateWindow("BUTTON","START",WS_CHILD|WS_VISIBLE|WS_DISABLED,282,109,54,47,hwnd,(HMENU)1111,ins,NULL);
             
             SNDMSG(GEOMETRY_NONE,BM_SETCHECK,(WPARAM)BST_CHECKED,(LPARAM)0); 
             CenterOnScreen();
             DragAcceptFiles(hwnd,1);
        }
        break;   
        case WM_DROPFILES:
		{
           HDROP hDrop;
		   UINT sa;
           UINT x;    
           UCHAR tmp[MAX_PATH];

           hDrop=(HDROP)wParam;	
           sa=DragQueryFile(hDrop,0xFFFFFFFF,0,0);
           for(x=0;x<sa;x++)
           {
               memset(tmp,0,MAX_PATH);
               DragQueryFile(hDrop,x,tmp,MAX_PATH);
               if((GetFileAttributes(tmp) & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY)
                   continue;
               AddSkin(tmp);
           }
		   DragFinish(hDrop);
     }
     break; 
       case WM_COMMAND:
       {
            switch(LOWORD(wParam))
            {                  
                 case 1111:
                 {
                    EnableWindow(B_go,0);  
                    CreateThread(0,0,(LPTHREAD_START_ROUTINE)MakeMyPack,0,0,0);        
                 }
                 break;                 
            }
            return 0;
       }
       break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}


int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
            
    MSG messages;    
    WNDCLASSEX wincl;  
    HWND hwnd;       
    ins=hThisInstance;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;  
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (ins,MAKEINTRESOURCE(200));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;  
    wincl.cbClsExtra = 0;  
    wincl.cbWndExtra = 0;      
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND+1;

    if (!RegisterClassEx (&wincl))
        return 0;

    hwnd = CreateWindowEx(WS_EX_TOPMOST,szClassName,"SkinPack Creator",WS_SYSMENU|WS_MINIMIZEBOX,CW_USEDEFAULT,CW_USEDEFAULT,
    347,190,HWND_DESKTOP,NULL,hThisInstance,NULL );
    
    ShowWindow (hwnd, nFunsterStil);

    while (GetMessage (&messages, NULL, 0, 0))
    {
         TranslateMessage(&messages);
         DispatchMessage(&messages);
    }

     return messages.wParam;
}

void CenterOnScreen()
{
     RECT rcClient, rcDesktop;
	 int nX, nY;
     SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);
     GetWindowRect(hWnd, &rcClient);
	 nX=((rcDesktop.right - rcDesktop.left) / 2) -((rcClient.right - rcClient.left) / 2);
	 nY=((rcDesktop.bottom - rcDesktop.top) / 2) -((rcClient.bottom - rcClient.top) / 2);
SetWindowPos(hWnd, NULL, nX, nY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

