/*    ____         ________     __      _        ______  
 *   |  _ \_      |  ______|   |  \    | |     _/  ____\
 *   | | \_ \_    |  |         |   \   | |    /  _/   
 *   | |   \  |   |  |_____    | |\ \  | |   |  |   _____ 
 *   | |    | |   |  ______|   | | \ \ | |   |  |  |___  | 
 *   | |  _/ _|   |  |         | |  \ \| |    \  \    / /
 *   | |_/ _/     |  |_____    | |   \   |     \_ \__/ /
 *   |____/       |________|   |_|    \__|       \____/
 *               
 * __________________________________________________________
 * __________________________________________________________
 *                       Project DENG 
 *
 *
 * Copyright (C) 2020 - 2021
 * This Software is licensed under Apache License as described 
 * in the LICENSE.md file, which you should have recieved with 
 * this distribution.
 * 
 * You may reproduce and distribute copies of the
 * Work or Derivative Works thereof in any medium, with or without
 * modifications, and in Source or Object form, provided that You
 * meet the following conditions:
 *
 * (a) You must give any other recipients of the Work or
 *     Derivative Works a copy of this License; and
 *
 * (b) You must cause any modified files to carry prominent notices
 *     stating that You changed the files; and
 *
 * (c) You must retain, in the Source form of any Derivative Works
 *     that You distribute, all copyright, patent, trademark, and
 *     attribution notices from the Source form of the Work,
 *     excluding those notices that do not pertain to any part of
 *     the Derivative Works; and
 *
 * (d) If the Work includes a "NOTICE" text file as part of its
 *     distribution, then any Derivative Works that You distribute must
 *     include a readable copy of the attribution notices contained
 *     within such NOTICE file, excluding those notices that do not
 *     pertain to any part of the Derivative Works, in at least one
 *     of the following places: within a NOTICE text file distributed
 *     as part of the Derivative Works; within the Source form or
 *     documentation, if provided along with the Derivative Works; or,
 *     within a display generated by the Derivative Works, if and
 *     wherever such third-party notices normally appear. The contents
 *     of the NOTICE file are for informational purposes only and
 *     do not modify the License. You may add Your own attribution
 *     notices within Derivative Works that You distribute, alongside
 *     or as an addendum to the NOTICE text from the Work, provided
 *     that such additional attribution notices cannot be construed
 *     as modifying the License.
 *
 * You may add Your own copyright statement to Your modifications and
 * may provide additional or different license terms and conditions
 * for use, reproduction, or distribution of Your modifications, or
 * for any such Derivative Works as a whole, provided Your use,
 * reproduction, and distribution of the Work otherwise complies with
 * the conditions stated in this License.
 */ 


#define __DENG_SURFACE_C
#include <deng/surface/surface_window.h>

// Static X11 exclusive functions
static void __deng_XInitCursors(deng_SurfaceWindow *p_win);
static void __deng_XFreeCursors(deng_SurfaceWindow *p_win);
static void __deng_XHandleKeyEvents(); 
static void __deng_XHandleMouseEvents(); 
static void __deng_XSetCursor (
    deng_SurfaceWindow *p_win,
    deng_bool_t hide
);

static Atom __atom_kill;

static Cursor __hidden_cur;
static Cursor __default_cur;

