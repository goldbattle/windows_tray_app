// Windows Header Files:
#include <windows.h>
#include <shellapi.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
// C++ Runtime files
#include <iostream>
#include "resource.h"

#define MAX_LOADSTRING 100
#define WM_USER_SHELLICON WM_USER + 1

// Global Variables:
HINSTANCE hInst;
MSG msg;
NOTIFYICONDATA nidApp;
HMENU hPopMenu;
BOOL bDisable = FALSE;

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {


  // Debug
  std::cout << "[debug]: Windows Main Function Called" << std::endl;

  // The do something, not sure what
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  
  // Register our window class
  std::cout << "[debug]: Registering Our Window Class" << std::endl;
  MyRegisterClass(hInstance);

  // Perform application initialization:
  if (!InitInstance(hInstance, nCmdShow)) {
    std::cerr << "[error]: Unable to Create Window" << std::endl;
    return FALSE;
  }

  // Success
  std::cout << "[debug]: Window Has Been Created" << std::endl;

  // The main message loop
  while (GetMessage(&msg, NULL, 0, 0)) {
    // For now, if we get a message, lets process it
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }


  // Cleanup, and shutdown
  std::cout << "[debug]: Clean Exit, Shutting Down" << std::endl;
  return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {

  // Icons
  HICON hMainIcon;
  HICON hSmallIcon;

  // Load in our icon image
  //hMainIcon = LoadIcon(hInstance, "SysTrayDemo.ico");
  hMainIcon = (HICON)LoadImage(0, _T("resources/icon_large.ico"), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
  hSmallIcon = (HICON)LoadImage(0, _T("resources/icon_small.ico"), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);

  // Check to make sure we loaded it
  if(!hMainIcon || !hSmallIcon) {
    std::cerr << "[error]: Unable to Load Icons" << std::endl;
  }

  // Create the main window class
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName = "SYSTRAYDEMO";
  wcex.lpszClassName = "SysTrayMenu";
  wcex.hIcon = hMainIcon;
  wcex.hIconSm = hSmallIcon;

  // Return our constructed class
  return RegisterClassEx(&wcex);
}


//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
  HWND hWnd;
  HICON hMainIcon;

  // Store instance handle in our global variable
  hInst = hInstance;

  // Create our windowIDM_ADMIN_PANEL
  hWnd = CreateWindow("SysTrayMenu", "SysTrayMenu", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT, (HWND) NULL, (HMENU) NULL, hInstance, (LPVOID) NULL);

  // Check to make sure the window is created
  if (!hWnd) {
    return FALSE;
  }

  // Load in our icon image
  hMainIcon = (HICON)LoadImage(0, _T("resources/icon_large.ico"), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);

  // Load the rest of the app settings
  nidApp.cbSize = sizeof(NOTIFYICONDATA); // sizeof the struct in bytes
  nidApp.hWnd = (HWND) hWnd; //handle of the window which will process this app. messages
  nidApp.uID = IDI_SYSTRAYDEMO; //ID of the icon that willl appear in the system tray
  nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; //ORing of all the flags
  nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon
  nidApp.uCallbackMessage = WM_USER_SHELLICON; 
  strcpy(nidApp.szTip, "SysTray Demo");
  Shell_NotifyIcon(NIM_ADD, &nidApp);

  // Success we have started the application
  return TRUE;
}



//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT  - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  int wmId, wmEvent;
  POINT lpClickPoint;

  // Main switch, windows event types
  switch (message) {

  // System icon has been clicked
  case WM_USER_SHELLICON: 
    switch(LOWORD(lParam)) 
    {
      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
        // Master flags for each button
        UINT uFlag = MF_BYPOSITION|MF_STRING;
        
        // Create the popup menu, which we will load buttons in
        hPopMenu = CreatePopupMenu();

        // Insert the about dialog
        InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION|MF_STRING, IDM_ADMIN_PANEL, _T("Homepage"));

        // Insert a nice separator
        InsertMenu(hPopMenu, 0xFFFFFFFF, MF_SEPARATOR, IDM_SEP, _T("SEP"));

        // If disabled is enabled, then grayout the test buttons
        if(bDisable == TRUE) {
          uFlag |= MF_GRAYED;
        }

        // Add test menus
        InsertMenu(hPopMenu, 0xFFFFFFFF, uFlag, IDM_TEST2, _T("Test 2")); // Test 2
        InsertMenu(hPopMenu, 0xFFFFFFFF, uFlag, IDM_TEST1, _T("Test 1")); // Test 1

        // Insert a nice separator
        InsertMenu(hPopMenu, 0xFFFFFFFF, MF_SEPARATOR, IDM_SEP, _T("SEP"));

        // If we are disabled, then show the enable button instead
        if(bDisable == TRUE) {
          InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION|MF_STRING, IDM_ENABLE, _T("Enable"));                  
        }
        else {
          InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION|MF_STRING, IDM_DISABLE, _T("Disable"));        
        }

        // Insert a nice separator
        InsertMenu(hPopMenu, 0xFFFFFFFF, MF_SEPARATOR, IDM_SEP, _T("SEP"));

        // Finally insert nice exit button for closing out
        InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION|MF_STRING, IDM_EXIT, _T("Exit"));

        // Display the menu, and put it near the cursor
        SetForegroundWindow(hWnd);
        GetCursorPos(&lpClickPoint);
        TrackPopupMenu(hPopMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_BOTTOMALIGN,lpClickPoint.x, lpClickPoint.y,0,hWnd,NULL);
        return true;
    }
    break;

  // A button has been clicked
  case WM_COMMAND:
    // Get the id and event
    wmId = LOWORD(wParam);
    wmEvent = HIWORD(wParam);

    // Parse the menu selections:
    switch(wmId) {
      // Admin panel, open up the url
      case IDM_ADMIN_PANEL:
        ShellExecute(NULL, "open", "http://google.com", NULL, NULL, SW_SHOWNORMAL);
        break;
      // Test button 1 has been clicked
      case IDM_TEST1:
        MessageBox(NULL, "This is a test for menu Test 1", "Test 1", MB_OK);
        break;
      // Test button 1 has been clicked
      case IDM_TEST2:
        MessageBox(NULL, "This is a test for menu Test 2", "Test 2", MB_OK);
        break;
      // Disable button have been clicked, toggle our state
      case IDM_DISABLE:
        bDisable = TRUE;
        break;
      // Enable button has been clicked, toggle our state
      case IDM_ENABLE:
        bDisable = FALSE;
        break;
      // Exit button, close the program, and destroy all parts
      case IDM_EXIT:
        Shell_NotifyIcon(NIM_DELETE,&nidApp);
        DestroyWindow(hWnd);
        break;
      default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;

  // Paint event
  //case WM_PAINT:
  //  hdc = BeginPaint(hWnd, &ps);
  //  // TODO: Add any drawing code here...
  //  EndPaint(hWnd, &ps);

  // Default exit if windows tells us to
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}


