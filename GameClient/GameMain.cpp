#include "resource.h"
#include <windowsx.h>
#include <winsock2.h>
#include <process.h>
#include "bitmap.h"
#include "map.h"
#include "packet.h"
#include "character.h"
#include "logindialog.h"

#define WM_SOCKET WM_USER+1

extern HWND hWnd;
extern HINSTANCE g_hInst;

extern char UserName[17];
extern char Password[9];
extern Character character[255];
extern Map curmap;
extern SOCKET sock;
extern SOCKADDR_IN serveraddr,localaddr;
extern CRITICAL_SECTION mapcs,charcs;
extern int phase;

void _cdecl GameMain(void *arg) {
	HDC hdc,memdc;
	HBITMAP membitmap;
	PACKET packet;
	RECT rect;
	int i,dx,dy;

	while(1) {
		hdc = GetDC(hWnd);
		memdc = CreateCompatibleDC(hdc);
		membitmap = CreateCompatibleBitmap(hdc,320,320);
		SelectObject(memdc,membitmap);
		if(phase == 0) {
			LOGIN *login;
			if(DialogBox(g_hInst,MAKEINTRESOURCE(IDD_LOGIN),hWnd,LoginDialogProc) == IDCANCEL) {
				SendMessage(hWnd,WM_DESTROY,0,0);
				return;
			}
			phase = 1;
			InvalidateRect(hWnd,NULL,true);
			GetClientRect(hWnd,&rect);
			dx = 320 - (rect.right - rect.left);
			dy = 340 - (rect.bottom - rect.top);

			GetWindowRect(hWnd,&rect);

			MoveWindow(hWnd,rect.left,rect.top,rect.right - rect.left + dx, rect.bottom - rect.top + dy, true);
			login = (LOGIN *)packet.data;
			packet.command = PACKET_LOGIN;
			strcpy(login->username,UserName);
			strcpy(login->password,Password);
			if(sendto(sock,(char *)&packet,sizeof(PACKET),0,(SOCKADDR *)&serveraddr,sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
				SendMessage(hWnd,WM_DESTROY,0,0);
			}
		}
		if(phase == 1) {
			EnterCriticalSection(&mapcs);
			curmap.Draw(memdc,0,0,320,320,0,0);
			LeaveCriticalSection(&mapcs);
			EnterCriticalSection(&charcs);
			for(i=1;i<255;i++) {
				if(character[i].who != -1)
					character[i].Draw(memdc);
			}
			if(character[0].who != -1) character[0].Draw(memdc);
			LeaveCriticalSection(&charcs);
		}
		BitBlt(hdc,0,0,320,320,memdc,0,0,SRCCOPY);
		DeleteObject(membitmap);
		DeleteDC(memdc);
		ReleaseDC(hWnd,hdc);
	}
}