/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: mesh_gen.h - mesh generation class header
/// author: Karl-Mihkel Ott


#ifndef __MESH_GEN_H
#define __MESH_GEN_H


#ifdef __MESH_GEN_CPP
    #include <array>
    #include <vector>
    #include <string.h>
    #include <stdlib.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <common/uuid.h>
    #include <common/hashmap.h>
    #include <data/assets.h>
    #include <data/das_runtime.h>
    #include <math/deng_math.h>
#endif

#define __CUBE_INDICES_C            36
#define __CUBE_VERT_C               8
#define __CUBE_TEX_MAP_VERT_C       14

namespace dengUtils {

    /// This struct contains all methods for generic cube generation
    class CubeGenerator {
    private:
        std::array<dengMath::vec4<deng_vec_t>, __CUBE_VERT_C> m_base_cube_verts;
        std::array<dengMath::vec2<deng_ui32_t>, __CUBE_INDICES_C> m_base_cube_inds;
        std::array<das_ObjTextureData, __CUBE_TEX_MAP_VERT_C> m_base_cube_tex_verts;

    private:

        /// Construct transformation matrix based on given parameters
        dengMath::mat4<deng_vec_t> __mkTransformMatrix (
            const dengMath::vec3<deng_vec_t> &pos,
            const dengMath::vec3<deng_vec_t> &size,
            const dengMath::vec3<deng_vec_t> &origin
        );

    public:
        CubeGenerator();

        /// Generate unmapped cube with position, size, origin and color parameters
        void generateUnmappedCube (
            const dengMath::vec3<deng_vec_t> &pos,
            const dengMath::vec3<deng_vec_t> &size,
            const dengMath::vec3<deng_vec_t> &origin,
            std::vector<das_ObjPosData> &vert,
            std::vector<deng_ui32_t> &ind
        );


        /// Generate texture mapped cube with position, size and origin parameters
        void generateMappedCube (
            const dengMath::vec3<deng_vec_t> &pos,
            const dengMath::vec3<deng_vec_t> &size,
            const dengMath::vec3<deng_vec_t> &origin,
            std::vector<das_ObjPosData> &out_pos,
            std::vector<das_ObjTextureData> &out_tex,
            std::vector<deng_ui32_t> &out_pos_ind,
            std::vector<deng_ui32_t> &out_tex_ind
        );

        
        /// Create new unmapped cube asset
        das_Asset generateUnmappedCubeAsset (
            const dengMath::vec3<deng_vec_t> &pos,
            const dengMath::vec3<deng_vec_t> &size,
            const dengMath::vec3<deng_vec_t> &origin
        );

        
        /// Generate texture mapped cube asset
        das_Asset generateMappedCubeAsset (
            const dengMath::vec3<deng_vec_t> &pos,
            const dengMath::vec3<deng_vec_t> &size,
            const dengMath::vec3<deng_vec_t> &origin
        );
    };
}

#endif
