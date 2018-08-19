#include "logindialog.h"
#include "resource.h"
#include "packet.h"
#include "map.h"
#include <windowsx.h>
#include <winsock2.h>

#define WM_SOCKET WM_USER+1

extern HWND hWnd;
extern HINSTANCE g_hInst;

char UserName[17];
char Password[9];
extern SOCKET sock;

bool OnDialogCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnDialogPaint(HWND hwnd);
void OnDialogCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

int CALLBACK LoginDialogProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	switch(iMsg) {
		HANDLE_MSG(hWnd,WM_CREATE,OnDialogCreate);
		HANDLE_MSG(hWnd,WM_PAINT,OnDialogPaint);
		HANDLE_MSG(hWnd,WM_COMMAND,OnDialogCommand);
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

bool OnDialogCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) {
	return true;
}

void OnDialogPaint(HWND hwnd) {
	RECT windowrect, dialogrect;
	GetWindowRect(hWnd,&windowrect);
	GetWindowRect(hwnd,&dialogrect);
	
	MoveWindow(hwnd,(windowrect.right - (dialogrect.right - dialogrect.left))/2,
			(windowrect.bottom - (dialogrect.bottom - dialogrect.top))/2,dialogrect.right - dialogrect.left,
			dialogrect.bottom - dialogrect.top,true);
	SetFocus(GetDlgItem(hwnd,IDC_USERNAME));
}

void OnDialogCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
	if(id == IDOK) {
		EndDialog(hwnd,IDOK);
	}
	if(id == IDCANCEL) {
		EndDialog(hwnd,IDCANCEL);
	}
	if(id == IDC_USERNAME) {
		GetWindowText(hwndCtl,UserName,17);
	}
	if(id == IDC_PASSWORD) {
		GetWindowText(hwndCtl,Password,9);
	}
}

