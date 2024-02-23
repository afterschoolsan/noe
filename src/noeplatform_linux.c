#include "noeplatform.h"

#include <stdint.h>
#include <string.h>
#include <unistd.h>

#if defined(NPL_LINUX_DISPLAY_X11)
    #include <X11/Xlib.h>
    #include <X11/keysym.h>

    bool initX11(void);
    void deinitX11(void);
    bool initX11Window(NplWindow *window, uint32_t w,uint32_t h, int x, int y, const char *title);
    void deinitX11Window(NplWindow *window);
    void pollX11Event(void);
#elif defined(NPL_PLATFORM_WAYLAND)
    #error "Wayland display is not implemented yet"
#else
    #error "Unsupported linux display module"
#endif

typedef struct NplWindow {
    struct {
        Display *display;
        Window handle;
        Colormap colormap;
        Atom wmDeleteWindow;
    } x11;
    struct {
        PFNNPLWINDOWEVENTCALLBACK callback;
        void *userData;
    } event;
    bool initialized;
} NplWindow;

typedef struct NplPlatformContext{
    bool initialized;
    PFNNPLLOGCALLBACK traceLog;
    struct {
        Display *display;
        Screen *defaultScreen;
        int defaultScreenID;
        Window defaultRootWindow;
        int keycodes[256];
        int scancodes[NPL_KEY_LAST];
    } x11;

    NplWindow windows[NPL_MAXIMUM_WINDOWS];
} NplPlatformContext;

static NplPlatformContext PLATFORM;

#ifndef NDEBUG
#define NPL_PLATFORM_INFO(message) if(PLATFORM.traceLog) PLATFORM.traceLog(NPL_LOG_INFO, message)
#define NPL_PLATFORM_WARNING(message) if(PLATFORM.traceLog) PLATFORM.traceLog(NPL_LOG_WARNING, message)
#define NPL_PLATFORM_FATAL(message) if(PLATFORM.traceLog) PLATFORM.traceLog(NPL_LOG_FATAL, message)
#else
#define NPL_PLATFORM_INFO(message)
#define NPL_PLATFORM_WARNING(message)
#define NPL_PLATFORM_FATAL(message)
#endif

#define NPL_REQUIRE_INITIALIZATION_VOID(message) \
    do {                                            \
        if(!PLATFORM.initialized) {                 \
            NPL_PLATFORM_FATAL(message);            \
            return;                                 \
        }                                           \
    } while(0)

#define NPL_REQUIRE_INITIALIZATION(message, retval) \
    do {                                            \
        if(!PLATFORM.initialized) {                 \
            NPL_PLATFORM_FATAL(message);            \
            return (retval);                        \
        }                                           \
    } while(0)

bool nplInit(PFNNPLLOGCALLBACK callback)
{
    if(!callback) return false;
    PLATFORM.traceLog = callback;

    NPL_PLATFORM_INFO("Initializing platform (linux)");
    if(PLATFORM.initialized) {
        NPL_PLATFORM_FATAL("Initializing platform failed: You have initialize the platform");
        return false;
    }

#if defined(NPL_LINUX_DISPLAY_X11)
    NPL_PLATFORM_INFO("Initializing X11");
    if(!initX11()) {
        NPL_PLATFORM_FATAL("Initializing X11 failed");
        return false;
    }
    NPL_PLATFORM_FATAL("Initializing X11 success");
#elif defined(NPL_PLATFORM_WAYLAND)
    NPL_PLATFORM_INFO("Initializing linux display (wayland)");
    #error "Wayland display is not implemented yet"
#else
    #error "Unsupported linux display module"
#endif

    for(uint32_t i = 0; i < NPL_MAXIMUM_WINDOWS; ++i) PLATFORM.windows[i].initialized = false;

    NPL_PLATFORM_FATAL("Initializing platform success");
    PLATFORM.initialized = true;
    return true;
}

void nplDeinit(void)
{
    NPL_PLATFORM_INFO("Deinitializing platform (linux)");
    if(!PLATFORM.initialized) return;
#if defined(NPL_LINUX_DISPLAY_X11)
    deinitX11();
#elif defined(NPL_PLATFORM_WAYLAND)
    #error "Wayland display is not implemented yet"
#else
    #error "Unsupported linux display module"
#endif
}

