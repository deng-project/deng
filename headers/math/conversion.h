/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: conversion.cpp - mathematical conversions class implementation
/// author: Karl-Mihkel Ott


#ifndef __CONVERSION_H
#define __CONVERSION_H


#ifdef __CONVERSION_CPP
    #include <string.h>
    #include <utility>
    #include <type_traits>
    #include <stdlib.h>

    #include <common/base_types.h>
    #include <data/assets.h>
    #include <math/vec2.h>
#endif


namespace dengMath {
    namespace conversion {
        /// Convert degrees into radians
        const deng_f64_t degToRad(deng_f64_t deg);

        
        /// Convert radians into degrees
        const deng_f64_t radToDeg(deng_f64_t rad);


        /// Convert from vector coordinate size into pixel size
        const deng_px_t vector2DSizeToPixelSize (
            deng_vec_t vec_size, 
            deng_ui32_t window_size
        );


        /// Convert from pixel size into vector coordinate size
        const deng_vec_t pixelSizeToVector2DSize (
            deng_px_t pixel_size,
            deng_ui32_t window_size
        );


        /// Find the absolute position of child coordinates
        /// vert must be a pointer to valid array with at least 4 members
        const vec2<deng_vec_t> findAbsPosition (
            das_ObjPosData *vert,
            vec2<deng_vec_t> child_pos
        );


        /// Find the relative position of absolute coordinates
        /// vert must be a pointer to valid array with at least 4 members
        const vec2<deng_vec_t> findRelPosition (
            das_ObjPosData *vert,
            vec2<deng_vec_t> abs_pos
        );


        /// Find the absolute size of the child element
        const deng_vec_t findAbsSize (
            deng_vec_t parent_size,
            deng_vec_t child_size
        );


        /// Find the relative size of abs_size from its parent_size
        const deng_vec_t findRelSize (
            deng_vec_t parent_size,
            deng_vec_t abs_size
        );


        /// NOTE: Differences between mouse coordinates and vector coordinates
        /// Mouse coordinates are used to describe pixel coordinates with starting origin from top-left of the surface
        /// Primarly mouse coordinates are used to describe mouse and ImGui window locations 
        /// Vector coordinates on the other hand are used to specify object coordinates in the viewspace
        /// NOTE: Keep in mind that vector coordinates have a starting origin from the center of the surface and y-axis up is 
        /// negative, while y-axis down is positive coordinate direction


        /// Convert mouse axis position to corresponding vector coordinate
        const deng_vec_t mouseCoordToVecCoord(const deng_px_t mouse_axis_pos, const deng_px_t axis_len);


        /// Convert vector axis coordinate to corresponding mouse coordinate
        const deng_vec_t vecCoordToMouseCoord(const deng_vec_t vec_axis_pos, const deng_px_t axis_len);
    }
}

#endif
