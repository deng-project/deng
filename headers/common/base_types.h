/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: base_types.h - DENG base type definitions
/// author: Karl-Mihkel Ott

#ifndef BASE_TYPES_H
#define BASE_TYPES_H
#include <stdint.h>

#define PI 3.14159

/* Main deng integer types */
typedef uint64_t deng_ui64_t;
typedef int64_t deng_i64_t;

typedef uint32_t deng_ui32_t;
typedef int32_t deng_i32_t;

typedef uint16_t deng_ui16_t;
typedef int16_t deng_i16_t;

typedef uint8_t deng_ui8_t;
typedef int8_t deng_i8_t;

typedef float deng_f32_t;
typedef double deng_f64_t;

typedef uint8_t deng_bool_t;

/* Size types */
typedef int64_t deng_px_t;
typedef float deng_vec_t;

typedef deng_ui32_t deng_idx_t;
typedef deng_i32_t deng_gl_t;
typedef deng_ui32_t deng_ugl_t;


// MSVC C++ compiler does not allow redefining true and false keywords C4005
// GCC does allow it
#ifndef __cplusplus
    #define true 1
    #define false 0
#endif


/// Universal DENG identifier type
typedef char* deng_Id;


/// Registry type bitmasks
typedef enum deng_RegistryType {
    DENG_REGISTRY_TYPE_ASSET       = 0x0001,
    DENG_REGISTRY_TYPE_VK_ASSET    = 0x0002,
    DENG_REGISTRY_TYPE_TEXTURE     = 0x0008,
    DENG_REGISTRY_TYPE_VK_TEXTURE  = 0x0010,
    DENG_REGISTRY_TYPE_GL_TEXTURE  = 0x0020,
    DENG_REGISTRY_TYPE_PT_LIGHT    = 0x0040,
    DENG_REGISTRY_TYPE_SUN_LIGHT   = 0x0080,
    DENG_REGISTRY_TYPE_DIR_LIGHT   = 0x0100,

    DENG_REGISTRY_TYPE_FIRST       = DENG_REGISTRY_TYPE_ASSET,
    DENG_REGISTRY_TYPE_LAST        = DENG_REGISTRY_TYPE_DIR_LIGHT
} deng_RegistryTypes;
typedef deng_ui32_t deng_RegistryTypeBitMask;


/// Uniform flags mainly reserved for future use
typedef deng_ui32_t deng_CameraUniformFlagBits;
#define DENG_CAMERA_UNIFORM_ORTHOGRAPHIC_CAMERA_MODE_3D 0x01u
#define DENG_CAMERA_UNIFORM_PERSPECTIVE_CAMERA_MODE_3D  0x02u
#define DENG_CAMERA_UNIFORM_ORTHOGRAPHIC_CAMERA_MODE_2D 0x04u
#define DENG_CAMERA_UNIFORM_NO_CAMERA_MODE_2D           0x08u
#define DENG_CAMERA_UNIFORM_NO_CAMERA_MODE_3D           0x10u


/// Hints for controlling renderer features 
typedef deng_ui16_t deng_RendererHintBits;
#define DENG_RENDERER_HINT_ENABLE_VSYNC                     0x0001u
#define DENG_RENDERER_HINT_SHOW_CMD_FPS_COUNTER             0x0002u
#define DENG_RENDERER_HINT_SHOW_GUI_FPS_COUNTER             0x0004u
#define DENG_RENDERER_HINT_ENABLE_API_DEBUGGING             0x0008u
#define DENG_RENDERER_HINT_MSAA_MAX_HARDWARE_SUPPORTED      0x0010u
#define DENG_RENDERER_HINT_MSAA_64                          0x0020u
#define DENG_RENDERER_HINT_MSAA_32                          0x0040u
#define DENG_RENDERER_HINT_MSAA_16                          0x0080u
#define DENG_RENDERER_HINT_MSAA_8                           0x0100u
#define DENG_RENDERER_HINT_MSAA_4                           0x0200u
#define DENG_RENDERER_HINT_MSAA_2                           0x0400u
#define DENG_RENDERER_HINT_MSAA_1                           0x0800u
#define DENG_RENDERER_HINT_MIPMAP_ENABLE                    0x1000u
#define DENG_RENDERER_HINT_CUSTOM_LOOP                      0x2000u


/// Hints for controlling window creation
#define deng_WindowHint neko_Hint
#define DENG_WINDOW_HINT_API_OPENGL         NEKO_HINT_API_OPENGL
#define DENG_WINDOW_HINT_API_VULKAN         NEKO_HINT_API_VULKAN
#define DENG_WINDOW_HINT_FULL_SCREEN        NEKO_HINT_FULL_SCREEN
#define DENG_WINDOW_HINT_FIXED_SIZE         NEKO_HINT_FIXED_SIZE
#define DENG_WINDOW_HINT_RESIZEABLE         NEKO_HINT_RESIZEABLE


