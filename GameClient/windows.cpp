#include <winsock2.h>
#include <windowsx.h>
#include <process.h>
#include "character.h"
#include "packet.h"
#include "map.h"

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK EditProc(HWND,UINT,WPARAM,LPARAM);
void OnDestroy(HWND hwnd);
void OnPaint(HWND hwnd);
bool OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
void _cdecl GameMain(void *arg);
void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void OnSocket(HWND hWnd);
int phase = 0;

#define WM_SOCKET WM_USER+1

HWND hWnd,h_Edit;
HINSTANCE g_hInst;
SOCKADDR_IN serveraddr,localaddr;
SOCKET sock;
Map curmap;
int curmapno = -1;
CRITICAL_SECTION mapcs,charcs;
WNDPROC oldEditProc;

extern Character character[255];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	char *appName = "GameClient";
	MSG msg;
	WNDCLASS wndclass;
	
	g_hInst = hInstance;

	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndclass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = appName;
	wndclass.lpszMenuName = NULL;
	wndclass.style = 0;
	
	RegisterClass(&wndclass);

	hWnd = CreateWindow(appName,"Client",WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,CW_USEDEFAULT,
						CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hInstance,NULL);
	ShowWindow(hWnd,nShowCmd);
	UpdateWindow(hWnd);

	while(GetMessage(&msg,NULL,0,0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	switch(iMsg) {
		case WM_SOCKET:
			OnSocket(hWnd);
			return 0;
		HANDLE_MSG(hWnd,WM_PAINT,OnPaint);
		HANDLE_MSG(hWnd,WM_KEYDOWN,OnKey);
		HANDLE_MSG(hWnd,WM_CREATE,OnCreate);
		HANDLE_MSG(hWnd,WM_DESTROY,OnDestroy);
	}
	return DefWindowProc(hWnd,iMsg,wParam,lParam);
}

void OnPaint(HWND hwnd) {
	static char first = 1;
	HDC hdc;
	PAINTSTRUCT ps;
	Bitmap back;

	hdc = BeginPaint(hwnd,&ps);
	if(first == 1) {
		first = 0;
		_beginthread(GameMain,0,NULL);
		SetTimer(hwnd,0,100,TimerProc);
	}
	if(phase == 0) {
		back.SetImage("./image/back.bmp");
		back.Draw(hdc,0,0);
	}
	EndPaint(hwnd,&ps);
}

void OnDestroy(HWND hwnd) {
	PACKET pack;
	pack.command = PACKET_LOGOUT;
	sendto(sock,(char *)&pack,sizeof(PACKET),0,(SOCKADDR *)&serveraddr,sizeof(SOCKADDR_IN));
	PostQuitMessage(0);
}

bool OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) {
	RECT rect;
	int dx,dy;
	WSADATA wsd;
	FILE *fp;
	char buf[80];
	int port;

	InitializeCriticalSection(&mapcs);
	InitializeCriticalSection(&charcs);

	ZeroMemory(&serveraddr,sizeof(SOCKADDR_IN));
	ZeroMemory(&localaddr,sizeof(SOCKADDR_IN));
	localaddr.sin_addr.S_un.S_addr = INADDR_ANY;
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = 0;
	
	WSAStartup(MAKEWORD(2,2),&wsd);
	if((fp = fopen("server.ini","r")) == NULL) PostQuitMessage(0);
	fscanf(fp,"%s",buf);
	fscanf(fp,"%d",&port);
	fclose(fp);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.S_un.S_addr = inet_addr(buf);
	serveraddr.sin_port = htons(port);
	sock = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
	bind(sock,(SOCKADDR *)&localaddr,sizeof(SOCKADDR_IN));

	WSAAsyncSelect(sock,hwnd,WM_SOCKET,FD_READ);

	GetClientRect(hwnd,&rect);
	dx = 800 - (rect.right - rect.left);
	dy = 600 - (rect.bottom - rect.top);

	GetWindowRect(hwnd,&rect);

	MoveWindow(hwnd,rect.left,rect.top,rect.right - rect.left + dx, rect.bottom - rect.top + dy, true);
	
	h_Edit = CreateWindow("edit","",WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,0,320,320,20,hwnd,(HMENU)0,g_hInst,NULL);
	ShowWindow(h_Edit,SW_HIDE);
	oldEditProc = (WNDPROC)SetWindowLong(h_Edit,GWL_WNDPROC,(long)EditProc);

	return true;
}