deng_SurfaceWindow *deng_InitVKSurfaceWindow (
    deng_i32_t width, 
    deng_i32_t height, 
    char *title, 
    deng_SurfaceWindowMode window_mode
) {
    static deng_SurfaceWindow win;

    win.width = width;
    win.height = height;
    win.window_title = title;

    win.vc_data.is_enabled = 0;
    win.vc_data.orig_x = (deng_vec_t) width / 2;
    win.vc_data.orig_y = (deng_vec_t) height / 2;
    win.vc_data.x = 0;
    win.vc_data.y = 0;
 
    win.x11_handler.p_display = XOpenDisplay(NULL);
    win.x11_handler.screen = DefaultScreen(win.x11_handler.p_display);

    XSizeHints size_hints;

    /*
     * Set flags for creating a fixed window
     * however it is up to windows manager to decide if the size hint flag
     * is respected or not
     */
    if(window_mode == DENG_WINDOW_MODE_FIXED) {
        size_hints.flags |= (PMinSize | PMaxSize);
        size_hints.min_width = size_hints.max_width =  win.width;
        size_hints.min_height = size_hints.max_height = win.height;   
    }    

    win.x11_handler.window = XCreateSimpleWindow (
        win.x11_handler.p_display, 
        DefaultRootWindow(win.x11_handler.p_display), 
        0, 
        0, 
        width, 
        height, 
        __DENG_DEFAULT_WINDOW_BORDER, 
        WhitePixel (
            win.x11_handler.p_display, 
            win.x11_handler.screen
        ), 
        BlackPixel (
            win.x11_handler.p_display, 
            win.x11_handler.screen
        )
    );

    XSetStandardProperties (
        win.x11_handler.p_display, 
        win.x11_handler.window, 
        title, 
        title, 
        None, 
        NULL, 
        0, 
        NULL
    );

    XSetWMNormalHints (
        win.x11_handler.p_display, 
        win.x11_handler.window, 
        &size_hints
    );

    XSelectInput (
        win.x11_handler.p_display, 
        win.x11_handler.window, 
        EVENT_MASKS
    );

    win.x11_handler.gc = XCreateGC (
        win.x11_handler.p_display, 
        win.x11_handler.window, 
        0, 
        0
    );

    XSetBackground (
        win.x11_handler.p_display, 
        win.x11_handler.gc, 
        BlackPixel (
            win.x11_handler.p_display, 
            win.x11_handler.screen
        )
    );

    XSetForeground (
        win.x11_handler.p_display, 
        win.x11_handler.gc, 
        WhitePixel (
            win.x11_handler.p_display, 
            win.x11_handler.screen
        )
    );

    Bool supported;
    if
    (
        !XkbSetDetectableAutoRepeat (
            win.x11_handler.p_display, 
            True, 
            &supported
        )
    ) exit(EXIT_FAILURE);

    XClearWindow (
        win.x11_handler.p_display, 
        win.x11_handler.window
    );

    XMapRaised (
        win.x11_handler.p_display, 
        win.x11_handler.window
    );

    __is_running = true;
    win.mode = X11_WINDOW;

    __atom_kill = XInternAtom (
        win.x11_handler.p_display,
        "WM_DELETE_WINDOW",
        True
    );

    XSetWMProtocols (
        win.x11_handler.p_display, 
        win.x11_handler.window, 
        &__atom_kill, 
        True
    );
    
    __deng_XInitCursors(&win);
    return &win;
}


/*
 * Initialise all cursors to use with xlib
 */
static void __deng_XInitCursors(deng_SurfaceWindow *p_win) {
    __hidden_cur = None;

    // Load default cursor
    __default_cur = XcursorLibraryLoadCursor (
        p_win->x11_handler.p_display,
        __DENG_XLIB_DEFAULT_CURSOR
    );
}


/* 
 * Destroy all cursors used in DENG
 */
static void __deng_XFreeCursors(deng_SurfaceWindow *p_win) {
    XFreeCursor (
        p_win->x11_handler.p_display, 
        __default_cur
    );

    XFreeCursor (
        p_win->x11_handler.p_display,
        __hidden_cur
    );
}


/*
 * Unlike WIN32 api X11 doesn't have a callback system on events, which
 * means that key events must be checked manually on every frame update 
 */
static void __deng_XHandleKeyEvents(deng_SurfaceWindow *p_win) {
    deng_Key key;
    switch (p_win->x11_handler.event.type)
    {
    case KeyPress: {
        key = translateX11Key (
            XLookupKeysym (
                &p_win->x11_handler.event.xkey, 
                0
            )
        );

        __deng_RegisterKeyEvent (
            key, 
            DENG_MOUSE_BTN_UNKNOWN, 
            DENG_INPUT_TYPE_KB, 
            DENG_INPUT_EVENT_TYPE_ACTIVE 
        );

        break;
    }
        
    case KeyRelease: {
        if(XEventsQueued(p_win->x11_handler.p_display, QueuedAfterReading)) {
            key = translateX11Key (
                XLookupKeysym (
                    &p_win->x11_handler.event.xkey, 
                    0
                )
            );

            __deng_RegisterKeyEvent (
                key,
                DENG_MOUSE_BTN_UNKNOWN,
                DENG_INPUT_TYPE_KB,
                DENG_INPUT_EVENT_TYPE_RELEASED
            );
        }
        break;
    }

    default:
        break;
    }
} 


