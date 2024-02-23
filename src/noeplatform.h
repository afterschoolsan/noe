#ifndef NOE_PLATFORM_H_
#define NOE_PLATFORM_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(_WIN32)
    #define NPL_PLATFORM_WINDOWS
#elif defined(__ANDROID__)
    #define NPL_PLATFORM_ANDROID
#elif defined(__linux__) || defined(__gnu_linux__)
    #define NPL_PLATFORM_LINUX
#else
    #error "Unsupported platform"
#endif

#ifdef NPL_PLATFORM_LINUX
    #if !defined(NPL_LINUX_DISPLAY_X11) && !defined(NPL_LINUX_DISPLAY_WAYLAND)
        #define NPL_LINUX_DISPLAY_X11
    #endif
#endif

#define NAPI
#define NPL_MAXIMUM_WINDOWS 8

typedef struct NplWindow NplWindow;

typedef void (*PFNNPLLOGCALLBACK)(int level, const char *message);

typedef struct NplKeyEvent {
    int scancode;
    int code;
    int mods;
} NplKeyEvent;

typedef struct NplEvent {
    int type;
    union {
        NplKeyEvent key;
    };
} NplEvent;

typedef void (*PFNNPLWINDOWEVENTCALLBACK)(NplWindow *window, NplEvent event, void *userData);

NAPI bool nplInit(PFNNPLLOGCALLBACK callback);
NAPI void nplDeinit(void);

NAPI NplWindow *nplCreateWindow(uint32_t width, uint32_t height, int x, int y, const char *title);
NAPI void nplDestroyWindow(NplWindow *window);
NAPI void nplSetWindowEventCallback(NplWindow *window, PFNNPLWINDOWEVENTCALLBACK callback, void *userData);
NAPI void nplPollEvent(void);

NAPI void nplConsoleWrite(const char *message, size_t messageLength);
NAPI void nplConsoleWriteError(const char *message, size_t messageLength);

typedef enum NplLogLevel {
    NPL_LOG_INFO = 0,
    NPL_LOG_WARNING,
    NPL_LOG_FATAL,
} NplLogLevel;

typedef enum NplEventType {
    NPL_INVALID_EVENT = 0,
    NPL_EVENT_WINDOW_CLOSE,
    NPL_EVENT_KEY_PRESSED,
    NPL_EVENT_KEY_RELEASED,
} NplEventType;