/// Definitions for mouse cursors
#define deng_CursorMode                 neko_CursorMode
#define DENG_CURSOR_MODE_STANDARD       NEKO_CURSOR_MODE_STANDARD
#define DENG_CURSOR_MODE_HIDDEN         NEKO_CURSOR_MODE_HIDDEN
#define DENG_CURSOR_MODE_POINTER        NEKO_CURSOR_MODE_POINTER
#define DENG_CURSOR_MODE_WAITING        NEKO_CURSOR_MODE_WAITING


/// Coordinate axis specifier 
typedef enum deng_CoordinateAxisType {
    DENG_COORD_AXIS_UNDEFINED   = -1,
    DENG_COORD_AXIS_X           = 0,
    DENG_COORD_AXIS_Y           = 1,
    DENG_COORD_AXIS_Z           = 2,
    DENG_COORD_AXIS_FIRST       = DENG_COORD_AXIS_UNDEFINED,
    DENG_COORD_AXIS_LAST        = DENG_COORD_AXIS_Z
} deng_CoordinateAxisType;


/// Movement mode specifier 
typedef enum deng_MovementEvent {
    DENG_MOVEMENT_NONE          = -1,
    DENG_MOVEMENT_FORWARD       = 0,
    DENG_MOVEMENT_BACKWARD      = 1,
    DENG_MOVEMENT_RIGHTWARD     = 2,
    DENG_MOVEMENT_LEFTWARD      = 3,
    DENG_MOVEMENT_UPWARD        = 4,
    DENG_MOVEMENT_DOWNWARD      = 5,
    DENG_MOVEMENT_FIRST         = DENG_MOVEMENT_NONE,
    DENG_MOVEMENT_LAST          = DENG_MOVEMENT_DOWNWARD
} deng_MovementEvent;


/// Pipeline type specifier 
typedef enum deng_PipelineType {
    DENG_PIPELINE_TYPE_UNKNOWN                  = -1,
    DENG_PIPELINE_TYPE_UNMAPPED_3D              =  0,
    DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D        =  1,
    DENG_PIPELINE_TYPE_UNMAPPED_2D              =  2,
    DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D        =  3,
    DENG_PIPELINE_TYPE_UI                       =  4,
    DENG_PIPELINE_TYPE_FIRST                    =  DENG_PIPELINE_TYPE_UNKNOWN,
    DENG_PIPELINE_TYPE_LAST                     =  DENG_PIPELINE_TYPE_UI
} deng_PipelineType;


/// Renderer usage mode 
typedef enum deng_RendererUsageMode {
    DENG_RENDERER_USAGE_ASSET_EDITOR    = 0,
    DENG_RENDERER_USAGE_MAP_EDITOR      = 1,
    DENG_RENDERER_USAGE_GAME_MODE       = 2,
    DENG_RENDERER_USAGE_FIRST           = DENG_RENDERER_USAGE_ASSET_EDITOR,
    DENG_RENDERER_USAGE_LAST            = DENG_RENDERER_USAGE_GAME_MODE
} deng_RendererUsageMode;


/// Camera type specifier 
typedef enum deng_CameraType {
    DENG_CAMERA_TYPE_FPP        = 0,
    DENG_CAMERA_TYPE_EDITOR     = 1,
    DENG_CAMERA_TYPE_FIRST      = 0,
    DENG_CAMERA_TYPE_LAST       = 1,
} deng_CameraType;


/// Editor camera event specifier 
typedef enum deng_EditorCameraEvent {
    DENG_EDITOR_CAMERA_EVENT_NONE               = -1,
    DENG_EDITOR_CAMERA_EVENT_Z_MOV_IN           = 0,
    DENG_EDITOR_CAMERA_EVENT_Z_MOV_OUT          = 1,
    DENG_EDITOR_CAMERA_EVENT_MOUSE_ROTATE       = 2,
    DENG_EDITOR_CAMERA_EVENT_FIRST              = -1,
    DENG_EDITOR_CAMERA_EVENT_LAST               = 2
} deng_EditorCameraEvent;


/// Specify input device type for action lookup
typedef enum deng_InputType {
    DENG_INPUT_TYPE_MOUSE   = 0,
    DENG_INPUT_TYPE_KB      = 1,
    DENG_INPUT_TYPE_FIRST   = 0,
    DENG_INPUT_TYPE_LAST    = 1

} deng_InputType;


