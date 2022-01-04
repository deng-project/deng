/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: conversion.cpp - mathematical conversions class implementation
/// author: Karl-Mihkel Ott


#define __CONVERSION_CPP
#include <math/conversion.h>


namespace dengMath {
    namespace conversion {

        /// Convert degrees into radians
        const deng_f64_t degToRad(deng_f64_t deg) {
            return PI * deg / 180;
        }


        /// Convert radians into degrees
        const deng_f64_t radToDeg(deng_f64_t rad) {
            return 180 * rad / PI;
        }


        /// Convert from vector coordinate size into pixel size
        const deng_px_t vector2DSizeToPixelSize (
            deng_vec_t vec_size, 
            deng_ui32_t window_size
        ) { return static_cast<deng_px_t>(vec_size * static_cast<deng_vec_t>(window_size) / 2.0f); }



        /// Convert from pixel size into vector coordinate size
        const deng_vec_t pixelSizeToVector2DSize (
            deng_px_t pixel_size, 
            deng_ui32_t window_size
        ) { return 2.0f * static_cast<deng_vec_t>(pixel_size) / static_cast<deng_vec_t>(window_size); }


        /// Find the absolute position of child coordinates
        /// vert must be a pointer to valid array with at least 4 members
        const vec2<deng_vec_t> findAbsPosition (
            das_ObjPosData *vert,
            vec2<deng_vec_t> child_pos
        ) {
            vec2<deng_vec_t> size;
            size.first = vert[1].vert_x - vert[0].vert_x;
            size.second = vert[3].vert_y - vert[0].vert_y;

            vec2<deng_vec_t> abs_coords = {
                vert[0].vert_x + (child_pos.first + 1.0f / 2 * size.first),
                vert[0].vert_y + (child_pos.second + 1.0f) / 2 * size.second
            };
           
            return abs_coords;
        }


        /// Find the relative position of absolute coordinates
        /// NOTE: vert must be a pointer to valid array with at least 4 members
        const vec2<deng_vec_t> findRelPosition (
            das_ObjPosData *vert,
            vec2<deng_vec_t> abs_pos
        ) {
            vec2<deng_vec_t> size;
            size.first = vert[1].vert_x - vert[0].vert_x;
            size.second = vert[3].vert_y - vert[0].vert_y;

            vec2<deng_vec_t> rel_pos = {
                2 * (abs_pos.first + vert[0].vert_x) / size.first - 1.0f,
                2 * (abs_pos.second + vert[0].vert_y) / size.second - 1.0f
            }; 

            return rel_pos; 
        }


        /// Find the absolute size of the child element
        const deng_vec_t findAbsSize (
            deng_vec_t parent_size,
            deng_vec_t child_size
        ) { return parent_size / 2 * child_size; }



        /// Find the relative size of abs_size from its parent_size
        const deng_vec_t findRelSize (
            deng_vec_t parent_size,
            deng_vec_t abs_size
        ) { return abs_size / parent_size * 2; }


        /// Convert mouse axis position to corresponding vector coordinate
        const deng_vec_t mouseCoordToVecCoord(const deng_px_t mouse_axis_pos, const deng_px_t axis_len) {
            return ((2.0f * static_cast<deng_vec_t>(mouse_axis_pos) - static_cast<deng_vec_t>(axis_len)) / axis_len);
        }


        /// Convert vector axis coordinate to corresponding mouse coordinate
        const deng_vec_t vecCoordToMouseCoord(const deng_vec_t vec_axis_pos, const deng_px_t axis_len) {
            return (vec_axis_pos + 1.0f) * axis_len / 2;
        }
    }
}