NplWindow *nplCreateWindow(uint32_t w,uint32_t h, int x, int y, const char *title)
{
    NPL_REQUIRE_INITIALIZATION("`nplCreateWindow()` requires you to call `nplInit()`", NULL);
    NPL_PLATFORM_INFO("Creating window");
    NplWindow *window = NULL;
    for(uint32_t i = 0; i < NPL_MAXIMUM_WINDOWS; ++i) {
        if(!PLATFORM.windows[i].initialized)
            window = &PLATFORM.windows[i];
    }

    if(!window) return NULL;
#if defined(NPL_LINUX_DISPLAY_X11)
    if(!initX11Window(window, w, h, x, y, title)) {
        NPL_PLATFORM_FATAL("Creating window failed");
        return NULL;
    }
#elif defined(NPL_PLATFORM_WAYLAND)
    #error "Wayland display is not implemented yet"
#else
    #error "Unsupported linux display module"
#endif

    NPL_PLATFORM_INFO("Creating window success");
    window->initialized = true;
    return window;
}

void nplDestroyWindow(NplWindow *window)
{
    NPL_PLATFORM_INFO("Destroying window");
    NPL_REQUIRE_INITIALIZATION_VOID("`nplDestroyWindow()` requires you to call `nplInit()`");
#if defined(NPL_LINUX_DISPLAY_X11)
    deinitX11Window(window);
#elif defined(NPL_PLATFORM_WAYLAND)
    #error "Wayland display is not implemented yet"
#else
    #error "Unsupported linux display module"
#endif
    window->event.callback = NULL;
    window->initialized = false;
}

void nplSetWindowEventCallback(NplWindow *window, PFNNPLWINDOWEVENTCALLBACK callback, void *userData)
{
    NPL_REQUIRE_INITIALIZATION_VOID("`nplSetWindowEventCallback()` requires you to call `nplInit()`");

    window->event.callback = callback;
    window->event.userData = userData;
}

void nplPollEvent(void)
{
    NPL_REQUIRE_INITIALIZATION_VOID("`nplPollEvent()` requires you to call `nplInit()`");
#if defined(NPL_LINUX_DISPLAY_X11)
    pollX11Event();
#elif defined(NPL_PLATFORM_WAYLAND)
    #error "Wayland display is not implemented yet"
#else
    #error "Unsupported linux display module"
#endif
}

void nplConsoleWrite(const char *message, size_t messageLength)
{
    write(STDOUT_FILENO, message, messageLength);
}

void nplConsoleWriteError(const char *message, size_t messageLength)
{
    write(STDERR_FILENO, message, messageLength);
}


