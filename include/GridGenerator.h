// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GridGenerator.h - Grid generator class header
// author: Karl-Mihkel Ott


#ifndef GRID_GENERATOR_H
#define GRID_GENERATOR_H

#ifdef GRID_GENERATOR_CPP
    #include <stdlib.h>
    #include <vector>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <common/uuid.h>
    #include <data/assets.h>
    #include <data/das_runtime.h>
    #include <math/deng_math.h>
#endif

#define DENG_DEFAULT_GRID_COLOR             dengMath::vec4<deng_vec_t>(0.8f, 0.8f, 0.8f, 1.0f)
#define DENG_DEFAULT_GRID_RAY_LEN           50.0f    
#define DENG_DEFAULT_GRID_ROW_C             100
#define DENG_GRID_THICKNESS                 0.005f

namespace DENG {
    
    /// This class is used to create new grid for editors
    class GridGenerator {
    private:
        const das_ObjNormalData m_vert_normal = das_ObjNormalData{0.0f, -1.0f, 0.0f};
        dengMath::vec4<deng_vec_t> m_color = DENG_DEFAULT_GRID_COLOR;
        deng_vec_t m_grid_len = DENG_DEFAULT_GRID_RAY_LEN;
        deng_ui32_t m_row_c = DENG_DEFAULT_GRID_ROW_C;
        das_Asset m_asset = {};

    public:
        Grid(const dengMath::vec4<deng_vec_t> &color, deng_vec_t len, deng_ui32_t row_c);

        /// Calculate vertices and indices for the grid 
        void calc();

    /// Setters and getters
    public:
        das_Asset &getGridAsAnAsset();
        void setColor(das_ObjColorData &color);
        void setRowCountPerLine(deng_ui32_t row_c);
        void setLineLength(deng_vec_t ray_len);
    };
}

#endif
