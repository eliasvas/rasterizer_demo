#ifndef PLATFORM_H
#define PLATFORM_H

#include <windows.h>
#include "tools.h"

extern char error_log[512]; 

enum keyboard_keys
{
    KEY_A = 1, // NOTE: Starts at 1 so that 0 can represent no input
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,

    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,

    KEY_NUMPAD_0,
    KEY_NUMPAD_1,
    KEY_NUMPAD_2,
    KEY_NUMPAD_3,
    KEY_NUMPAD_4,
    KEY_NUMPAD_5,
    KEY_NUMPAD_6,
    KEY_NUMPAD_7,
    KEY_NUMPAD_8,
    KEY_NUMPAD_9,
    KEY_NUMPAD_MULTIPLY,
    KEY_NUMPAD_ADD,
    KEY_NUMPAD_SUBTRACT,
    KEY_NUMPAD_DECIMAL,
    KEY_NUMPAD_DIVIDE,

    KEY_LEFT,
    KEY_UP,
    KEY_RIGHT,
    KEY_DOWN,

    KEY_BACKSPACE,
    KEY_TAB,
    KEY_CTRL,
    KEY_RETURN,
    KEY_SPACE,
    KEY_LSHIFT,
    KEY_RSHIFT,
    KEY_LCONTROL,
    KEY_RCONTROL,
    KEY_ALT,
    KEY_LSUPER,
    KEY_RSUPER,
    KEY_CAPSLOCK,
    KEY_ESCAPE,
    KEY_PAGEUP,
    KEY_PAGEDOWN,
    KEY_HOME,
    KEY_END,
    KEY_INSERT,
    KEY_DELETE,
    KEY_PAUSE,
    KEY_NUMLOCK,
    KEY_PRINTSCREEN,

    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,


    KEY_MAX
};


typedef struct Platform
{
    i32 window_width; //width of window (width border)
    i32 window_height; //height of window (with border)
    b32 window_resized;
    b32 exit; //whether we should exit
    b32 vsync; //whether the app has vsync (capped FPS.. kinda)
    b32 fullscreen; //whether the app is in fullscreen
    b32 initialized; //whether the app is initialized
    f32 target_fps; //in how many frames should the app run
    f32 current_time; //current time in milliseconds

    f32 mouse_x; //mouse x pos wrt top left corner
    f32 mouse_y; //mouse y coordinate wrt top left corner
    vec2 mouse_dt;
    b8 left_mouse_down; //whether LMB is down 
    b8 right_mouse_down; //whether RMB is down
    b8 key_down[KEY_MAX]; //whether a key is down
    b8 key_pressed[KEY_MAX]; //whether a key was JUST pressed
    i32 last_key; //the last key pressed
    b32 keyboard_used; //whether keyboard is used..
    f32 dt; //time between frames in milliseconds


    Arena permanent_storage; //an arena to store permanent data, never flushed
    Arena frame_storage; //an arena to store frame data, flushed EVERY frame
}Platform;

#endif