#if defined(NPL_LINUX_DISPLAY_X11)
static int translateKeySyms(const KeySym* keysyms, int width)
{
    if (width > 1)
    {
        switch (keysyms[1])
        {
            case XK_KP_0:           return NPL_KEY_KP_0;
            case XK_KP_1:           return NPL_KEY_KP_1;
            case XK_KP_2:           return NPL_KEY_KP_2;
            case XK_KP_3:           return NPL_KEY_KP_3;
            case XK_KP_4:           return NPL_KEY_KP_4;
            case XK_KP_5:           return NPL_KEY_KP_5;
            case XK_KP_6:           return NPL_KEY_KP_6;
            case XK_KP_7:           return NPL_KEY_KP_7;
            case XK_KP_8:           return NPL_KEY_KP_8;
            case XK_KP_9:           return NPL_KEY_KP_9;
            case XK_KP_Separator:
            case XK_KP_Decimal:     return NPL_KEY_KP_DECIMAL;
            case XK_KP_Equal:       return NPL_KEY_KP_EQUAL;
            case XK_KP_Enter:       return NPL_KEY_KP_ENTER;
            default:                break;
        }
    }

    switch (keysyms[0])
    {
        case XK_Escape:         return NPL_KEY_ESCAPE;
        case XK_Tab:            return NPL_KEY_TAB;
        case XK_Shift_L:        return NPL_KEY_LEFT_SHIFT;
        case XK_Shift_R:        return NPL_KEY_RIGHT_SHIFT;
        case XK_Control_L:      return NPL_KEY_LEFT_CONTROL;
        case XK_Control_R:      return NPL_KEY_RIGHT_CONTROL;
        case XK_Meta_L:
        case XK_Alt_L:          return NPL_KEY_LEFT_ALT;
        case XK_Mode_switch: // Mapped to Alt_R on many keyboards
        case XK_ISO_Level3_Shift: // AltGr on at least some machines
        case XK_Meta_R:
        case XK_Alt_R:          return NPL_KEY_RIGHT_ALT;
        case XK_Super_L:        return NPL_KEY_LEFT_SUPER;
        case XK_Super_R:        return NPL_KEY_RIGHT_SUPER;
        case XK_Menu:           return NPL_KEY_MENU;
        case XK_Num_Lock:       return NPL_KEY_NUM_LOCK;
        case XK_Caps_Lock:      return NPL_KEY_CAPS_LOCK;
        case XK_Print:          return NPL_KEY_PRINT_SCREEN;
        case XK_Scroll_Lock:    return NPL_KEY_SCROLL_LOCK;
        case XK_Pause:          return NPL_KEY_PAUSE;
        case XK_Delete:         return NPL_KEY_DELETE;
        case XK_BackSpace:      return NPL_KEY_BACKSPACE;
        case XK_Return:         return NPL_KEY_ENTER;
        case XK_Home:           return NPL_KEY_HOME;
        case XK_End:            return NPL_KEY_END;
        case XK_Page_Up:        return NPL_KEY_PAGE_UP;
        case XK_Page_Down:      return NPL_KEY_PAGE_DOWN;
        case XK_Insert:         return NPL_KEY_INSERT;
        case XK_Left:           return NPL_KEY_LEFT;
        case XK_Right:          return NPL_KEY_RIGHT;
        case XK_Down:           return NPL_KEY_DOWN;
        case XK_Up:             return NPL_KEY_UP;
        case XK_F1:             return NPL_KEY_F1;
        case XK_F2:             return NPL_KEY_F2;
        case XK_F3:             return NPL_KEY_F3;
        case XK_F4:             return NPL_KEY_F4;
        case XK_F5:             return NPL_KEY_F5;
        case XK_F6:             return NPL_KEY_F6;
        case XK_F7:             return NPL_KEY_F7;
        case XK_F8:             return NPL_KEY_F8;
        case XK_F9:             return NPL_KEY_F9;
        case XK_F10:            return NPL_KEY_F10;
        case XK_F11:            return NPL_KEY_F11;
        case XK_F12:            return NPL_KEY_F12;
        case XK_F13:            return NPL_KEY_F13;
        case XK_F14:            return NPL_KEY_F14;
        case XK_F15:            return NPL_KEY_F15;
        case XK_F16:            return NPL_KEY_F16;
        case XK_F17:            return NPL_KEY_F17;
        case XK_F18:            return NPL_KEY_F18;
        case XK_F19:            return NPL_KEY_F19;
        case XK_F20:            return NPL_KEY_F20;
        case XK_F21:            return NPL_KEY_F21;
        case XK_F22:            return NPL_KEY_F22;
        case XK_F23:            return NPL_KEY_F23;
        case XK_F24:            return NPL_KEY_F24;
        case XK_F25:            return NPL_KEY_F25;

        // Numeric keypad
        case XK_KP_Divide:      return NPL_KEY_KP_DIVIDE;
        case XK_KP_Multiply:    return NPL_KEY_KP_MULTIPLY;
        case XK_KP_Subtract:    return NPL_KEY_KP_SUBTRACT;
        case XK_KP_Add:         return NPL_KEY_KP_ADD;

        // These should have been detected in secondary keysym test above!
        case XK_KP_Insert:      return NPL_KEY_KP_0;
        case XK_KP_End:         return NPL_KEY_KP_1;
        case XK_KP_Down:        return NPL_KEY_KP_2;
        case XK_KP_Page_Down:   return NPL_KEY_KP_3;
        case XK_KP_Left:        return NPL_KEY_KP_4;
        case XK_KP_Right:       return NPL_KEY_KP_6;
        case XK_KP_Home:        return NPL_KEY_KP_7;
        case XK_KP_Up:          return NPL_KEY_KP_8;
        case XK_KP_Page_Up:     return NPL_KEY_KP_9;
        case XK_KP_Delete:      return NPL_KEY_KP_DECIMAL;
        case XK_KP_Equal:       return NPL_KEY_KP_EQUAL;
        case XK_KP_Enter:       return NPL_KEY_KP_ENTER;

        // Last resort: Check for printable keys (should not happen if the XKB
        // extension is available). This will give a layout dependent mapping
        // (which is wrong, and we may miss some keys, especially on non-US
        // keyboards), but it's better than nothing...
        case XK_a:              return NPL_KEY_A;
        case XK_b:              return NPL_KEY_B;
        case XK_c:              return NPL_KEY_C;
        case XK_d:              return NPL_KEY_D;
        case XK_e:              return NPL_KEY_E;
        case XK_f:              return NPL_KEY_F;
        case XK_g:              return NPL_KEY_G;
        case XK_h:              return NPL_KEY_H;
        case XK_i:              return NPL_KEY_I;
        case XK_j:              return NPL_KEY_J;
        case XK_k:              return NPL_KEY_K;
        case XK_l:              return NPL_KEY_L;
        case XK_m:              return NPL_KEY_M;
        case XK_n:              return NPL_KEY_N;
        case XK_o:              return NPL_KEY_O;
        case XK_p:              return NPL_KEY_P;
        case XK_q:              return NPL_KEY_Q;
        case XK_r:              return NPL_KEY_R;
        case XK_s:              return NPL_KEY_S;
        case XK_t:              return NPL_KEY_T;
        case XK_u:              return NPL_KEY_U;
        case XK_v:              return NPL_KEY_V;
        case XK_w:              return NPL_KEY_W;
        case XK_x:              return NPL_KEY_X;
        case XK_y:              return NPL_KEY_Y;
        case XK_z:              return NPL_KEY_Z;
        case XK_1:              return NPL_KEY_1;
        case XK_2:              return NPL_KEY_2;
        case XK_3:              return NPL_KEY_3;
        case XK_4:              return NPL_KEY_4;
        case XK_5:              return NPL_KEY_5;
        case XK_6:              return NPL_KEY_6;
        case XK_7:              return NPL_KEY_7;
        case XK_8:              return NPL_KEY_8;
        case XK_9:              return NPL_KEY_9;
        case XK_0:              return NPL_KEY_0;
        case XK_space:          return NPL_KEY_SPACE;
        case XK_minus:          return NPL_KEY_MINUS;
        case XK_equal:          return NPL_KEY_EQUAL;
        case XK_bracketleft:    return NPL_KEY_LEFT_BRACKET;
        case XK_bracketright:   return NPL_KEY_RIGHT_BRACKET;
        case XK_backslash:      return NPL_KEY_BACKSLASH;
        case XK_semicolon:      return NPL_KEY_SEMICOLON;
        case XK_apostrophe:     return NPL_KEY_APOSTROPHE;
        case XK_grave:          return NPL_KEY_GRAVE_ACCENT;
        case XK_comma:          return NPL_KEY_COMMA;
        case XK_period:         return NPL_KEY_PERIOD;
        case XK_slash:          return NPL_KEY_SLASH;
        case XK_less:           return NPL_KEY_WORLD_1; // At least in some layouts...
        default:                break;
    }

    // No matching translation was found
    return NPL_INVALID_KEY;
}