/*
 * Check for any mouse button events
 */
static void __deng_XHandleMouseEvents(deng_SurfaceWindow *p_win) {
    deng_MouseButton btn = DENG_MOUSE_BTN_UNKNOWN;
    switch (p_win->x11_handler.event.type)
    {
    case ButtonPress: {
        btn = translateX11Btn(p_win->x11_handler.event.xbutton.button);
        __deng_RegisterKeyEvent (
            DENG_KEY_UNKNOWN, 
            btn, 
            DENG_INPUT_TYPE_MOUSE, 
            DENG_INPUT_EVENT_TYPE_ACTIVE
        );
        
        break;
    }
    
    case ButtonRelease:
        btn = translateX11Btn(p_win->x11_handler.event.xbutton.button);
        
        __deng_RegisterKeyEvent (
            DENG_KEY_UNKNOWN,
            btn,
            DENG_INPUT_TYPE_MOUSE,
            DENG_INPUT_EVENT_TYPE_RELEASED
        );
        break;

    default:
        break;
    }
}


/*
 * Force mouse cursor to certain location on window
 */
void deng_SetMouseCoords (
    deng_SurfaceWindow *p_win, 
    deng_px_t x, 
    deng_px_t y
) {
    XWarpPointer (
        p_win->x11_handler.p_display, 
        None, 
        p_win->x11_handler.window, 
        0, 
        0, 
        0, 
        0, 
        x, 
        y
    );
}


/*
 * Synchronise mouse position in deng_SurfaceWindow
 */
