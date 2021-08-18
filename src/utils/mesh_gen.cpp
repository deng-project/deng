/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: mesh_gen.cpp - mesh generation class implementations
/// author: Karl-Mihkel Ott


#define __MESH_GEN_CPP
#include <utils/mesh_gen.h>

namespace dengUtils {
    /****************************************/
    /********** CubeGenerator class *********/
    /****************************************/

    CubeGenerator::CubeGenerator() {
        // Base cube has total dimentions of 1.0 x 1.0 x 1.0
        m_base_cube_verts[0] = dengMath::vec4<deng_vec_t>{-0.5f, -0.5f, -0.5f, 1.0f};
        m_base_cube_verts[1] = dengMath::vec4<deng_vec_t>{0.5f, -0.5f, -0.5f, 1.0f};
        m_base_cube_verts[2] = dengMath::vec4<deng_vec_t>{0.5f, 0.5f, -0.5f, 1.0f};
        m_base_cube_verts[3] = dengMath::vec4<deng_vec_t>{-0.5f, 0.5f, -0.5f, 1.0f};

        m_base_cube_verts[4] = dengMath::vec4<deng_vec_t>{-0.5f, -0.5f, 0.5f, 1.0f};
        m_base_cube_verts[5] = dengMath::vec4<deng_vec_t>{0.5f, -0.5f, 0.5f, 1.0f};
        m_base_cube_verts[6] = dengMath::vec4<deng_vec_t>(0.5f, 0.5f, 0.5f, 1.0f);
        m_base_cube_verts[7] = dengMath::vec4<deng_vec_t>{-0.5f, 0.5f, 0.5f, 1.0f};


        // Specify the flat hexahedron texture mapping
        m_base_cube_tex_verts[0] = {-0.33f, -1.0f};
        m_base_cube_tex_verts[1] = {0.33f, -1.0f};

        m_base_cube_tex_verts[2] = {-1.0f, -0.5f};
        m_base_cube_tex_verts[3] = {-0.33f, -0.5f};
        m_base_cube_tex_verts[4] = {0.33f, -0.5f};
        m_base_cube_tex_verts[5] = {1.0f, -0.5f};

        m_base_cube_tex_verts[6] = {-1.0f, 0.0f};
        m_base_cube_tex_verts[7] = {-0.33f, 0.0f};
        m_base_cube_tex_verts[8] = {0.33f, 0.0f};
        m_base_cube_tex_verts[9] = {1.0f, 0.0f};

        m_base_cube_tex_verts[10] = {-0.33f, 0.5f};
        m_base_cube_tex_verts[11] = {0.33f, 0.5f};

        m_base_cube_tex_verts[11] = {-0.33f, 1.0f};
        m_base_cube_tex_verts[12] = {0.33f, 1.0f};

        m_base_cube_inds = {{
            {5, 1}, {4, 0}, {7, 3}, {7, 3}, {6, 4}, {5, 1}, // back face
            {4, 2}, {0, 6}, {3, 7}, {3, 7}, {7, 3}, {4, 2}, // left face
            {0, 10}, {1, 11}, {2, 8}, {2, 8}, {3, 7}, {0, 10}, // front face
            {1, 9}, {5, 5}, {6, 4}, {6, 4}, {2, 8}, {1, 9}, // right face
            {4, 12}, {5, 13}, {1, 11}, {1, 11}, {0, 10}, {4, 12}, // top face
            {3, 7}, {2, 8}, {6, 4}, {6, 4}, {7, 3}, {3, 7} // bottom face
        }};
    }


    /// Construct transformation matrix based on given parameters
    dengMath::mat4<deng_vec_t> CubeGenerator::__mkTransformMatrix (
        const dengMath::vec3<deng_vec_t> &pos,
        const dengMath::vec3<deng_vec_t> &size,
        const dengMath::vec3<deng_vec_t> &origin
    ) {
        dengMath::vec4<deng_vec_t> rl_pos = {pos.first, pos.second, pos.third, 1.0f};
        dengMath::mat4<deng_vec_t> tf_mat;

        // Set scaling of the cube
        tf_mat.row1.first = size.first;
        tf_mat.row2.second = size.second;
        tf_mat.row3.third = size.third;
        tf_mat.row4.fourth = 1.0f;

        // Calculate position that has considered origin
        rl_pos = tf_mat * rl_pos;
        rl_pos += {origin.first, origin.second, origin.third, 1.0f};

        // Set transformation
        tf_mat.row1.fourth = rl_pos.first;
        tf_mat.row2.fourth = rl_pos.second;
        tf_mat.row3.fourth = rl_pos.third;
        tf_mat.row4.fourth = rl_pos.fourth;

        return tf_mat;
    }