static void setupX11KeyMaps(void)
{
    int scancodeMin, scancodeMax;

    memset(PLATFORM.x11.keycodes, -1, sizeof(PLATFORM.x11.keycodes));
    memset(PLATFORM.x11.scancodes, -1, sizeof(PLATFORM.x11.scancodes));

    XDisplayKeycodes(PLATFORM.x11.display, &scancodeMin, &scancodeMax);

    int width;
    KeySym* keysyms = XGetKeyboardMapping(PLATFORM.x11.display,
                                          scancodeMin,
                                          scancodeMax - scancodeMin + 1,
                                          &width);

    for (int scancode = scancodeMin;  scancode <= scancodeMax;  scancode++)
    {
        // Translate the un-translated key codes using traditional X11 KeySym
        // lookups
        if (PLATFORM.x11.keycodes[scancode] < 0)
        {
            const size_t base = (scancode - scancodeMin) * width;
            PLATFORM.x11.keycodes[scancode] = translateKeySyms(&keysyms[base], width);
        }

        // Store the reverse translation for faster key name lookup
        if (PLATFORM.x11.keycodes[scancode] > 0)
            PLATFORM.x11.scancodes[PLATFORM.x11.keycodes[scancode]] = scancode;
    }

    XFree(keysyms);
}

bool initX11(void)
{
    Display *dpy = XOpenDisplay(NULL);

    if(!dpy) {
        NPL_PLATFORM_FATAL("Failed to initialize native display system (X11 Display)");
        return false;
    }
    
    PLATFORM.x11.defaultScreen = XDefaultScreenOfDisplay(dpy);
    PLATFORM.x11.defaultScreenID = XDefaultScreen(dpy);
    PLATFORM.x11.defaultRootWindow = XRootWindow(dpy, PLATFORM.x11.defaultScreenID);
    PLATFORM.x11.display = dpy;
    setupX11KeyMaps();
    return true;
}

void deinitX11(void)
{
    XCloseDisplay(PLATFORM.x11.display);
}

