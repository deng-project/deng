/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: grid_generator.cpp - grid generation class implementation
/// author: Karl-Mihkel Ott


#define __GRID_GENERATOR_CPP
#include <utils/grid_generator.h>

namespace dengUtils {

    Grid::Grid(const dengMath::vec4<deng_vec_t> &color, deng_vec_t len, deng_ui32_t row_c) {
        m_color = color;
        m_grid_len = len;
        m_row_c = row_c;
        
        const deng_vec_t step = len / (deng_vec_t) row_c;

        // Set the asset mode and its uuid
        m_asset.asset_mode = DAS_ASSET_MODE_3D_UNMAPPED;
        m_asset.uuid = uuid_Generate();

        // Allocate memory for asset vertices
        m_asset.vertices.v3d.mul.pn = row_c * 6;
        m_asset.vertices.v3d.mul.pos = (das_ObjPosData*) calloc (
            m_asset.vertices.v3d.mul.pn, sizeof(das_ObjPosData));

        m_asset.vertices.v3d.mul.nn = 1;
        m_asset.vertices.v3d.mul.norm = (das_ObjNormalData*) &m_vert_normal;

        // Allocate memory for indices
        m_asset.indices.n = m_asset.vertices.v3d.mul.pn;
        m_asset.indices.pos = (deng_ui32_t*) calloc(m_asset.indices.n, sizeof(deng_ui32_t));
        m_asset.indices.norm = (deng_ui32_t*) calloc(m_asset.indices.n, sizeof(deng_ui32_t));

        // Set the temporary color data instance
        das_ObjColorData tmp_col;
        tmp_col.col_r = m_color.first;
        tmp_col.col_g = m_color.second;
        tmp_col.col_b = m_color.third;
        tmp_col.col_a = m_color.fourth;

        size_t i, j;

        // Z axis rays
        deng_vec_t cur_pos = -(len / 2);

        // For each line in Z-axis find the vertices and indices
        for(i = 0; i < row_c * 3; i += 3) {
            m_asset.vertices.v3d.mul.pos[i].vert_x = -(len / 2);
            m_asset.vertices.v3d.mul.pos[i].vert_y = 0;
            m_asset.vertices.v3d.mul.pos[i].vert_z = cur_pos;
            
            m_asset.vertices.v3d.mul.pos[i + 1].vert_x = len / 2;
            m_asset.vertices.v3d.mul.pos[i + 1].vert_y = 0;
            m_asset.vertices.v3d.mul.pos[i + 1].vert_z = cur_pos;

            m_asset.vertices.v3d.mul.pos[i + 2].vert_x = -(len / 2);
            m_asset.vertices.v3d.mul.pos[i + 2].vert_y = 0;
            m_asset.vertices.v3d.mul.pos[i + 2].vert_z = cur_pos + DENG_GRID_THICKNESS;
            
            m_asset.indices.pos[i] = (deng_ui32_t) i;
            m_asset.indices.pos[i + 1] = (deng_ui32_t) i + 1;
            m_asset.indices.pos[i + 2] = (deng_ui32_t) i + 2;
            cur_pos += step;
        }

        // X axis rays
        j = i;
        cur_pos = -(len / 2);

        // For each line in X-axis find the vertices and indices
        for(i = 0; i < row_c * 3; i += 3, j += 3) {
            m_asset.vertices.v3d.mul.pos[j].vert_x = cur_pos;
            m_asset.vertices.v3d.mul.pos[j].vert_y = 0;
            m_asset.vertices.v3d.mul.pos[j].vert_z = -(len / 2);

            m_asset.vertices.v3d.mul.pos[j + 1].vert_x = cur_pos;
            m_asset.vertices.v3d.mul.pos[j + 1].vert_y = 0;
            m_asset.vertices.v3d.mul.pos[j + 1].vert_z = len / 2;

            m_asset.vertices.v3d.mul.pos[j + 2].vert_x = cur_pos + DENG_GRID_THICKNESS;
            m_asset.vertices.v3d.mul.pos[j + 2].vert_y = 0;
            m_asset.vertices.v3d.mul.pos[j + 2].vert_z = -(len / 2);

            m_asset.indices.pos[j] = (deng_ui32_t) j;
            m_asset.indices.pos[j + 1 ] = (deng_ui32_t) j + 1;
            m_asset.indices.pos[j + 2] = (deng_ui32_t) j + 2;
            cur_pos += step;
        }

        // Generate vertex normals
        das_MkAssetNormals(&m_asset);
    }

    das_Asset &Grid::getGridAsAnAsset() { return m_asset; }
}
