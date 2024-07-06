#ifndef WIN32_CONFIG_H
#define WIN32_CONFIG_H

#include<windows.h>

/*GLOBAL VARS*/
//HWND                    hwnd;      /* global var to HWND */
HINSTANCE               hInstance; /* global var to HINSTANCE */
HMODULE                 hModule;   /* global var to HMODULE */
WNDCLASS                wc  = {0};
PIXELFORMATDESCRIPTOR   pfd = {0};

UINT uMsg;
WPARAM wParam;
LPARAM lParam;

const char* className = "LEFANT";



int isWindowBorderless = 0;
int isWindowActive = 1; /* Variable to control whether the game window is active or not */
int windowWidth;        /*Width Size Var*/ 
int windowHeight;       /*Height Size Var*/





/* Função de callback para o procedimento da janela */
LRESULT CALLBACK WindowProc(HWND    hwnd,
                            UINT    uMsg,
                            WPARAM  wParam,
                            LPARAM  lParam)
{
    switch (uMsg)
    {
        case WM_SIZE:
        {
            /* Update global variables with the window width and height */
            windowWidth = LOWORD(lParam);
            windowHeight = HIWORD(lParam);

            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    default:
        return DefWindowProc(hwnd,
                             uMsg,
                             wParam,
                             lParam);
    }
    return 0;
}


void engineCreateDisplay(HWND hwnd, int width, int height)
{  
    int screenWidth = 0;
    int screenHeight = 0;
    int windowPosX = CW_USEDEFAULT;
    int windowPosY = CW_USEDEFAULT;

    /* Register the window class */
    hInstance = GetModuleHandle(NULL); 

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.lpszMenuName = NULL;

    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = className;

    if(!RegisterClass(&wc))
    {
        printf(" ERROR : RegistClass(); Win32_API");
        exit(1);
    }

    /* Obter o módulo atual */
    hModule = GetModuleHandle(NULL);


    /* Create th Window */
    hwnd = CreateWindowEx(0,
                          className,
                          "LEFAENGINE",
                          WS_OVERLAPPEDWINDOW,
                          windowPosX,           /* window PosX   */
                          windowPosY,           /* window PosY   */
                          width,                /* window Width  */
                          height,               /* window Height */
                          NULL,
                          NULL,
                          wc.hInstance,
                          NULL);

    if(hwnd == NULL)
    {
        DWORD error = GetLastError();
        printf("ERROR: CreateWindowEx() failed with error code %lu\n", error);
        exit(1);
    }


    /* Show The Window. */
    ShowWindow(hwnd, SW_SHOWDEFAULT);

}

void getWindowSize(HWND hwnd, int *width, int *height) 
{
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    *width = clientRect.right - clientRect.left;
    *height = clientRect.bottom - clientRect.top;
}

static int runEngineLoop(HWND hwnd, void (*renderFunction)(), int loopVar)
{
    /* DWORD lastFrameTime = timeGetTime(); 
    */

    /* Start Loop */
    MSG msg;
    while(loopVar) 
    {
        /* check if the window is active */
        isWindowActive = (GetForegroundWindow() == hwnd);

         //setInputEnabled(isWindowActive);

        if (isWindowActive)
        {
            //int nahx, nahy;
            //mouseInput(hwnd, TRUE, &nahx, &nahy);
        }

        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                loopVar = 0; /* Set loopVar to false to end the loop */
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        renderFunction();
    }


    DestroyWindow(hwnd);
    return msg.wParam;
}

void engineSetWindowTitle(HWND hwnd, const char* format, ...) 
{
    char title[256];
    va_list args;

    // Initialize the argument list
    va_start(args, format);

    // Format the string
    vsnprintf(title, sizeof(title), format, args);

    // Clean up the argument list
    va_end(args);

    // Set the window title
    SetWindowText(hwnd, title);
}

void engineSetWindowIcon(HWND hwnd, const char* iconPath)
{
    /* Set the window icon */
    HICON hIcon = 
        (HICON)LoadImage(GetModuleHandle(NULL),
        iconPath,
        IMAGE_ICON,
        32,
        32,
        LR_LOADFROMFILE);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
}

void windowBorderless(HWND hwnd, 
                      int windowWidth, 
                      int windowHeight)
{
    if(isWindowBorderless) 
    {
        /* If it is already borderless, restore the bordered window style */
        DWORD style = GetWindowLongPtr(hwnd, GWL_STYLE);
        style |= (WS_CAPTION | WS_THICKFRAME);
        SetWindowLongPtr(hwnd, GWL_STYLE, style);
        SetWindowPos(hwnd, 
                     HWND_TOP,
                     0, 0,
                     windowWidth, windowHeight,
                     SWP_SHOWWINDOW | SWP_FRAMECHANGED);

        isWindowBorderless = 0;
    }
    else 
    {
        /* If it has borders, remove the borders and set the size and position of the window to occupy the entire screen  */
        DWORD style = GetWindowLongPtr(hwnd, GWL_STYLE);
        style &= ~(WS_CAPTION | WS_THICKFRAME);
        SetWindowLongPtr(hwnd, GWL_STYLE, style);
        SetWindowPos(hwnd, HWND_TOP,
                     0, 0,
                     GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
                     SWP_SHOWWINDOW | SWP_FRAMECHANGED);

        isWindowBorderless = 1;
    }
}



#endif /* WIN32_CONFIG_H */