/// Light source type specifier enum
typedef enum deng_LightSrcType {
    DENG_LIGHT_SRC_TYPE_PT      = 0,
    DENG_LIGHT_SRC_TYPE_SUN     = 1,
    DENG_LIGHT_SRC_TYPE_DIR     = 2,

    DENG_LIGHT_SRC_TYPE_FIRST   = DENG_LIGHT_SRC_TYPE_PT,
    DENG_LIGHT_SRC_TYPE_LAST    = DENG_LIGHT_SRC_TYPE_DIR
} deng_LightSrcType;


/// Specify input event type for lookup
typedef enum deng_InputEventType {
    DENG_INPUT_EVENT_TYPE_ACTIVE    = 0,
    DENG_INPUT_EVENT_TYPE_RELEASED  = 1,
    DENG_INPUT_EVENT_TYPE_FIRST     = 0,
    DENG_INPUT_EVENT_TYPE_LAST      = 1
} deng_InputEventType;


/// Specify what should happen if virtual cursor achieves 
/// its maximum or minimum position 
typedef enum deng_VCPOverflowAction {
    DENG_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION   = 0,
    DENG_VCP_OVERFLOW_ACTION_BLOCK_POSITION         = 1,
    DENG_VCP_OVERFLOW_ACTION_NONE                   = 2,
    DENG_VCP_OVERFLOW_FIRST                         = DENG_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION,
    DENG_VCP_OVERFLOW_LAST                          = DENG_VCP_OVERFLOW_ACTION_BLOCK_POSITION
} deng_VCPOverflowAction;


/// Camera action specifier enum
typedef enum deng_CameraAction {
    DENG_CAMERA_ACTION_MOV_U        = 0,
    DENG_CAMERA_ACTION_MOV_NU       = 1,
    DENG_CAMERA_ACTION_MOV_V        = 2,
    DENG_CAMERA_ACTION_MOV_NV       = 3,
    DENG_CAMERA_ACTION_MOV_W        = 4,
    DENG_CAMERA_ACTION_MOV_NW       = 5,

    DENG_CAMERA_ACTION_MOV_X        = 6,
    DENG_CAMERA_ACTION_MOV_NX       = 7,
    DENG_CAMERA_ACTION_MOV_Y        = 8,
    DENG_CAMERA_ACTION_MOV_NY       = 9,
    DENG_CAMERA_ACTION_MOV_Z        = 10,
    DENG_CAMERA_ACTION_MOV_NZ       = 11,

    DENG_CAMERA_ACTION_ROT_U        = 12,
    DENG_CAMERA_ACTION_ROT_NU       = 13,
    DENG_CAMERA_ACTION_ROT_V        = 14,
    DENG_CAMERA_ACTION_ROT_NV       = 15,
    DENG_CAMERA_ACTION_ROT_W        = 16,
    DENG_CAMERA_ACTION_ROT_NW       = 17,

    DENG_CAMERA_ACTION_ROT_X        = 18,
    DENG_CAMERA_ACTION_ROT_NX       = 19,
    DENG_CAMERA_ACTION_ROT_Y        = 20,
    DENG_CAMERA_ACTION_ROT_NY       = 21,
    DENG_CAMERA_ACTION_ROT_Z        = 22,
    DENG_CAMERA_ACTION_ROT_NZ       = 23,

    // Change first person camera mouse input mode
    DENG_CAMERA_ACTION_CHANGE_MM    = 24,

    DENG_CAMERA_ACTION_FIRST        = DENG_CAMERA_ACTION_MOV_U,
    DENG_CAMERA_ACTION_LAST         = DENG_CAMERA_ACTION_CHANGE_MM,
} deng_CameraAction;


#define DENG_KEY_PRESS_INTERVAL         500 // ms
#define DENG_MOVEMENT_INTERVAL          17 // ms

#define DENG_CAMERA_BASE_SPEED_X        0.015
#define DENG_CAMERA_BASE_SPEED_Y        0.015
#define DENG_CAMERA_BASE_SPEED_Z        -0.015

#define DENG_FPP_CAMERA_DEFAULT_POS_X   0.0f
#define DENG_FPP_CAMERA_DEFAULT_POS_Y   0.5f
#define DENG_FPP_CAMERA_DEFAULT_POS_Z   0.5f

#define DENG_EDITOR_CAMERA_DEFAULT_POS_X    0.0f
#define DENG_EDITOR_CAMERA_DEFAULT_POS_Y    0.0f
#define DENG_EDITOR_CAMERA_DEFAULT_POS_Z    3.0f
#define DENG_EDITOR_CAMERA_DEFAULT_X_ROT    dengMath::Conversion::degToRad(-30.0f)
#define DENG_EDITOR_CAMERA_DEFAULT_Y_ROT    dengMath::Conversion::degToRad(30.0f)
#endif
