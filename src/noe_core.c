#include "noe.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct _WindowState {
    const char *title;
    uint32_t width, height;
    bool visible, resizable, fullScreen;

    bool shouldClose;
} _WindowState;

typedef struct _ApplicationState {
    bool initialized;
    _WindowState window;
    _InputManager inputs;
} _ApplicationState;

static _ApplicationConfig appConfig = (_ApplicationConfig) {
    .window.width = 800,
    .window.height = 600,
    .window.title = "Noe Window",
    .window.visible = true,
    .window.resizable = false,
    .window.fullScreen = false,

    .opengl.version.major = 3,
    .opengl.version.minor = 3,
    .opengl.use_core_profile = true,
    .opengl.use_debug_context = false,
    .opengl.use_opengles = false,
    .opengl.use_native = true,
};

static _ApplicationState APP = {0};

#define NOE_REQUIRE_INIT_OR_RETURN_VOID(...)    \
    do {                                        \
        if(!APP.initialized) {                  \
            TRACELOG(LOG_ERROR, __VA_ARGS__);   \
            return;                             \
        }                                       \
    } while(0)

#define NOE_REQUIRE_INIT_OR_RETURN(retval, ...) \
    do {                                        \
        if(!APP.initialized) {                  \
            TRACELOG(LOG_ERROR, __VA_ARGS__);   \
            return (retval);                    \
        }                                       \
    } while(0)

bool platformInit(const _ApplicationConfig *config);
void platformDeinit(void);
void platformPollEvent(_InputManager *inputs);
void platformSetWindowTitle(const char *title);
void platformSetWindowSize(uint32_t width, uint32_t height);
void platformSetWindowVisible(bool isVisible);
void platformSetWindowResizable(bool isResizable);
void platformSetWindowFullscreen(bool isFullscreen);

bool InitApplication(void)
{
    if(APP.initialized) return false;
    if(!platformInit(&appConfig)) return false;
    APP.window.shouldClose = false;
    APP.window.title = appConfig.window.title;
    APP.window.width = appConfig.window.width;
    APP.window.height = appConfig.window.height;
    APP.window.visible = appConfig.window.visible;
    APP.window.resizable = appConfig.window.resizable;
    APP.window.fullScreen = appConfig.window.fullScreen;

    APP.initialized = true;
    return true;
}

void DeinitApplication(void)
{
    if(!APP.initialized) return;
    platformDeinit();
}

void SetWindowTitle(const char *title)
{
    NOE_REQUIRE_INIT_OR_RETURN_VOID("`SetWindowTitle()` requires you to call `InitApplication()`");
    APP.window.title = title;
    platformSetWindowTitle(title);
}

void SetWindowSize(uint32_t width, uint32_t height)
{
    NOE_REQUIRE_INIT_OR_RETURN_VOID("`SetWindowSize()` requires you to call `InitApplication()`");
    APP.window.width = width;
    APP.window.height = height;
}

void SetWindowVisible(bool isVisible)
{
    NOE_REQUIRE_INIT_OR_RETURN_VOID("`SetWindowVisible()` requires you to call `InitApplication()`");
    APP.window.visible = isVisible;
    platformSetWindowVisible(isVisible);
}

void SetWindowResizable(bool isResizable)
{
    NOE_REQUIRE_INIT_OR_RETURN_VOID("`SetWindowResizable()` requires you to call `InitApplication()`");
    APP.window.resizable = isResizable;
    platformSetWindowResizable(isResizable);
}

void SetWindowFullscreen(bool isFullscreen)
{
    NOE_REQUIRE_INIT_OR_RETURN_VOID("`SetWindowFullscreen()` requires you to call `InitApplication()`");
    APP.window.fullScreen = isFullscreen;
}

bool IsWindowVisible(void)
{
    return APP.window.visible;
}

bool IsWindowResizable(void)
{
    return APP.window.resizable;
}

bool IsWindowFullscreen(void)
{
    return APP.window.fullScreen;
}

void PollInputEvents(void)
{
    NOE_REQUIRE_INIT_OR_RETURN_VOID("`PollInputEvents()` requires you to call `InitApplication()`");
    APP.window.shouldClose = false;
    if(!APP.initialized) return;
    APP.inputs.keyboard.keyPressedQueueCount = 0;

    for (int i = 0; i < MAXIMUM_KEYBOARD_KEYS; i++)
        APP.inputs.keyboard.previousKeyState[i] = APP.inputs.keyboard.currentKeyState[i];

    for (int i = 0; i < MAXIMUM_MOUSE_BUTTONS; i++) 
        APP.inputs.mouse.previousButtonState[i] = APP.inputs.mouse.currentButtonState[i];

    // APP.inputs.mouse.previousPosition = APP.inputs.mouse.currentPosition;
    // APP.inputs.mouse.currentPosition = (Vector2){ 0.0f, 0.0f };

    // APP.inputs.mouse.previousWheelMove = APP.inputs.mouse.currentWheelMove;
    // APP.inputs.mouse.currentWheelMove = (Vector2){ 0.0f, 0.0f };

    platformPollEvent(&APP.inputs);
}

