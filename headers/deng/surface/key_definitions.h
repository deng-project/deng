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


#ifndef DENG_KEY_DEFINITIONS_H
#define DENG_KEY_DEFINITIONS_H

#define DENG_INPUT_EV_COUNT 126

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __KEY_TRANSLATION_C
    #include <common/base_types.h>
#endif

#ifdef __linux__
    #include <X11/XKBlib.h>
    #include <X11/keysym.h>
#endif

#ifdef _WIN32 
    #include <windows.h>
#endif

/*
 * Mouse mode specifier enum
 */
typedef enum deng_MouseMode {
    DENG_MOUSE_MODE_UNKNOWN         = -1,
    DENG_MOUSE_MODE_INVISIBLE       = 0,
    DENG_MOUSE_MODE_CURSOR_VISIBLE  = 1,
    DENG_MOUSE_MODE_FIRST           = -1,
    DENG_MOUSE_MODE_LAST            = 1
} deng_MouseMode;


/*
 * Keyboard key event specifier enum
 */
typedef enum deng_Key {
    DENG_KEY_UNKNOWN        =   0,
    DENG_KEY_SPACE          =   1,
    DENG_KEY_APOSTROPHE     =   2,
    DENG_KEY_COMMA          =   3,
    DENG_KEY_MINUS          =   4,
    DENG_KEY_DOT            =   5,
    DENG_KEY_SLASH          =   6,
    DENG_KEY_0              =   7,
    DENG_KEY_1              =   8,
    DENG_KEY_2              =   9,
    DENG_KEY_3              =   10,
    DENG_KEY_4              =   11,
    DENG_KEY_5              =   12,
    DENG_KEY_6              =   13,
    DENG_KEY_7              =   14,
    DENG_KEY_8              =   15,
    DENG_KEY_9              =   16,
    DENG_KEY_SEMICOLON      =   17,
    DENG_KEY_EQUAL          =   18,
    DENG_KEY_A              =   19,
    DENG_KEY_B              =   20,
    DENG_KEY_C              =   21,
    DENG_KEY_D              =   22,
    DENG_KEY_E              =   23,
    DENG_KEY_F              =   24,
    DENG_KEY_G              =   25,
    DENG_KEY_H              =   26,
    DENG_KEY_I              =   27,
    DENG_KEY_J              =   28,
    DENG_KEY_K              =   29,
    DENG_KEY_L              =   30,
    DENG_KEY_M              =   31,
    DENG_KEY_N              =   32,
    DENG_KEY_O              =   33,
    DENG_KEY_P              =   34,
    DENG_KEY_Q              =   35,
    DENG_KEY_R              =   36,
    DENG_KEY_S              =   37,
    DENG_KEY_T              =   38,
    DENG_KEY_U              =   39,
    DENG_KEY_V              =   40,
    DENG_KEY_W              =   41,
    DENG_KEY_X              =   42,
    DENG_KEY_Y              =   43,
    DENG_KEY_Z              =   44,
    DENG_KEY_ESCAPE         =   45,
    DENG_KEY_ENTER          =   46,
    DENG_KEY_TAB            =   47,
    DENG_KEY_BACKSPACE      =   48,
    DENG_KEY_INSERT         =   49,
    DENG_KEY_DELETE         =   50,
    DENG_KEY_RIGHT          =   51,
    DENG_KEY_LEFT           =   52,
    DENG_KEY_DOWN           =   53,
    DENG_KEY_UP             =   54,
    DENG_KEY_PAGE_UP        =   55,
    DENG_KEY_PAGE_DOWN      =   56,
    DENG_KEY_HOME           =   57,
    DENG_KEY_END            =   58,
    DENG_KEY_CAPS_LOCK      =   59,
    DENG_KEY_SCROLL_LOCK    =   60,
    DENG_KEY_NUM_LOCK       =   61,
    DENG_KEY_PRINT_SCREEN   =   62,
    DENG_KEY_PAUSE          =   63,
    DENG_KEY_F1             =   64,
    DENG_KEY_F2             =   65,
    DENG_KEY_F3             =   66,
    DENG_KEY_F4             =   67,
    DENG_KEY_F5             =   68,
    DENG_KEY_F6             =   69,
    DENG_KEY_F7             =   70,
    DENG_KEY_F8             =   71,
    DENG_KEY_F9             =   72,
    DENG_KEY_F10            =   73,
    DENG_KEY_F11            =   74,
    DENG_KEY_F12            =   75,
    DENG_KEY_F13            =   76,
    DENG_KEY_F14            =   77,
    DENG_KEY_F15            =   78,
    DENG_KEY_F16            =   79,
    DENG_KEY_F17            =   80,
    DENG_KEY_F18            =   81,
    DENG_KEY_F19            =   82,
    DENG_KEY_F20            =   83,
    DENG_KEY_F21            =   84,
    DENG_KEY_F22            =   85,
    DENG_KEY_F23            =   86,
    DENG_KEY_F24            =   87,
    DENG_KEY_F25            =   88,
    DENG_KEY_NP_0           =   89,
    DENG_KEY_NP_1           =   90,
    DENG_KEY_NP_2           =   91,
    DENG_KEY_NP_3           =   92,
    DENG_KEY_NP_4           =   93,
    DENG_KEY_NP_5           =   94,
    DENG_KEY_NP_6           =   95,
    DENG_KEY_NP_7           =   96,
    DENG_KEY_NP_8           =   97,
    DENG_KEY_NP_9           =   98,
    DENG_KEY_NP_DECIMAL     =   99,
    DENG_KEY_NP_DIVIDE      =   100,
    DENG_KEY_NP_MULTIPLY    =   101,
    DENG_KEY_NP_SUBTRACT    =   102,
    DENG_KEY_NP_ADD         =   103,
    DENG_KEY_NP_ENTER       =   104,
    DENG_KEY_NP_EQUAL       =   105,
    DENG_KEY_LEFT_SHIFT     =   106,
    DENG_KEY_LEFT_CTRL      =   107,
    DENG_KEY_LEFT_ALT       =   108,
    DENG_KEY_LEFT_SUPER     =   109,
    DENG_KEY_RIGHT_SHIFT    =   110,
    DENG_KEY_RIGHT_CTRL     =   111,
    DENG_KEY_RIGHT_ALT      =   112,
    DENG_KEY_RIGHT_SUPER    =   113,
    DENG_KEY_MENU           =   114,
    DENG_KEY_FIRST          =   DENG_KEY_UNKNOWN,
    DENG_KEY_LAST           =   DENG_KEY_MENU
} deng_Key;


