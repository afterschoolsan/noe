#include "noe.h"

#include "windows.h"

typedef struct _PlatformWindowState {
    HWND hWnd;
} _PlatformWindowState;

typedef struct _PlatformDisplayState {
    HINSTANCE hInstance;
} _PlatformDisplayState;

typedef struct _PlatformState {
    bool initialized;
    _PlatformDisplayState display;
    _PlatformWindowState window;
} _PlatformState;

static _PlatformState PLATFORM = {0};

LRESULT CALLBACK win32WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool platformInit(const _ApplicationConfig *config)
{
    TRACELOG(LOG_INFO, "Initializing platform (Win32)");
    if(PLATFORM.initialized) {
        TRACELOG(LOG_ERROR, "Initializing platform failed: You have initialize the platform");
        return false;
    }
    PLATFORM.display.hInstance = GetModuleHandleA(NULL);
    if(!PLATFORM.display.hInstance) {
        TRACELOG(LOG_FATAL, "Failed to get the hInstance of this application (Win32)");
        return false;
    }

    WNDCLASSA wc;
    wc.lpszClassName = "NoeEngineWindow";
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = win32WindowProc;
    wc.hInstance = PLATFORM.display.hInstance;
    wc.hIcon = LoadIcon(PLATFORM.display.hInstance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    
    ATOM windowClass = RegisterClassA(&wc);
    if(!windowClass) {
        TRACELOG(LOG_FATAL, "Failed to register window class (Win32)");
        return false;
    }

    RECT wr;
    wr.right = config->window.width;
    wr.bottom = config->window.height;
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    HWND handle = CreateWindowExA(
            WS_EX_APPWINDOW,
            MAKEINTATOM(windowClass),
            config->window.title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            wr.right - wr.left,
            wr.bottom - wr.top,
            NULL,
            NULL,
            PLATFORM.display.hInstance,
            NULL);
        
    PLATFORM.window.hWnd = handle;

    PLATFORM.initialized = true;
    TRACELOG(LOG_INFO, "Initializing platform success (Linux)");
    return true;
}

void platformDeinit(void)
{
    DestroyWindow(PLATFORM.window.handle);
}

void platformPollEvent(_InputManager *inputs)
{
    MSG msg;

    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if(msg.message == WM_QUIT) {
            SetWindowShouldClose(true);
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
void platformSetWindowTitle(const char *title);
void platformSetWindowSize(uint32_t width, uint32_t height);
void platformSetWindowVisible(bool isVisible);
void platformSetWindowResizable(bool isResizable);
void platformSetWindowFullscreen(bool isFullscreen);
void SwapGLBuffer(void);