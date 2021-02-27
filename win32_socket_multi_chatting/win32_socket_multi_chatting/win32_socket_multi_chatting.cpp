#include "framework.h"
#include "win32_socket_multi_chatting.h"
#include <string>
#include <process.h>
#include "Client.h"

using namespace std;

#define MAX_LOADSTRING 100
#define PACKET_SIZE 1024
#define MAX_NAME_LENGTH 100

HWND g_hWnd;
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

Client* client;
HANDLE hMutex;
string informationMessage;
HWND editboxClientName;
HWND connectButton;

HWND editBoxOutputHandle;
HWND editBoxInputHandle;

void CreateLobbyFrame();
void HideLobbyFrame();
void CreateChattingFrame();
void UploadChatting();

void MoveScrollbarToEnd(HWND hwnd);

UINT WINAPI ReceiveMessageThread(void* arg);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32SOCKETMULTICHATTING, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32SOCKETMULTICHATTING));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32SOCKETMULTICHATTING));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = 0; //MAKEINTRESOURCEW(IDC_WIN32SOCKETMULTICHATTING);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 500, 500, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        g_hWnd = hWnd;
        CreateLobbyFrame();
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);

            switch (wmId)
            {
            case IDC_CONNECT_ROOM:
                char clientName[MAX_NAME_LENGTH];
                GetWindowText(editboxClientName, clientName, MAX_NAME_LENGTH);

                HideLobbyFrame();

                CreateChattingFrame();
                client = new Client(clientName);
                hMutex = CreateMutex(NULL, false, NULL);
                _beginthreadex(NULL, 0, ReceiveMessageThread, nullptr, 0, NULL);
                break;
            case IDC_EDIT_BOX: 
                break;
            case IDC_SEND_MESSAGE:
                UploadChatting();
                break;
            /*case IDC_CREATE_ROOM:
                MessageBox(hWnd, "test", "제목", 0);
                break;
            case IDC_CONNECT_ROOM:
                MessageBox(hWnd, "test", "제목", 0);
                break;*/
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        delete client;

        CloseHandle(hMutex);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void CreateLobbyFrame()
{
    editboxClientName = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER
        , 200, 150, 100, 30, g_hWnd, (HMENU)IDC_EDIT_BOX, hInst, NULL);
    connectButton = CreateWindow("button", "입장", WS_CHILD | WS_VISIBLE | WS_BORDER, 200, 230, 100, 50, g_hWnd, (HMENU)IDC_CONNECT_ROOM, hInst, NULL);
}

void HideLobbyFrame()
{
    ShowWindow(editboxClientName, SW_HIDE);
    ShowWindow(connectButton, SW_HIDE);
}

void CreateChattingFrame()
{
    editBoxOutputHandle = CreateWindow("edit", informationMessage.c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY,
        15, 25, 455, 350, g_hWnd, (HMENU)IDC_CHATTING_BOX, hInst, NULL);
    editBoxInputHandle = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOVSCROLL | ES_MULTILINE
        , 15, 390, 405, 40, g_hWnd, (HMENU)IDC_EDIT_BOX, hInst, NULL);
    CreateWindow("button", "전송", WS_CHILD | WS_VISIBLE | WS_BORDER, 428, 385, 50, 50, g_hWnd, (HMENU)IDC_SEND_MESSAGE, hInst, NULL);
}

void UploadChatting()
{
    SetFocus(editBoxInputHandle);
    char tempChatMessage[PACKET_SIZE];
    GetWindowText(editBoxInputHandle, tempChatMessage, PACKET_SIZE);
    if (0 == strlen(tempChatMessage))
        return;
    
    char chatMessage[PACKET_SIZE + MAX_NAME_LENGTH];
    strcpy(chatMessage, client->GetName().c_str());
    strcat(chatMessage, " : ");
    strcat(chatMessage, tempChatMessage);
    client->SendMessageToServer(chatMessage);
    SetWindowText(editBoxInputHandle, "");

    // scrollbar 자동 이동
    MoveScrollbarToEnd(editBoxOutputHandle);
}

void MoveScrollbarToEnd(HWND hwnd)
{
    SendMessageA(hwnd, EM_SETSEL, 0, -1); //Select all. 
    SendMessageA(hwnd, EM_SETSEL, -1, -1);//Unselect and stay at the end pos
    SendMessageA(hwnd, EM_SCROLLCARET, -1, -1); //Set scrollcaret to the current Pos
}

UINT WINAPI ReceiveMessageThread(void* arg)
{
    bool loop = true;
    string recvString;
    while (loop)
    {
        recvString = client->RecvMessageFromServer();
        if (recvString == "")
            break;
        else
        {
            WaitForSingleObject(hMutex, INFINITE);

            informationMessage += recvString + "\r\n";
            SetWindowText(editBoxOutputHandle, informationMessage.c_str());

            // scrollbar 자동 이동
            MoveScrollbarToEnd(editBoxOutputHandle);
          
            ReleaseMutex(hMutex);
        }
    }

    return 0;
}