/*
 * Mouse button event specifier enum
 */
typedef enum deng_MouseButton {
    DENG_MOUSE_BTN_UNKNOWN  = 115,
    DENG_MOUSE_BTN_1        = 116,
    DENG_MOUSE_BTN_2        = 117,
    DENG_MOUSE_BTN_3        = 118,
    DENG_MOUSE_SCROLL_DOWN  = 119,
    DENG_MOUSE_SCROLL_UP    = 120,
    DENG_MOUSE_BTN_FIRST    = DENG_MOUSE_BTN_UNKNOWN,
    DENG_MOUSE_BTN_LAST     = DENG_MOUSE_SCROLL_UP
} deng_MouseButton;


/*
 * These values are used in case delta mouse movement is detected
 */
typedef enum deng_MouseDeltaMovement {
    DENG_MOUSE_DELTA_UNKNOWN    = 121,
    DENG_MOUSE_DELTA_X          = 122,
    DENG_MOUSE_DELTA_NX         = 123,
    DENG_MOUSE_DELTA_Y          = 124,
    DENG_MOUSE_DELTA_NY         = 125,
    DENG_MOUSE_DELTA_FIRST      = DENG_MOUSE_DELTA_UNKNOWN,
    DENG_MOUSE_DELTA_LAST       = DENG_MOUSE_DELTA_NY
} deng_MouseDeltaMovement;


/*
 * Universal union for specifying input information
 */
typedef union {
    deng_Key key;
    deng_MouseButton btn;
    deng_MouseDeltaMovement md_mov;
} deng_InputEv;


/*
 * Universal input event type for general purpose input devices
 */
typedef deng_ui64_t deng_InputBits;
#define MAX_KEY_COMBO   8


#ifdef __linux__
    deng_Key translateX11Key(KeySym keysym);
    deng_MouseButton translateX11Btn(unsigned int button);
#endif

#ifdef WIN32
    deng_Key translateWIN32Key(deng_ui16_t key_code);
    deng_MouseButton translateWIN32Btn(UINT msg);
#endif

#ifdef __cplusplus
}
#endif

#endif