typedef enum NplKeyCode {
    NPL_INVALID_KEY            = 0,

    /* Printable keys */
    NPL_KEY_SPACE              = 32,
    NPL_KEY_APOSTROPHE         = 39,  /* ' */
    NPL_KEY_COMMA              = 44,  /* , */
    NPL_KEY_MINUS              = 45,  /* - */
    NPL_KEY_PERIOD             = 46,  /* . */
    NPL_KEY_SLASH              = 47,  /* / */
    NPL_KEY_0                  = 48,
    NPL_KEY_1                  = 49,
    NPL_KEY_2                  = 50,
    NPL_KEY_3                  = 51,
    NPL_KEY_4                  = 52,
    NPL_KEY_5                  = 53,
    NPL_KEY_6                  = 54,
    NPL_KEY_7                  = 55,
    NPL_KEY_8                  = 56,
    NPL_KEY_9                  = 57,
    NPL_KEY_SEMICOLON          = 59,  /* ; */
    NPL_KEY_EQUAL              = 61,  /* = */
    NPL_KEY_A                  = 65,
    NPL_KEY_B                  = 66,
    NPL_KEY_C                  = 67,
    NPL_KEY_D                  = 68,
    NPL_KEY_E                  = 69,
    NPL_KEY_F                  = 70,
    NPL_KEY_G                  = 71,
    NPL_KEY_H                  = 72,
    NPL_KEY_I                  = 73,
    NPL_KEY_J                  = 74,
    NPL_KEY_K                  = 75,
    NPL_KEY_L                  = 76,
    NPL_KEY_M                  = 77,
    NPL_KEY_N                  = 78,
    NPL_KEY_O                  = 79,
    NPL_KEY_P                  = 80,
    NPL_KEY_Q                  = 81,
    NPL_KEY_R                  = 82,
    NPL_KEY_S                  = 83,
    NPL_KEY_T                  = 84,
    NPL_KEY_U                  = 85,
    NPL_KEY_V                  = 86,
    NPL_KEY_W                  = 87,
    NPL_KEY_X                  = 88,
    NPL_KEY_Y                  = 89,
    NPL_KEY_Z                  = 90,
    NPL_KEY_LEFT_BRACKET       = 91,  /* [ */
    NPL_KEY_BACKSLASH          = 92,  /* \ */
    NPL_KEY_RIGHT_BRACKET      = 93,  /* ] */
    NPL_KEY_GRAVE_ACCENT       = 96,  /* ` */
    NPL_KEY_WORLD_1            = 161, /* non-US #1 */
    NPL_KEY_WORLD_2            = 162, /* non-US #2 */

    /* Function keys */
    NPL_KEY_ESCAPE             = 256,
    NPL_KEY_ENTER              = 257,
    NPL_KEY_TAB                = 258,
    NPL_KEY_BACKSPACE          = 259,
    NPL_KEY_INSERT             = 260,
    NPL_KEY_DELETE             = 261,
    NPL_KEY_RIGHT              = 262,
    NPL_KEY_LEFT               = 263,
    NPL_KEY_DOWN               = 264,
    NPL_KEY_UP                 = 265,
    NPL_KEY_PAGE_UP            = 266,
    NPL_KEY_PAGE_DOWN          = 267,
    NPL_KEY_HOME               = 268,
    NPL_KEY_END                = 269,
    NPL_KEY_CAPS_LOCK          = 280,
    NPL_KEY_SCROLL_LOCK        = 281,
    NPL_KEY_NUM_LOCK           = 282,
    NPL_KEY_PRINT_SCREEN       = 283,
    NPL_KEY_PAUSE              = 284,
    NPL_KEY_F1                 = 290,
    NPL_KEY_F2                 = 291,
    NPL_KEY_F3                 = 292,
    NPL_KEY_F4                 = 293,
    NPL_KEY_F5                 = 294,
    NPL_KEY_F6                 = 295,
    NPL_KEY_F7                 = 296,
    NPL_KEY_F8                 = 297,
    NPL_KEY_F9                 = 298,
    NPL_KEY_F10                = 299,
    NPL_KEY_F11                = 300,
    NPL_KEY_F12                = 301,
    NPL_KEY_F13                = 302,
    NPL_KEY_F14                = 303,
    NPL_KEY_F15                = 304,
    NPL_KEY_F16                = 305,
    NPL_KEY_F17                = 306,
    NPL_KEY_F18                = 307,
    NPL_KEY_F19                = 308,
    NPL_KEY_F20                = 309,
    NPL_KEY_F21                = 310,
    NPL_KEY_F22                = 311,
    NPL_KEY_F23                = 312,
    NPL_KEY_F24                = 313,
    NPL_KEY_F25                = 314,
    NPL_KEY_KP_0               = 320,
    NPL_KEY_KP_1               = 321,
    NPL_KEY_KP_2               = 322,
    NPL_KEY_KP_3               = 323,
    NPL_KEY_KP_4               = 324,
    NPL_KEY_KP_5               = 325,
    NPL_KEY_KP_6               = 326,
    NPL_KEY_KP_7               = 327,
    NPL_KEY_KP_8               = 328,
    NPL_KEY_KP_9               = 329,
    NPL_KEY_KP_DECIMAL         = 330,
    NPL_KEY_KP_DIVIDE          = 331,
    NPL_KEY_KP_MULTIPLY        = 332,
    NPL_KEY_KP_SUBTRACT        = 333,
    NPL_KEY_KP_ADD             = 334,
    NPL_KEY_KP_ENTER           = 335,
    NPL_KEY_KP_EQUAL           = 336,
    NPL_KEY_LEFT_SHIFT         = 340,
    NPL_KEY_LEFT_CONTROL       = 341,
    NPL_KEY_LEFT_ALT           = 342,
    NPL_KEY_LEFT_SUPER         = 343,
    NPL_KEY_RIGHT_SHIFT        = 344,
    NPL_KEY_RIGHT_CONTROL      = 345,
    NPL_KEY_RIGHT_ALT          = 346,
    NPL_KEY_RIGHT_SUPER        = 347,
    NPL_KEY_MENU               = 348,
    NPL_KEY_LAST               = NPL_KEY_MENU,
} NplKeyCode;

typedef enum NoeKeyMods {
    NPL_KEY_MOD_SHIFT      = 0x0001,
    NPL_KEY_MOD_CONTROL    = 0x0002,
    NPL_KEY_MOD_ALT        = 0x0004,
    NPL_KEY_MOD_SUPER      = 0x0008,
    NPL_KEY_MOD_CAPSLOCK   = 0x0010,
    NPL_KEY_MOD_NUMLOCK    = 0x0020,
} NoeKeyMods;

#endif // NOE_PLATFORM_H_