void SetWindowShouldClose(bool shouldClose)
{
    NOE_REQUIRE_INIT_OR_RETURN_VOID("`SetWindowShouldClose()` requires you to call `InitApplication()`");
    APP.window.shouldClose = shouldClose;
}

bool WindowShouldClose(void)
{
    NOE_REQUIRE_INIT_OR_RETURN(true, "`WindowShouldClose()` requires you to call `InitApplication()`");
    return APP.window.shouldClose;
}

bool IsKeyPressed(int key)
{
    NOE_REQUIRE_INIT_OR_RETURN(true, "`IsKeyPressed()` requires you to call `InitApplication()`");
    bool pressed = false;
    if((key > 0) && (key < MAXIMUM_KEYBOARD_KEYS)) {
        if ((APP.inputs.keyboard.previousKeyState[key] == 0) && (APP.inputs.keyboard.currentKeyState[key] == 1)) 
            pressed = true;
    }
    return pressed;
}

bool IsKeyDown(int key)
{
    NOE_REQUIRE_INIT_OR_RETURN(true, "`IsKeyDown()` requires you to call `InitApplication()`");
    bool down = false;
    if ((key > 0) && (key < MAXIMUM_KEYBOARD_KEYS)) {
        if (APP.inputs.keyboard.currentKeyState[key] == 1) down = true;
    }
    return down;
}

bool IsKeyReleased(int key)
{
    NOE_REQUIRE_INIT_OR_RETURN(true, "`IsKeyReleased()` requires you to call `InitApplication()`");
    bool released = false;
    if ((key > 0) && (key < MAXIMUM_KEYBOARD_KEYS)) {
        if ((APP.inputs.keyboard.previousKeyState[key] == 1) && (APP.inputs.keyboard.currentKeyState[key] == 0)) 
            released = true;
    }
    return released;
}

bool IsKeyUp(int key)
{
    NOE_REQUIRE_INIT_OR_RETURN(true, "`IsKeyUp()` requires you to call `InitApplication()`");
    bool up = false;
    if ((key > 0) && (key < MAXIMUM_KEYBOARD_KEYS)) {
        if (APP.inputs.keyboard.currentKeyState[key] == 0) up = true;
    }
    return up;
}

void SetWindowConfig(uint32_t width, uint32_t height, const char *title, uint32_t flags)
{
    appConfig.window.title = title;
    appConfig.window.width = width;
    appConfig.window.height = height;
    appConfig.window.resizable = FLAG_CHECK(WINDOW_FLAG_RESIZABLE, flags) ? 1 : 0;
    appConfig.window.fullScreen = FLAG_CHECK(WINDOW_FLAG_FULLSCREEN, flags) ? 1 : 0;
    appConfig.window.visible = FLAG_CHECK(WINDOW_FLAG_VISIBLE, flags) ? 1 : 0;
}

void *MemorySet(void *dst, int value, size_t length)
{
    for(size_t i = 0; i < (length); ++i) 
        CAST(int8_t *, dst)[i] = CAST(int8_t, value);
    return dst;
}

void *MemoryCopy(void *dst, const void *src, size_t length)
{
    for(size_t i = 0; i < (length); ++i) 
        CAST(uint8_t *, dst)[i] = CAST(const uint8_t *, src)[i];
    return dst;
}

char *StringCopy(char *dst, const char *src, size_t length)
{
    for(size_t i = 0; i < length; ++i)
        dst[i] = src[i];
    return dst;
}

size_t StringLength(const char *str)
{
    size_t i = 0;
    while(str[i++] != '\0');
    return i;
}

const char *StringFind(const char *haystack, const char *needle)
{
    if (!*needle) return (char *)haystack;

    while (*haystack) {
        const char *p1 = haystack;
        const char *p2 = needle;
        while (*p1 && *p2 && *p1 == *p2) {
            p1++;
            p2++;
        }
        if (!*p2) {
            return haystack;
        }
        haystack++;
    }

    return NULL; 
}