    /// Generate unmapped cube with position, size, origin and color parameters
    void CubeGenerator::generateUnmappedCube (
        const dengMath::vec3<deng_vec_t> &pos,
        const dengMath::vec3<deng_vec_t> &size,
        const dengMath::vec3<deng_vec_t> &origin,
        std::vector<das_ObjPosData> &out_pos,
        std::vector<deng_ui32_t> &out_ind
    ) {
        // Generate transformation matrix for the cube
        dengMath::mat4<deng_vec_t> tf_mat = __mkTransformMatrix(pos, size, origin); 

        // Find the previous position and indices sizes and resize their corresponding vectors
        size_t old_pos_c = out_pos.size();
        size_t old_ind_c = out_ind.size();
        out_pos.resize(old_pos_c + m_base_cube_verts.size());
        out_ind.resize(old_ind_c + m_base_cube_inds.size());

        // For each new vertex in out_pos assign new values according to the transformation matrix
        for(size_t i = old_pos_c; i < out_pos.size(); i++) {
            dengMath::vec4<deng_vec_t> tmp = tf_mat * m_base_cube_verts[i - old_pos_c];
            out_pos[i].vert_x = tmp.first;
            out_pos[i].vert_y = tmp.second;
            out_pos[i].vert_z = tmp.third;
        }

        // For each new index in out_ind assign new values from base indices vector
        for(size_t i = old_ind_c; i < out_ind.size(); i++) 
            out_ind[i] = m_base_cube_inds[i].first;
    }


    /// Generate texture mapped cube with position, size and origin parameters
    void CubeGenerator::generateMappedCube (
        const dengMath::vec3<deng_vec_t> &pos,
        const dengMath::vec3<deng_vec_t> &size,
        const dengMath::vec3<deng_vec_t> &origin,
        std::vector<das_ObjPosData> &out_pos,
        std::vector<das_ObjTextureData> &out_tex,
        std::vector<deng_ui32_t> &out_pos_ind,
        std::vector<deng_ui32_t> &out_tex_ind
    ) {
        // Generate transformation matrix for the cube
        dengMath::mat4<deng_vec_t> tf_mat = __mkTransformMatrix(pos, size, origin); 

        // Find the previous output vector sizes and resize them accordingly
        size_t old_pos_c = out_pos.size();
        size_t old_tex_c = out_tex.size();
        size_t old_pos_ind_c = out_pos_ind.size();
        size_t old_tex_ind_c = out_tex_ind.size();

        out_pos.resize(old_pos_c + m_base_cube_verts.size());
        out_tex.resize(old_tex_c + m_base_cube_verts.size());
        out_pos_ind.resize(old_pos_ind_c + m_base_cube_inds.size());
        out_tex_ind.resize(old_tex_ind_c + m_base_cube_inds.size());

        // For each new position vertex assign new value
        for(size_t i = old_pos_c; i < out_pos.size(); i++) {
            dengMath::vec4<deng_vec_t> tmp = tf_mat * m_base_cube_verts[i - old_pos_c];
            out_pos[i].vert_x = tmp.first;
            out_pos[i].vert_y = tmp.second;
            out_pos[i].vert_z = tmp.third;
        }

        // For each new texture vertex assign new value
        for(size_t i = old_tex_c; i < out_tex.size(); i++)
            out_tex[i] = m_base_cube_tex_verts[i];


        // For each new index assign new value
        for(size_t i = old_pos_ind_c, j = old_tex_ind_c; i < out_pos_ind.size(); i++, j++) {
            out_pos_ind[i] = m_base_cube_inds[i - old_pos_ind_c].first;
            out_tex_ind[j] = m_base_cube_inds[j - old_tex_ind_c].second;
        }
    }


