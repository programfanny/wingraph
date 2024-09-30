#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "graphres.h"
//#include "graphGUI.h"
#include "graph.h"
VOID DrawGraph(HWND hwnd, GRAPH *graph);
VOID DrawPath(HWND hwnd, GRAPH *graph, PathList *pathList, int num);
VOID ShowPathList(HWND hwnd, PathList* pathList);
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Graph Demo") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = "graphmenu" ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"), szAppName, MB_ICONERROR) ;
		return 0 ;
	}
	hwnd = CreateWindow (szAppName,                 	// window class name
						TEXT ("The Graph Demo Program"), 	// window caption
						WS_OVERLAPPEDWINDOW,        	// window style
						CW_USEDEFAULT,              	// initial x position
						CW_USEDEFAULT,              	// initial y position
						CW_USEDEFAULT,              	// initial x size
						CW_USEDEFAULT,              	// initial y size
						NULL,                       	// parent window handle
						NULL,                       	// window menu handle
						hInstance,                  	// program instance handle
						NULL) ;                     	// creation parameters
	 
	 ShowWindow (hwnd, iCmdShow) ;
	 UpdateWindow (hwnd) ;
	 
	 while (GetMessage (&msg, NULL, 0, 0))
	 {
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	 }
	 return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
	PAINTSTRUCT ps ;
	HINSTANCE 	hInstance;
	char buf[100];
	static int num=0;
	static RECT rect;
	static GRAPH *graph;
	static PathList *pathList=NULL;
	int endpoint[2];


	switch (message)
	{
	case WM_CREATE:
		//PlaySound (TEXT ("hellowin.wav"), NULL, SND_FILENAME | SND_ASYNC) ;
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		InitGraph(&graph);

		return 0 ;

	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;
		GetClientRect (hwnd, &rect) ;
		//BitBlt(hdc,0,0,500,500,mdc,0,0,SRCCOPY);
		srand((unsigned)time(NULL));
		DrawGraph(hwnd, graph);

		EndPaint (hwnd, &ps) ;
		return 0 ;
	case WM_COMMAND:
		switch(wParam){
		case IDM_TIMER_START:
			if(pathList->size>1)
				SetTimer(hwnd, ID_TIMER, 3000,NULL);
			break;
		case IDM_TIMER_STOP:
			KillTimer(hwnd, ID_TIMER);
			break;
		}

		return 0;	
	case WM_TIMER:
			if(pathList->size){
				DrawGraph(hwnd, graph);
				DrawPath(hwnd, graph, pathList,num);
				//Beep(300+num*15,300);
				num=(num+1)%pathList->size;
			}
		return 0;	
	case WM_KEYDOWN:
		switch(wParam){
			case VK_ESCAPE:
				PostMessage( hwnd,WM_DESTROY,0,0 );
				break;
			case VK_F2:
				if(pathList && pathList->size){
					DrawGraph( hwnd, graph );
					DrawPath( hwnd, graph, pathList, num );
					num=(num+1)%pathList->size;
					//Beep(750,300);
				}
				break;
			case VK_RETURN:
				if(pathList){
					freePathList(pathList);	
					pathList=NULL;
				}
				InitGraph(&graph);
				InvalidateRect(hwnd,&rect,TRUE);
				break;
			case VK_SPACE:
					if(!pathList){
						generateUniqueRandomNumbers(graph->numVertices,2, endpoint);
						pathList=findPaths(graph, endpoint[0], endpoint[1]);
						ShowPathList(hwnd, pathList);
						num=0;
					}
				break;
		}
		return 0;	
		  
	case WM_DESTROY:
		freePathList(pathList);
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
VOID GetPoint(POINT a, POINT b, POINT** pt){
	(*pt)[0] = a;(*pt)[3]=b;
	(*pt)[1].x=(40*(*pt)[0].x+10*(*pt)[3].x-((*pt)[3].y-(*pt)[0].y))/50;
	(*pt)[1].y=(40*(*pt)[0].y+10*(*pt)[3].y+((*pt)[3].x-(*pt)[0].x))/50;
	(*pt)[2].x=(10*(*pt)[0].x+40*(*pt)[3].x-((*pt)[3].y-(*pt)[0].y))/50;
	(*pt)[2].y=(10*(*pt)[0].y+40*(*pt)[3].y+((*pt)[3].x-(*pt)[0].x))/50;
	(*pt)[4].x=((*pt)[0].x+4*(*pt)[3].x)/5;
	(*pt)[4].y=((*pt)[0].y+4*(*pt)[3].y)/5;
	(*pt)[5].x=((*pt)[3].x+2*(*pt)[2].x+2*(*pt)[2].x)/5;
	(*pt)[5].y=((*pt)[3].y+2*(*pt)[2].y+2*(*pt)[2].y)/5;	
}
VOID DrawGraph(HWND hwnd, GRAPH *graph){
	char buf[100];
	HPEN greypen, oldpen;
	HDC hdc = GetDC(hwnd);
	greypen=CreatePen(PS_SOLID,1,RGB(202,222,222));
	oldpen = SelectObject(hdc,greypen);
	POINT* gpos=graph->pos;
	POINT *pt=(POINT*)calloc(7,sizeof(POINT));//four used for Bezier, 3 for trangle arrow
	sprintf(buf,"numVertices: %d",graph->numVertices);
	TextOut(hdc,500,0,buf,strlen(buf));
	sprintf(buf,"numEdges: %d / %d",graph->numEdges, graph->numVertices*(graph->numVertices-1));
	TextOut(hdc,500,20,buf,strlen(buf));
	sprintf(buf,"Adjacency list of vertex:");
	TextOut(hdc,30,0,buf,strlen(buf));
	for(int v=0;v<graph->numVertices; v++){
		GraphNode *temp=graph->adjList[v];
		sprintf(buf," %d ",v);
		TextOut(hdc,10,20+v*20,buf,strlen(buf));
		int w=0;
		while(temp){
			int u=temp->vertex;
			sprintf(buf,"->%d",u);
			TextOut(hdc,30+w*30,20+v*20,buf,strlen(buf));
			GetPoint(gpos[v],gpos[u],&pt);
			PolyBezier(hdc,pt,4);
			MoveToEx(hdc,pt[2].x,pt[2].y,NULL);
			LineTo(hdc,pt[5].x,pt[5].y);
			LineTo(hdc,pt[4].x,pt[4].y);
			temp=temp->next;
			w++;
		}        
	}
	SelectObject(hdc, oldpen);
	SetTextColor(hdc,RGB(64,64,255));
	for(int v=0;v<graph->numVertices; v++){
		Ellipse(hdc,gpos[v].x-10,gpos[v].y-10,gpos[v].x+10,gpos[v].y+10);
		sprintf(buf,"%d",v);
		TextOut(hdc,gpos[v].x-5, gpos[v].y-8,buf,strlen(buf));
	}
	free(pt);
	SetTextColor(hdc,RGB(0,0,0));
	DeleteObject(greypen);
	ReleaseDC(hwnd,hdc);
}
VOID ShowPathList(HWND hwnd, PathList* pathList) {
	char buf[20];
	HDC hdc=GetDC(hwnd);
	sprintf(buf,"pathList count: %d ", pathList->size);
	TextOut(hdc,750,0,buf,strlen(buf));

	for (int i = 0; i < pathList->size; i++) {
		for (int j = 0; j < pathList->paths[i].length; j++) {
			sprintf(buf,"%d ", pathList->paths[i].vertices[j]);
			TextOut(hdc,750+j*13,20+i*18,buf,strlen(buf));
		}
	}
	ReleaseDC(hwnd, hdc);
}
//COLORREF alphaBlend(int alpha,COLORREF color){
//	return RGB(GetRValue(color)*alpha/255,GetGValue(color)*alpha/255,GetBValue(color)*alpha/255);
//}

VOID DrawPath(HWND hwnd, GRAPH *graph, PathList *pathList, int num){
	HDC hdc = GetDC(hwnd);
	char buf[10];
	HPEN redpen, oldpen;
	
	Path* path=&(pathList->paths[num]);
	redpen = CreatePen(PS_SOLID,1,RGB(255,0,0));
	oldpen = SelectObject(hdc,redpen);
	POINT *pt=calloc(7,sizeof(POINT));
	for (int j = 0; j < path->length-1; j++) {
		GetPoint(graph->pos[path->vertices[j]],graph->pos[path->vertices[j+1]],&pt);
		PolyBezier(hdc,pt,4);
		MoveToEx(hdc,pt[2].x,pt[2].y,NULL);
		LineTo(hdc,pt[5].x,pt[5].y);
		LineTo(hdc,pt[4].x,pt[4].y);
	}
	free(pt);
	SelectObject(hdc,oldpen);
	SetTextColor(hdc,RGB(64,64,255));
	int alphaValue = 128;
	HBRUSH whiteBrush = CreateSolidBrush(RGB(255,255,255));
	HBRUSH pinkBrush = CreateSolidBrush(RGB(220,150,200));
	HBRUSH blueBrush = CreateSolidBrush(RGB(128,128,255));
	HBRUSH hOldBrush=(HBRUSH)SelectObject(hdc,whiteBrush);
	int endpoint[2];
	endpoint[0]=pathList->paths[0].vertices[0];
	endpoint[1]=(pathList->paths[0]).vertices[pathList->paths[0].length-1];
	for(int v=0;v<graph->numVertices; v++){
		if(v==endpoint[0]){
			SelectObject(hdc, blueBrush);
			SetBkColor(hdc, RGB(128,128,255));
		}else if(v==endpoint[1]){
			SelectObject(hdc, pinkBrush);
			SetBkColor(hdc, RGB(220,150,200));
		}else{
			SelectObject(hdc, whiteBrush);
			SetBkColor(hdc, RGB(255,255,255));
		}
		Ellipse(hdc,graph->pos[v].x-10,graph->pos[v].y-10,graph->pos[v].x+10,graph->pos[v].y+10);
		sprintf(buf,"%d",v);
		TextOut(hdc,graph->pos[v].x-5, graph->pos[v].y-8,buf,strlen(buf));
	}
	BitBlt(hdc,730,23+(num+pathList->size-1)%pathList->size *18,20,10,hdc,0,0,SRCCOPY);
	SelectObject(hdc, blueBrush);
	Ellipse(hdc,735,23+num*18, 745,33+num*18);
	SelectObject(hdc,hOldBrush);
	SetTextColor(hdc,RGB(0,0,0));
	DeleteObject(redpen);
	DeleteObject(whiteBrush);
	DeleteObject(pinkBrush);
	ReleaseDC(hwnd,hdc);
}
/*
typedef struct _WNDCLASS {
  UINT style; 
  WNDPROC lpfnWndProc; 
  int cbClsExtra; 
  int cbWndExtra; 
  HANDLE hInstance; 
  HICON hIcon; 
  HCURSOR hCursor; 
  HBRUSH hbrBackground; 
  LPCTSTR lpszMenuName; 
  LPCTSTR lpszClassName; 
} WNDCLASS; 
Members
style
Specifies the class style(s). Styles can be combined by using the bitwise OR (|) operator. This member can be any combination of the following values.

Value	Description
CS_DBLCLKS

Sends double-click messages to the window procedure when the user double-clicks the mouse while the cursor is within a window belonging to the class.

CS_HREDRAW

Redraws the entire window if a movement or size adjustment changes the width of the client area.

CS_NOCLOSE

Disables Close on the window menu.

CS_PARENTDC

Sets the clipping region of the child window to that of the parent window so that the child can draw on the parent. A window with the CS_PARENTDC style bit receives a regular device context from the system's cache of device contexts. It does not give the child the parent's device context or device context settings. Specifying CS_PARENTDC enhances an application's performance.

CS_VREDRAW

Redraws the entire window if a movement or size adjustment changes the height of the client area.

lpfnWndProc
Long pointer to the window procedure. You must use the CallWindowProc function to call the window procedure. For more information, see WindowProc.
cbClsExtra
Specifies the number of extra bytes to allocate following the window-class structure. The system initializes the bytes to zero.
cbWndExtra
Specifies the number of extra bytes to allocate following the window instance. The system initializes the bytes to zero. If an application uses WNDCLASS to register a dialog box created by using the CLASS directive in the resource file, the application must set cbWndExtra equal to the value obtained for cbWndExtra when the application calls the GetClassInfo function on the system DIALOG class. This value is the number of extra bytes the OS requires for each dialog box.
hInstance
Handle to the instance that the window procedure of this class is within.
hIcon
Unsupported; set to NULL. An application must draw an icon whenever the user minimizes the application window.
hCursor
Handle to the class cursor. This member must be a handle to a cursor resource. If this member is NULL, an application must explicitly set the cursor shape whenever the mouse moves into the application's window.
hbrBackground
Handle to the class background brush. This member can be a handle to the physical brush to be used for painting the background, or it can be a color value. A color value must be one of the following standard system colors (the value 1 must be added to the chosen color). If a color value is given, you must convert it to one of the following HBRUSH types:

COLOR_ACTIVEBORDER

COLOR_HIGHLIGHTTEXT

COLOR_ACTIVECAPTION

COLOR_INACTIVEBORDER

COLOR_APPWORKSPACE

COLOR_INACTIVECAPTION

COLOR_BACKGROUND

COLOR_MENU

COLOR_BTNFACE

COLOR_MENUTEXT

COLOR_BTNSHADOW

COLOR_SCROLLBAR

COLOR_BTNTEXT

COLOR_WINDOW

COLOR_CAPTIONTEXT

COLOR_WINDOWFRAME

COLOR_GRAYTEXT

COLOR_WINDOWTEXT

COLOR_HIGHLIGHT

 

The system automatically deletes class background brushes when the class is freed. An application should not delete these brushes, because a class may be used by multiple instances of an application.

When this member is NULL, an application must paint its own background whenever it is requested to paint in its client area. To determine whether the background must be painted, an application can either process the WM_ERASEBKGND message or test the fErase member of the PAINTSTRUCT structure filled by the BeginPaint function.

lpszMenuName
Long pointer to a null-terminated character string that specifies the resource name of the class menu, as the name appears in the resource file. If you use an integer to identify the menu, use the MAKEINTRESOURCE macro. If this member is NULL, windows belonging to this class have no default menu.
lpszClassName
Either a long pointer to a null-terminated string or an atom. If this member is an atom, it must be a global atom created by a previous call to the GlobalAddAtom function. The atom, a 16-bit value, must be in the low-order word of lpszClassName; the high-order word must be zero.

If lpszClassName is a string, it specifies the window class name.
Remarks
The following members are restricted.

cbClsExtra must be a multiple of four.
cbWndExtra must be a multiple of four.
Unless you are using the Windows Embedded CE Iconcurs component, which supports mouse cursors on appropriate target platforms, the hCursor member must be set to NULL.
lpszMenuName is not supported and must be NULL.
lpszClassName must be a string. Atoms are not supported.
In Windows CE 1.0, the style member can only be the CS_DBLCLKS value.
In Windows CE 2.0 and later, the style member can be the following values:
CS_DBLCLKS
CS_HREDRAW
CS_IME
CS_NOCLOSE
CS_PARENTDC
CS_VREDRAW
In Windows Embedded CE, all window classes are process global.

BOOL InvalidateRect(
	[in] HWND       hWnd,
	[in] const RECT *lpRect,
	[in] BOOL       bErase
);

typedef struct tagPAINTSTRUCT {
	HDC  hdc;
	BOOL fErase;
	RECT rcPaint;
	BOOL fRestore;
	BOOL fIncUpdate;
	BYTE rgbReserved[32];
} PAINTSTRUCT, *PPAINTSTRUCT, *NPPAINTSTRUCT, *LPPAINTSTRUCT;

typedef struct tagCREATESTRUCTA {
	LPVOID    lpCreateParams;
	HINSTANCE hInstance;
	HMENU     hMenu;
	HWND      hwndParent;
	int       cy;
	int       cx;
	int       y;
	int       x;
	LONG      style;
	LPCSTR    lpszName;
	LPCSTR    lpszClass;
	DWORD     dwExStyle;
} CREATESTRUCTA, *LPCREATESTRUCTA;

typedef struct tagCLIENTCREATESTRUCT {
	HANDLE hWindowMenu;
	UINT   idFirstChild;
} CLIENTCREATESTRUCT, *LPCLIENTCREATESTRUCT;

typedef struct tagMDICREATESTRUCTA {
	LPCSTR szClass;
	LPCSTR szTitle;
	HANDLE hOwner;
	int    x;
	int    y;
	int    cx;
	int    cy;
	DWORD  style;
	LPARAM lParam;
} MDICREATESTRUCTA, *LPMDICREATESTRUCTA;

typedef struct tagBITMAP {
  LONG   bmType;
  LONG   bmWidth;
  LONG   bmHeight;
  LONG   bmWidthBytes;
  WORD   bmPlanes;
  WORD   bmBitsPixel;
  LPVOID bmBits;
} BITMAP, *PBITMAP, *NPBITMAP, *LPBITMAP;

void Graphics(
  [in] HDC    hdc,
  [in] HANDLE hdevice
);

*/



