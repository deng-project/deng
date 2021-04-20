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


#define __GRID_GENERATOR_CPP
#include <utils/grid_generator.h>

namespace dengUtils {
    GridGenerator::GridGenerator (
        dengMath::vec4<deng_vec_t> color,
        deng_vec_t len,
        deng_ui32_t row_c
    ) {
        m_color = color;
        m_grid_len = len;
        m_row_c = row_c;
        
        deng_vec_t step = len / (deng_vec_t) row_c;

        // Setup asset
        m_asset.asset_mode = DAS_ASSET_MODE_3D_UNMAPPED;
        m_asset.uuid = uuid_Generate();
        m_asset.vertices.n = row_c * 6;
        m_asset.vertices.vuu = (VERT_UNMAPPED_UNOR*) calloc (
            m_asset.vertices.n,
            sizeof(VERT_UNMAPPED_UNOR)
        );

        m_asset.indices.n = m_asset.vertices.n;
        m_asset.indices.indices = (deng_ui32_t*) calloc (
            m_asset.indices.n ,
            sizeof(deng_ui32_t)
        );
        
        deng_vec_t cur_pos;
        das_ObjColorData tmp_col;
        tmp_col.col_r = m_color.first;
        tmp_col.col_g = m_color.second;
        tmp_col.col_b = m_color.third;
        tmp_col.col_a = m_color.fourth;

        size_t l_index, r_index;

        // Z axis rays
        cur_pos = -(len / 2);
        for(l_index = 0; l_index < row_c * 3; l_index += 3) {
            m_asset.vertices.vuu[l_index].vert_x = -(len / 2);
            m_asset.vertices.vuu[l_index].vert_y = 0;
            m_asset.vertices.vuu[l_index].vert_z = cur_pos;
            
            m_asset.vertices.vuu[l_index + 1].vert_x = len / 2;
            m_asset.vertices.vuu[l_index + 1].vert_y = 0;
            m_asset.vertices.vuu[l_index + 1].vert_z = cur_pos;

            m_asset.vertices.vuu[l_index + 2].vert_x = -(len / 2);
            m_asset.vertices.vuu[l_index + 2].vert_y = 0;
            m_asset.vertices.vuu[l_index + 2].vert_z = cur_pos + DENG_GRID_THICKNESS;
            
            m_asset.indices.indices[l_index] = l_index;
            m_asset.indices.indices[l_index + 1] = l_index + 1;
            m_asset.indices.indices[l_index + 2] = l_index + 2;
            cur_pos += step;
        }

        // X axis rays
        r_index = l_index;
        cur_pos = -(len / 2);
        for(l_index = 0; l_index < row_c * 3; l_index += 3, r_index += 3) {
            m_asset.vertices.vuu[r_index].vert_x = cur_pos;
            m_asset.vertices.vuu[r_index].vert_y = 0;
            m_asset.vertices.vuu[r_index].vert_z = -(len / 2);

            m_asset.vertices.vuu[r_index + 1].vert_x = cur_pos;
            m_asset.vertices.vuu[r_index + 1].vert_y = 0;
            m_asset.vertices.vuu[r_index + 1].vert_z = len / 2;

            m_asset.vertices.vuu[r_index + 2].vert_x = cur_pos + DENG_GRID_THICKNESS;
            m_asset.vertices.vuu[r_index + 2].vert_y = 0;
            m_asset.vertices.vuu[r_index + 2].vert_z = -(len / 2);

            m_asset.indices.indices[r_index] = r_index;
            m_asset.indices.indices[r_index + 1] = r_index + 1;
            m_asset.indices.indices[r_index + 2] = r_index + 2;
            cur_pos += step;
        }
    }

    das_Asset GridGenerator::getGrid() { return m_asset; }
}