    /// Create new unmapped cube asset
    das_Asset CubeGenerator::generateUnmappedCubeAsset (
        const dengMath::vec3<deng_vec_t> &pos, 
        const dengMath::vec3<deng_vec_t> &size, 
        const dengMath::vec3<deng_vec_t> &origin
    ) {
        std::vector<das_ObjPosData> vert;
        std::vector<deng_ui32_t> ind;

        // First generate base vertices for the cube
        generateUnmappedCube(pos, size, origin, vert, ind);

        // Create a new asset, set the asset mode and generate uuid
        das_Asset asset = { 0 };
        asset.asset_mode = DAS_ASSET_MODE_3D_UNMAPPED;
        asset.uuid = uuid_Generate();

        // Set vertices sizes
        asset.vertices.v3d.mul.pn = vert.size();
        asset.indices.n = ind.size();

        // Allocate memory for vertices and indices
        asset.vertices.v3d.mul.pos = (das_ObjPosData*) malloc(asset.vertices.v3d.mul.pn * sizeof(das_ObjPosData));
        asset.indices.pos = (deng_ui32_t*) malloc(asset.indices.n * sizeof(deng_ui32_t));

        // Copy all vertices and indices to the asset
        memcpy(asset.vertices.v3d.mul.pos, vert.data(),
            asset.vertices.v3d.mul.pn * sizeof(das_ObjPosData));

        memcpy(asset.indices.pos, ind.data(),
            asset.indices.n * sizeof(deng_ui32_t));

        // Generate vertex normals
        das_MkAssetNormals(&asset);
        return asset;
    }


    /// Generate texture mapped cube asset
    das_Asset CubeGenerator::generateMappedCubeAsset (
        const dengMath::vec3<deng_vec_t> &pos,
        const dengMath::vec3<deng_vec_t> &size,
        const dengMath::vec3<deng_vec_t> &origin
    ) {
        // Create vectors for storing retrieved data
        std::vector<das_ObjPosData> pos_vert(m_base_cube_verts.size());
        std::vector<das_ObjTextureData> tex_vert(m_base_cube_tex_verts.size());
        std::vector<deng_ui32_t> pos_ind(m_base_cube_inds.size());
        std::vector<deng_ui32_t> tex_ind(m_base_cube_inds.size());
        
        // First generate base vertices for the cube
        generateMappedCube(pos, size, origin, pos_vert, tex_vert, pos_ind, tex_ind);

        // Create a new asset, set the asset mode and is_shown and finally generate uuid
        das_Asset asset = { 0 };
        asset.asset_mode = DAS_ASSET_MODE_3D_UNMAPPED;
        asset.is_shown = true;
        asset.uuid = uuid_Generate();

        // Set the vertices and indices sizes
        asset.vertices.v3d.mul.pn = pos_vert.size();
        asset.vertices.v3d.mul.tn = tex_vert.size();
        asset.indices.n = pos_ind.size();

        // Allocate memory for vertices and indices
        asset.vertices.v3d.mul.pos = (das_ObjPosData*) malloc(asset.vertices.v3d.mul.pn * sizeof(das_ObjPosData)); 
        asset.vertices.v3d.mul.tex = (das_ObjTextureData*) malloc(asset.vertices.v3d.mul.tn * sizeof(das_ObjTextureData));
        asset.indices.pos = (deng_ui32_t*) malloc(asset.indices.n * sizeof(deng_ui32_t));
        asset.indices.tex = (deng_ui32_t*) malloc(asset.indices.n * sizeof(deng_ui32_t));

        // Copy all vertices and indices to asset instace
        memcpy(asset.vertices.v3d.mul.pos, pos_vert.data(),
            pos_vert.size() * sizeof(das_ObjPosData));

        memcpy(asset.vertices.v3d.mul.tex, tex_vert.data(),
            tex_vert.size() * sizeof(das_ObjTextureData));

        memcpy(asset.indices.pos, pos_ind.data(),
            pos_ind.size() * sizeof(deng_ui32_t));

        memcpy(asset.indices.tex, tex_ind.data(),
            tex_ind.size() * sizeof(deng_ui32_t));

        // Generate vertex normals
        das_MkAssetNormals(&asset);
        return asset;
    }
}