void deng_GetMousePos (
    deng_SurfaceWindow *p_win, 
    deng_bool_t init_vc
) {
    // dummy variables for x11
    Window return_window;
    int win_x, win_y, x, y;
    unsigned int mask;
    XQueryPointer (
        p_win->x11_handler.p_display, 
        p_win->x11_handler.window, 
        &return_window, 
        &return_window, 
        &win_x, 
        &win_y, 
        &x, 
        &y, 
        &mask
    );

    if(p_win->vc_data.is_enabled && !init_vc) {
        deng_f64_t movement_x = (deng_f64_t) ((deng_px_t) x - p_win->vc_data.orig_x);
        deng_f64_t movement_y = (deng_f64_t) ((deng_px_t) y - p_win->vc_data.orig_y);

        if
        (
            x != p_win->vc_data.orig_x || 
            y != p_win->vc_data.orig_y
        ) {
            deng_SetMouseCoords (
                p_win, 
                p_win->vc_data.orig_x, 
                p_win->vc_data.orig_y
            );
        }

        // Check for overflow on x position
        if(p_win->vc_data.x + movement_x >= (deng_f64_t) __max_vc_x) {
            if(__x_overflow_act == DENG_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION) {
                p_win->vc_data.x = __min_vc_x;
            }
        }
        
        else if(p_win->vc_data.x + movement_x <= (deng_f64_t) __min_vc_x) {
            if(__x_overflow_act == DENG_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                p_win->vc_data.x = __max_vc_x;
        }

        else p_win->vc_data.x += movement_x;


        // Check for overflow on y position
        if(p_win->vc_data.y + movement_y >= (deng_f64_t) __max_vc_y) {
            if(__y_overflow_act == DENG_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                p_win->vc_data.y = __min_vc_y;
        }
        
        else if(p_win->vc_data.y + movement_y <= (deng_f64_t) __min_vc_y) {
            if(__y_overflow_act == DENG_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                p_win->vc_data.y = __max_vc_y;
        }

        else p_win->vc_data.y += movement_y;
    }

    else {
        p_win->mx = (deng_px_t) x;
        p_win->my = (deng_px_t) y;

        /*if(init_vc) {*/
            /*deng_SetMouseCoords (*/
                /*p_win, */
                /*p_win->vc_data.orig_x,*/
                /*p_win->vc_data.orig_y*/
            /*);*/
        /*}*/
    }
}


/*
 * Currently only mouse hidden and mouse shown are supported 
 */
static void __deng_XSetCursor (
    deng_SurfaceWindow *p_win,
    deng_bool_t hide
) {
    if(hide) {
        XDefineCursor (
            p_win->x11_handler.p_display,
            p_win->x11_handler.window,
            __hidden_cur
        );
    }
    else {
        XDefineCursor (
            p_win->x11_handler.p_display, 
            p_win->x11_handler.window, 
            __default_cur
        );
    }
}


/*
 * Switch mouse cursor behaviour within the DENG window 
 */
void deng_SetMouseCursorMode (
    deng_SurfaceWindow *p_win, 
    deng_MouseMode mouse_mode
) {
    switch(mouse_mode) 
    {
    case DENG_MOUSE_MODE_INVISIBLE:
        __deng_XSetCursor (
            p_win, 
            false
        );
        p_win->vc_data.is_enabled = false;
        deng_GetMousePos (
            p_win, 
            true
        );

        deng_SetMouseCoords (
            p_win,
            p_win->vc_data.orig_x, 
            p_win->vc_data.orig_y
        );

        p_win->vc_data.is_enabled = true;
        break;

    case DENG_MOUSE_MODE_CURSOR_VISIBLE: 
        __deng_XSetCursor (
            p_win, 
            true
        );
        p_win->vc_data.is_enabled = false;
        break;

    default:
        break;
    }
}


/*
 * Update window events and key arrays
 * This function is meant to be called with every loop iteration 
 */
void deng_UpdateWindow(deng_SurfaceWindow *p_win) {
    // Check for exit event
    if
    (
        XCheckTypedWindowEvent (
            p_win->x11_handler.p_display,
            p_win->x11_handler.window, 
            ClientMessage, 
            &p_win->x11_handler.event
        )
    ) {
        printf("Closing window");
        __is_running = false;
        return;
    }

    __deng_UnreleaseKeys();
    
    if
    (
        XCheckWindowEvent (
            p_win->x11_handler.p_display, 
            p_win->x11_handler.window, 
            KeyPressMask | KeyReleaseMask, 
            &p_win->x11_handler.event
        )
    ) __deng_XHandleKeyEvents(p_win);
    
    if
    (
        XCheckWindowEvent (
            p_win->x11_handler.p_display, 
            p_win->x11_handler.window, 
            ButtonPressMask | ButtonReleaseMask, 
            &p_win->x11_handler.event
        )
    ) __deng_XHandleMouseEvents(p_win);
}


/*
 * Destroy window instance and free all resources that were used
 */
void deng_DestroyWindow(deng_SurfaceWindow *p_win) {
    XFreeGC (
        p_win->x11_handler.p_display, 
        p_win->x11_handler.gc
    );
    XDestroyWindow (
        p_win->x11_handler.p_display, 
        p_win->x11_handler.window
    );

    __deng_XFreeCursors(p_win);
}


/*
 * Limit the largest and smallest virtual cursor position that can be achieved using 
 * virtual mouse positioning
 */
void deng_LimitVirtualPos (
    deng_px_t max_x,        
    deng_px_t min_x,        
    deng_px_t max_y,        
    deng_px_t min_y     
) {
    __max_vc_x = max_x;
    __min_vc_x = min_x;
    __max_vc_y = max_y;
    __min_vc_y = min_y;
}


/*
 * Set virtual mouse position overflow actions that specify what
 * should happen if virtual mouse position limit has been reached
 */
void deng_SetOverflowAction (
    deng_VCPOverflowAction x_overflow_act,
    deng_VCPOverflowAction y_overflow_act
) {
    __x_overflow_act = x_overflow_act;
    __y_overflow_act = y_overflow_act;
}


/*
 * Check if window is still running and no close events have happened
 */
deng_bool_t deng_IsRunning() {
    return __is_running;
}