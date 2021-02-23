#include "framework.h"
#include "win32_socket_multi_chatting.h"
#include <string>

using namespace std;

#define MAX_LOADSTRING 100
#define PACKET_SIZE 1024

HWND g_hWnd;
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

string informationMessage;
HWND editBoxHandle;

void createChattingFrame();
void uploadChatting();

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
        createChattingFrame();
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);

            switch (wmId)
            {
            case IDC_EDIT_BOX: 
                break;
            case IDC_SEND_MESSAGE:
                uploadChatting();
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
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    UNREFERENCED_PARAMETER(lParam);
//    switch (message)
//    {
//    case WM_INITDIALOG:
//        return (INT_PTR)TRUE;
//
//    case WM_COMMAND:
//        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//        {
//            EndDialog(hDlg, LOWORD(wParam));
//            return (INT_PTR)TRUE;
//        }
//        break;
//    }
//    return (INT_PTR)FALSE;
//}

void createChattingFrame()
{
    CreateWindow("static", informationMessage.c_str(), WS_CHILD | WS_VISIBLE | SS_RIGHT | WS_BORDER | WS_VSCROLL,
        15, 25, 455, 350, g_hWnd, (HMENU)IDC_CHATTING_BOX, hInst, NULL);
    editBoxHandle = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | SS_RIGHT | WS_BORDER | ES_AUTOVSCROLL
        , 15, 390, 405, 40, g_hWnd, (HMENU)IDC_EDIT_BOX, hInst, NULL);
    CreateWindow("button", "전송", WS_CHILD | WS_VISIBLE | WS_BORDER, 428, 385, 50, 50, g_hWnd, (HMENU)IDC_SEND_MESSAGE, hInst, NULL);
}

void uploadChatting()
{
    SetFocus(editBoxHandle);
    char tempChatMessage[PACKET_SIZE];
    GetWindowText(editBoxHandle, tempChatMessage, PACKET_SIZE);
    if (0 == strlen(tempChatMessage))
        return;

    informationMessage += tempChatMessage;
    informationMessage += "\n";
    SetDlgItemText(g_hWnd, IDC_CHATTING_BOX, informationMessage.c_str());
    SetWindowText(editBoxHandle, "");
}
