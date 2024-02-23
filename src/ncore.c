#include "noe.h"

#include "noeplatform.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef MAXIMUM_KEYBOARD_KEYS
    #define MAXIMUM_KEYBOARD_KEYS 512
#endif
#ifndef MAXIMUM_MOUSE_BUTTONS
    #define MAXIMUM_MOUSE_BUTTONS 8
#endif
#ifndef MAXIMUM_KEYPRESSED_QUEUE
    #define MAXIMUM_KEYPRESSED_QUEUE 16
#endif

typedef struct ApplicationState {
    bool initialized;

    struct {
        NplWindow *handle;
        const char *title;
        uint32_t width, height;
        bool shouldClose;
    } window;

    struct {
        struct {
            char currentKeyState[MAXIMUM_KEYBOARD_KEYS];
            char previousKeyState[MAXIMUM_KEYBOARD_KEYS];

            int keyPressedQueue[MAXIMUM_KEYPRESSED_QUEUE];
            int keyPressedQueueCount;
        } keyboard;
    } inputs;
} ApplicationState;

static ApplicationState APP = {
    .window.width = 800,
    .window.height = 600,
    .window.title = "Noe Window",
};

#define NOE_REQUIRE_INITIALIZATION(retval, ...)   \
        if(!APP.initialized) {                      \
            TRACELOG(LOG_ERROR, __VA_ARGS__);       \
            return (retval);                        \
        }                                           \

void nplLogCallback(int level, const char *message);
void nplWindowEventCallback(NplWindow *window, NplEvent event, void *userData);

bool InitApplication(void)
{
    if(APP.initialized) return false;
    if(!nplInit(nplLogCallback)) return false;

    APP.window.handle = nplCreateWindow(APP.window.width, APP.window.height, 0, 0, APP.window.title);
    APP.window.shouldClose = false;
    nplSetWindowEventCallback(APP.window.handle, nplWindowEventCallback, &APP);

    APP.initialized = true;
    return true;
}

void DeinitApplication(void)
{
    if(!APP.initialized) return;

    nplDeinit();
}

void PollInputEvents(void)
{
    if(!APP.initialized) return;
    APP.inputs.keyboard.keyPressedQueueCount = 0;

    for (int i = 0; i < MAXIMUM_KEYBOARD_KEYS; i++) {
        APP.inputs.keyboard.previousKeyState[i] = APP.inputs.keyboard.currentKeyState[i];
        // APP.inputs.keyboard.keyRepeatInFrame[i] = 0;
    }

    // for (int i = 0; i < MAXIMUM_MOUSE_BUTTONS; i++) 
    //     APP.inputs.mouse.previousButtonState[i] = APP.inputs.mouse.currentButtonState[i];

    // APP.inputs.mouse.previousPosition = APP.inputs.mouse.currentPosition;
    // APP.inputs.mouse.currentPosition = (Vector2){ 0.0f, 0.0f };

    // APP.inputs.mouse.previousWheelMove = APP.inputs.mouse.currentWheelMove;
    // APP.inputs.mouse.currentWheelMove = (Vector2){ 0.0f, 0.0f };

    nplPollEvent();
}

bool WindowShouldClose(void)
{
    return APP.window.shouldClose;
}

bool IsKeyPressed(int key)
{
    bool pressed = false;
    if((key > 0) && (key < MAXIMUM_KEYBOARD_KEYS)) {
        if ((APP.inputs.keyboard.previousKeyState[key] == 0) && (APP.inputs.keyboard.currentKeyState[key] == 1)) 
            pressed = true;
    }
    return pressed;
}

bool IsKeyDown(int key)
{
    bool down = false;
    if ((key > 0) && (key < MAXIMUM_KEYBOARD_KEYS)) {
        if (APP.inputs.keyboard.currentKeyState[key] == 1) down = true;
    }
    return down;
}

bool IsKeyReleased(int key)
{
    bool released = false;
    if ((key > 0) && (key < MAXIMUM_KEYBOARD_KEYS)) {
        if ((APP.inputs.keyboard.previousKeyState[key] == 1) && (APP.inputs.keyboard.currentKeyState[key] == 0)) 
            released = true;
    }
    return released;
}

bool IsKeyUp(int key)
{
    bool up = false;
    if ((key > 0) && (key < MAXIMUM_KEYBOARD_KEYS)) {
        if (APP.inputs.keyboard.currentKeyState[key] == 0) up = true;
    }
    return up;
}

void SetWindowConfig(uint32_t width, uint32_t height, const char *title)
{
    APP.window.title = title;
    APP.window.width = width;
    APP.window.height = height;
}

void nplLogCallback(int level, const char *message)
{
    switch(level) {
        case NPL_LOG_INFO:
            TRACELOG(LOG_INFO, "%s", message);
            break;
        case NPL_LOG_WARNING:
            TRACELOG(LOG_WARNING, "%s", message);
            break;
        case NPL_LOG_FATAL:
            TRACELOG(LOG_FATAL, "%s", message);
            break;
        default:
            break;
    }
}

void nplWindowEventCallback(NplWindow *window, NplEvent event, void *userData)
{
    (void)window;
    (void)userData;
    ApplicationState *APP = (ApplicationState *)userData;

    switch(event.type) {
        case NPL_EVENT_WINDOW_CLOSE:
            {
                APP->window.shouldClose = true;
            } break;
        case NPL_EVENT_KEY_PRESSED:
        case NPL_EVENT_KEY_RELEASED:
            {
                if(event.key.code < 0) return;
                if (event.type == NPL_EVENT_KEY_PRESSED) APP->inputs.keyboard.currentKeyState[event.key.code] = 1;
                else if(event.type == NPL_EVENT_KEY_RELEASED) APP->inputs.keyboard.currentKeyState[event.key.code] = 0;

                // Check if there is space available in the key queue
                if ((APP->inputs.keyboard.keyPressedQueueCount < MAXIMUM_KEYPRESSED_QUEUE) 
                        && (event.type == NPL_EVENT_KEY_PRESSED)) {
                    APP->inputs.keyboard.keyPressedQueue[APP->inputs.keyboard.keyPressedQueueCount] = event.key.code;
                    APP->inputs.keyboard.keyPressedQueueCount++;
                }
            } break;
        default:
            break;
    }
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

#define LOG_MESSAGE_MAXIMUM_LENGTH (32*1024)
static const char *logLevelsAsText[] = {
    "[INFO] ",
    "[WARNING] ",
    "[ERROR] ",
    "[FATAL] ",
};

void TraceLog(int logLevel, const char *fmt, ...)
{
#warning "`TraceLog()` still using `printf()` from libc`
    if(!(LOG_INFO <= logLevel && logLevel <= LOG_FATAL)) return;
    char logMessage[LOG_MESSAGE_MAXIMUM_LENGTH] = {0};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(logMessage, LOG_MESSAGE_MAXIMUM_LENGTH, fmt, ap);
    va_end(ap);
    if(logLevel == LOG_FATAL || logLevel == LOG_ERROR) fprintf(stderr, "%s %s\n", logLevelsAsText[logLevel], logMessage);
    else printf("%s %s\n", logLevelsAsText[logLevel], logMessage);
}

void *MemoryAlloc(size_t nBytes)
{
#warning "`MemoryAlloc()` still using `malloc()` from libc`
    void *result = malloc(nBytes);
    if(!result) return NULL;
    return result;
}

void MemoryFree(void *ptr)
{
#warning "`MemoryFree()` still using `free()` from libc`
    if(ptr) free(ptr);
}

