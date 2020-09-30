#ifndef DENGUI_CORE_H
#define DENGUI_CORE_H

#define DENGUI_WINDOW_LAYER_MULTIPLIER static_cast<float>(0.001)
#define DENGUI_MINIMIZE_TRIANGLE_OFFSET static_cast<float>(0.01)

namespace dengUI {
    enum dengUINonFilledBorderOffsetMode {
        DENGUI_NON_FILLED_BORDER_OFFSET_MODE_IN = 0,
        DENGUI_NON_FILLED_BORDER_OFFSET_MODE_OUT = 1
    };

    enum dengUIWindowLayerSequenceId {
        DENGUI_WINDOW_LAYER_TYPE_MAIN_WINDOW = 1,
        DENGUI_WINDOW_LAYER_TYPE_BORDERS_AND_TITLEBAR = 2,
        DENGUI_WINDOW_LAYER_TYPE_BUTTON = 3
    };

    enum dengUIWindowObjectShape {
        DENGUI_WINDOW_OBJECT_SHAPE_TRIANGLE = 0,
        DENGUI_WINDOW_OBJECT_SHAPE_RECTANGLE = 1
    };

    enum dengUIPixelBoundariesMode {
        DENGUI_PIXEL_BOUNDARIES_MODE_MINIMUM = 0,
        DENGUI_PIXEL_BOUNDARIES_MODE_MAXIMUM = 1
    };

    enum dengUIWindowSizeMode {
        DENGUI_WINDOW_SIZE_MODE_MAXIMISED = 0,
        DENGUI_WINDOW_SIZE_MODE_MINIMIZED = 1
    };

    enum dengUIWindowObjectHandlePtrUsage {
        DENGUI_WINDOW_OBJECT_HANDLE_PTR_USAGE_CALL = 0,
        DENGUI_WINDOW_OBJECT_HANDLE_PTR_USAGE_SET_CALLER = 1
    };
}

#include "dengui_infos.h"
#include "dengui_pixel_collision.h"
#include "dengui_events.h"
#include "dengui_event_handles.h"
#include "dengui_sprites.h"
#include "dengui_window.h"

#endif