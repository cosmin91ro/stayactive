// StayActive.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "StayActive.h"
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include "strsafe.h"
#include "Commctrl.h"

#define MAX_LOADSTRING               100   
#define TIMER_ID                     1

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // the title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hLogBox, hBanner;                                 
int tick = 5;  // seconds
int userActive = 1;  // minutes
SYSTEMTIME from, to, now;
LASTINPUTINFO lastInputInfo;
char str[256];                                  // for debug messages


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                SendKeystroke();
void                Log(LPCSTR);
BOOL CALLBACK       SettingsProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
void                ResetTimer(HWND hwnd, int timerId);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_STAYACTIVE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_STAYACTIVE));

    MSG msg;

    // Main message loop:
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


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_STAYACTIVE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 620, 240, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int res;
    HBITMAP hBitmap;

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            Log("Exiting...");
            DestroyWindow(hWnd);
            break;
        case ID_FILE_SETTINGS:
            DialogBox(
                hInst,
                MAKEINTRESOURCE(DLG_SETTINGS),
                hWnd,
                SettingsProc);
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
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_TIMER:
        // check if it's inside user activity window
        GetLocalTime(&now);
        
        FILETIME fromFT, toFT, nowFT;
        SystemTimeToFileTime(&now, &nowFT);
        SystemTimeToFileTime(&from, &fromFT);
        SystemTimeToFileTime(&to, &toFT);

        if (from.wHour * 60 * 60 + from.wMinute * 60 + from.wSecond > now.wHour * 60 * 60 + now.wMinute * 60 + now.wSecond ||
            to.wHour * 60 * 60 + to.wMinute * 60 + to.wSecond < now.wHour * 60 * 60 + now.wMinute * 60 + now.wSecond) {
            Log("Not during the working hours");
            break;
        }

        if(now.wHour >= from.wHour)


        lastInputInfo.cbSize = sizeof(LASTINPUTINFO);
        if (!GetLastInputInfo(&lastInputInfo)) 
        {
            int err = GetLastError();
            Log("Could not get last input info");
        }
        else {
            DWORD tick = GetTickCount();
            DWORD delta = tick - lastInputInfo.dwTime;
            sprintf_s(str, "Ticks since last user input: %d", delta);
            Log(str);
            if (delta >= 60 * 1000 * userActive) {
                // Emulating user activity
                Log("User inactive");
                SendKeystroke();

                // Keeping display open
                // This can be omited since display is kept open by simulating user activity
                // but keeping it here just in case
                res = SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
                if (res == NULL) {
                    sprintf_s(str, "Error while setting thread exec state: %d", res);
                    Log(str);
                }
            }
        }
        break;
    case WM_CREATE:
        // creating the timer
        SetTimer(hWnd,             // handle to main window 
            TIMER_ID,            // timer identifier 
            tick * 1000,
            (TIMERPROC)NULL);     // no timer callback 

        // creating UI
        // log box
        hLogBox = CreateWindow(
            "EDIT",
            "",
            WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT |
            ES_MULTILINE | ES_AUTOVSCROLL,
            0,
            0,
            100,
            100,
            hWnd,
            NULL,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL
        );

        // banner picture
        hBanner = CreateWindow(
            "STATIC",
            NULL,
            WS_CHILD | WS_VISIBLE | SS_BITMAP,
            400,
            0,
            200,
            200,
            hWnd,
            NULL,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL
        );

        hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP5));
        SendMessage(hBanner, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);


        // initialize the user activity window (default 09:00 - 18:00)
        from = { 2021,1,5,1,9,0,0,0 };
        to = { 2021,1,5,1,18,0,0,0 };

        break;
    case WM_SIZE:
        // Make the LogBox the size of the window's client area. 
        MoveWindow(hLogBox,
            0, 0,                  // starting x- and y-coordinates 
            LOWORD(lParam) - 200,        // width of client area 
            HIWORD(lParam),        // height of client area 
            TRUE);
        break;
        
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void SendKeystroke()
{
    std::cout << "Sending keystroke ...\n";
    INPUT inputs[2];
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_NUMLOCK;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_NUMLOCK;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
    {
        std::cout << "SendInput failed: " << HRESULT_FROM_WIN32(GetLastError()) << "\n";
    }
}

void Log(LPCSTR msg) {
    GetLocalTime(&now);

    int const finalMsgSize = strlen(msg) + 24;  // to accomodate the datetime prefix + newline char
    LPTSTR finalMsg = new TCHAR[finalMsgSize];
    size_t cbDest = finalMsgSize * sizeof(TCHAR);
    StringCbPrintf(finalMsg, cbDest, "%04d/%02d/%02d %02d:%02d:%02d: %s\r\n",
        now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, msg);
        
    int index = GetWindowTextLength(hLogBox);
    SetFocus(hLogBox); // set focus
    SendMessageA(hLogBox, EM_SETSEL, (WPARAM)index, (LPARAM)index); // set selection - end of text
    SendMessageA(hLogBox, EM_REPLACESEL, 0, (LPARAM)finalMsg); // append!
}

BOOL CALLBACK SettingsProc(HWND hwndDlg,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    HWND hTimePicker;
    int success;
    
    switch (message)
    {
    case WM_INITDIALOG:
        SetDlgItemInt(hwndDlg, EDIT_TICK, tick, false);
        SetDlgItemInt(hwndDlg, EDIT_USER, userActive, false);

        hTimePicker = GetDlgItem(hwndDlg, TIMEPICKER_FROM);
        SendMessage(hTimePicker, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)(&from));

        hTimePicker = GetDlgItem(hwndDlg, TIMEPICKER_TO);
        SendMessage(hTimePicker, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)(&to));

        break;
    case WM_COMMAND:
        int id = LOWORD(wParam);
        switch (id)
        {
        case IDOK:
            tick = GetDlgItemInt(hwndDlg, EDIT_TICK, &success, false);
            if (!success) {
                Log("Could not get tick value");
                goto closeit;
            }
            
            ResetTimer(GetParent(hwndDlg), TIMER_ID);
            userActive = GetDlgItemInt(hwndDlg, EDIT_USER, &success, false);
            if (!success) {
                Log("Could not get user timeout value");
                goto closeit;
            }


            hTimePicker = GetDlgItem(hwndDlg, TIMEPICKER_FROM);
            SendMessage(hTimePicker, DTM_GETSYSTEMTIME, NULL, (LPARAM)(&from));

            hTimePicker = GetDlgItem(hwndDlg, TIMEPICKER_TO);
            SendMessage(hTimePicker, DTM_GETSYSTEMTIME, NULL, (LPARAM)(&to));

            Log("Settings saved");
            
        case IDCANCEL:
        closeit:
            EndDialog(hwndDlg, wParam);
            return TRUE;
        }
    }
    return FALSE;
}

void ResetTimer(HWND hwnd, int timerId) {
    KillTimer(hwnd, timerId);

    SetTimer(hwnd,             // handle to main window 
        TIMER_ID,            // timer identifier 
        tick * 1000,
        (TIMERPROC)NULL);     // no timer callback 
}