bool initX11Window(NplWindow *window, uint32_t w,uint32_t h, int x, int y, const char *title)
{
    Window handle;
    Colormap colormap = XCreateColormap(PLATFORM.x11.display, PLATFORM.x11.defaultRootWindow, 
            XDefaultVisualOfScreen(PLATFORM.x11.defaultScreen), AllocNone);
    Atom wmDeleteWindow = XInternAtom(PLATFORM.x11.display, "WM_DELETE_WINDOW", False);

    XSetWindowAttributes swa;
    swa.override_redirect = True;
    swa.border_pixel = None;
    swa.background_pixel = XBlackPixel(PLATFORM.x11.display, PLATFORM.x11.defaultScreenID);
    swa.colormap = colormap;
    swa.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask |
                    PointerMotionMask | ButtonPressMask | ButtonReleaseMask |
                    ExposureMask | FocusChangeMask | VisibilityChangeMask |
                    EnterWindowMask | LeaveWindowMask | PropertyChangeMask;

    handle = XCreateWindow(PLATFORM.x11.display, PLATFORM.x11.defaultRootWindow,
            x, y, w, h, 0,
            CopyFromParent, InputOutput,
            CopyFromParent, 
            CWBackPixel | CWColormap | CWBorderPixel | CWEventMask,
            &swa);

    if(!handle) {
        NPL_PLATFORM_FATAL("Failed to create native window (X11 Window)");
        return false;
    }

    XSetWMProtocols(PLATFORM.x11.display, handle, &wmDeleteWindow, 1);
    XStoreName(PLATFORM.x11.display, handle, title);
    XMapWindow(PLATFORM.x11.display, handle);

    window->x11.display = PLATFORM.x11.display;
    window->x11.handle = handle;
    window->x11.colormap = colormap;
    window->x11.wmDeleteWindow = wmDeleteWindow;

    XEvent event = {0};
    XNextEvent(PLATFORM.x11.display, &event);
    return true;
}

void deinitX11Window(NplWindow *window)
{
    XFreeColormap(window->x11.display, window->x11.colormap);
    XDestroyWindow(window->x11.display, window->x11.handle);
}

int translateX11Key(int scancode) {
    if (scancode < 0 || scancode > 255)
        return NPL_INVALID_KEY;

    return PLATFORM.x11.keycodes[scancode];
}

int translateX11KeyState(int state) {
    int mods = 0;

    if (state & ShiftMask)
        mods |= NPL_KEY_MOD_SHIFT;
    if (state & ControlMask)
        mods |= NPL_KEY_MOD_CONTROL;
    if (state & Mod1Mask)
        mods |= NPL_KEY_MOD_ALT;
    if (state & Mod4Mask)
        mods |= NPL_KEY_MOD_SUPER;
    if (state & LockMask)
        mods |= NPL_KEY_MOD_CAPSLOCK;
    if (state & Mod2Mask)
        mods |= NPL_KEY_MOD_NUMLOCK;

    return mods;
}

void pollX11Event(void)
{
    XEvent event = {0};
    NplEvent result;
    while(XPending(PLATFORM.x11.display) > 0) {
        XNextEvent(PLATFORM.x11.display, &event);
        int scancode = 0;
        bool filtered = False;
        // HACK: Save scancode as some IMs clear the field in XFilterEvent
        if (event.type == KeyPress || event.type == KeyRelease) scancode = event.xkey.keycode;
        filtered = XFilterEvent(&event, None);

        switch(event.type) {
            case ClientMessage:
                {
                    for(int i = 0; i < NPL_MAXIMUM_WINDOWS; ++i) {
                        NplWindow *window = &PLATFORM.windows[i];
                        if((Atom)event.xclient.data.l[0] == window->x11.wmDeleteWindow) {
                            result.type = NPL_EVENT_WINDOW_CLOSE;
                            window->event.callback(window, result, window->event.userData);

                            // still doubt about this
                            return;
                        }
                    }
                } break;
            case KeyPress:
                {
                    result.key.scancode = scancode;
                    result.key.mods = translateX11KeyState(event.xkey.state);
                    result.key.code= translateX11Key(scancode);
                    result.type = NPL_EVENT_KEY_PRESSED;
                } break;
            case KeyRelease:
                {
                    result.key.scancode = scancode;
                    result.key.mods = translateX11KeyState(event.xkey.state);
                    result.key.code = translateX11Key(scancode);
                    result.type = NPL_EVENT_KEY_RELEASED;
                } break;
            default:
                {
                } break;
        }

    }

    for(int i = 0; i < NPL_MAXIMUM_WINDOWS; ++i) {
        NplWindow *window = &PLATFORM.windows[i];
        if(window->initialized) window->event.callback(window, result, window->event.userData);
    }
}
#elif defined(NPL_PLATFORM_WAYLAND)
    #error "Wayland display is not implemented yet"
#else
    #error "Unsupported linux display module"
#endif