void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) {
	PACKET packet;
	switch(vk) {
	case VK_RETURN:
		ShowWindow(h_Edit,SW_SHOW);
		SetFocus(h_Edit);
		return;
	case VK_RIGHT:
		packet.command = PACKET_RIGHT;
		break;
	case VK_LEFT:
		packet.command = PACKET_LEFT;
		break;
	case VK_UP:
		packet.command = PACKET_UP;
		break;
	case VK_DOWN:
		packet.command = PACKET_DOWN;
		break;
	default:
		return;
	}
	if(!character[0].animation) {
		sendto(sock,(char *)&packet,sizeof(PACKET),0,(SOCKADDR *)&serveraddr,sizeof(SOCKADDR_IN));
	}
}

void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime) {
	int i;
	for(i=0;i<255;i++) {
		character[i].TimeProc();
	}
}

void OnSocket(HWND hWnd) {
	SOCKADDR_IN fromaddr;
	int len = sizeof(SOCKADDR_IN);
	ZeroMemory(&fromaddr,sizeof(SOCKADDR_IN));
	char filename[80];
	bool find = false;
	int i;

	PACKET packet;
	MOVE *move;
	USER *user;

	recvfrom(sock,(char *)&packet,sizeof(PACKET),0,(SOCKADDR *)&fromaddr,&len);
	switch(packet.command) {
	case PACKET_NOTOK:
		MessageBox(hWnd,"로그인 실패하였습니다","Error",0);
		PostQuitMessage(0);
		break;
	case PACKET_LOGINSUCCESS:
		user = (USER *)packet.data;
		EnterCriticalSection(&charcs);
		character[0].Load("./character/rila.chr");
		character[0].who = user->userno;
		character[0].facing = user->Char.facing;
		character[0].SetTileX(user->Char.x);
		character[0].SetTileY(user->Char.y);
		for(i=1;i<255;i++) {
			character[i].who = -1;
		}
		LeaveCriticalSection(&charcs);
		if(curmapno != user->Char.mapno) {
			curmapno = user->Char.mapno;
			wsprintf(filename,"./map/map%d.mpd",curmapno);
			EnterCriticalSection(&mapcs);
			curmap.Load(filename);
			LeaveCriticalSection(&mapcs);
		}
		break;
	case PACKET_MOVE:
		EnterCriticalSection(&charcs);
		move = (MOVE *)packet.data;
		for(i=0;i<255;i++) {
			if(character[i].who == packet.who) {
				find = true;
				character[i].SetTileX(move->mapx);
				character[i].SetTileY(move->mapy);
				character[i].Face(move->direction);
				if(move->anim != 255 && character[i].animation != 1) {
					character[i].PlayMotion(move->anim);
				}
			}
		}
		if(find == false) {
			for(i=0;i<255;i++) {
				if(character[i].who == -1) {
					character[i].who = packet.who;
					character[i].Load("./character/rila.chr");
					character[i].SetTileX(move->mapx);
					character[i].SetTileY(move->mapy);
					character[i].Face(move->direction);
					if(move->anim != 255 && character[i].animation != 1) {
						character[i].PlayMotion(move->anim);
					}
					LeaveCriticalSection(&charcs);
					return;
				}
			}
		}
		find = false;
		LeaveCriticalSection(&charcs);
		break;
	case PACKET_CHAT:
		for(i=0;i<255;i++) {
			if(character[i].who == packet.who) {
				character[i].Chat(packet.data);
				break;
			}
		}
		break;
	case PACKET_LOGOUT:
		for(i=0;i<255;i++) {
			if(character[i].who == packet.who) {
				character[i].who = -1;
				break;
			}
		}
		break;
	}
}

LRESULT CALLBACK EditProc(HWND hWnd,UINT iMsg,WPARAM wParam,LPARAM lParam) {
	char buf[51];
	PACKET pack;

	switch(iMsg) {
	case WM_KEYDOWN:
		if(wParam == VK_RETURN) {
			GetWindowText(hWnd,buf,39);
			pack.command = PACKET_CHAT;
			strcpy(pack.data,buf);
			SetWindowText(hWnd,"");
			ShowWindow(hWnd,SW_HIDE);
			if(strlen(buf) != 0) sendto(sock,(char *)&pack,sizeof(PACKET),0,(SOCKADDR *)&serveraddr,sizeof(SOCKADDR_IN));
		}
		break;
	}
	return CallWindowProc(oldEditProc,hWnd,iMsg,wParam,lParam);
}