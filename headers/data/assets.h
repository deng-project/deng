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


#ifndef ASSETS_H
#define ASSETS_H

#define DAS_DEFAULT_DIFFUSE_COLOR       (das_ObjColorData) {0.6f, 0.6f, 0.6f, 1.0f}   
#define DAS_DEFAULT_AMBIENT_COLOR       (das_ObjColorData) {0.4f, 0.4f, 0.4f, 1.0f}   
#define DAS_DEFAULT_SPECULAR_COLOR       (das_ObjColorData) {0.0f, 0.0f, 0.0f, 1.0f}   

/// Universal asset element offset container struct 
typedef struct das_OffsetData {
    // Vertices offsets
    deng_ui64_t pos_offset;
    deng_ui64_t tex_offset;
    deng_ui64_t nor_offset;

    // Indices buffer offset
    deng_ui64_t ind_offset;

    // Color uniform buffer offset
    deng_ui64_t ubo_offset;
} das_OffsetData;


/// Base structures for storing model vertices data 
typedef struct das_ObjPosData {
    deng_vec_t vert_x;
    deng_vec_t vert_y;
    deng_vec_t vert_z;
} das_ObjPosData;


typedef struct das_ObjPosData2D {
    deng_vec_t vert_x;
    deng_vec_t vert_y;
} das_ObjPosData2D;


/// Base struct for storing information about model's 
/// texture mapping vertices
typedef struct das_ObjTextureData {
    deng_vec_t tex_x;
    deng_vec_t tex_y;
} das_ObjTextureData;


/// Base struct for storing information about model's
/// vertex normals vertices
typedef struct das_ObjNormalData {
    deng_vec_t nor_x;
    deng_vec_t nor_y;
    deng_vec_t nor_z;
} das_ObjNormalData;


/// Base struct for asset vertices RGB color specification
typedef struct das_ObjColorData {
    deng_vec_t col_r;
    deng_vec_t col_g;
    deng_vec_t col_b;
    deng_vec_t col_a;
} das_ObjColorData;


/// Dynamic vertices structures
typedef struct __das_VertDynamic3D {
    das_ObjPosData *pos;
    size_t pn;
    das_ObjTextureData *tex;
    size_t tn;
    das_ObjNormalData *norm;
    size_t nn;
} __das_VertDynamic3D;


typedef struct __das_VertDynamic2D {
    das_ObjPosData2D *pos;
    size_t pn;
    das_ObjTextureData *tex;
    size_t tn;
    deng_ui32_t hier;
} __das_VertDynamic2D;


/// Universal dynamic vertices container union for both 3D and 2D assets
typedef union das_VertDynamic {
    __das_VertDynamic2D v2d;
    __das_VertDynamic3D v3d;
} das_VertDynamic;


/// Structure for universal heap allocated indices data storage
typedef struct das_IndicesDynamic {
    deng_ui32_t *pos;
    deng_ui32_t *tex;
    deng_ui32_t *norm;
    size_t n;
} das_IndicesDynamic;


/// Structure for universal texture image bitmap data storage
typedef struct das_PixelDataDynamic {
    deng_ui8_t *pixel_data;
    size_t size;
    deng_ui16_t width;
    deng_ui16_t height;
    deng_ui64_t memory_offset;
} das_PixelDataDynamic;


/// Specify the type of the asset 
typedef enum das_AssetMode {
    DAS_ASSET_MODE_UNDEFINED                        = -1,
    DAS_ASSET_MODE_3D_TEXTURE_MAPPED                = 0,
    __DAS_ASSET_MODE_3D_TEXTURE_MAPPED_UNOR         = 1,
    DAS_ASSET_MODE_3D_UNMAPPED                      = 2,
    __DAS_ASSET_MODE_3D_UNMAPPED_UNOR               = 3,
    DAS_ASSET_MODE_2D_TEXTURE_MAPPED                = 4,
    DAS_ASSET_MODE_2D_UNMAPPED                      = 5,
    DAS_ASSET_MODE_FIRST                            = DAS_ASSET_MODE_UNDEFINED,
    DAS_ASSET_MODE_LAST                             = DAS_ASSET_MODE_2D_UNMAPPED
} das_AssetMode;


/// Main asset structure for 2D and 3D assets
typedef struct das_Asset {
    deng_Id uuid;               // Identifier that is used to access the asset from the registry
    deng_Id tex_uuid;           // Texture id that the current asset is bound to (can be set to zero, if unmapped)
    deng_Id vk_id;              // Vulkan asset id, that is generated when Vulkan is used as the backend api
    deng_Id gl_id;              // OpenGL asset id, that is generated when OpenGL is used as the backend api
    char *name;                 // Asset name that is displayed in editors 

    // Temporary variables, since no material system is present
    das_ObjColorData diffuse;   // Asset diffuse color property
    das_ObjColorData ambient;   // Asset ambient light intensity
    das_ObjColorData specular;  // Specular light intensity
    deng_vec_t phong_exp;       // Phong exponent that is used, with Blinn-Phong shading
    

    das_AssetMode asset_mode;   // Asset mode specifier
    deng_bool_t is_shown;       // False if asset is ignored from draw calls, otherwise True
    deng_bool_t is_transformed; // Toggle transformation on and off
    deng_bool_t force_unmap;    // Force the asset not to use texture image and texture mapping
    das_VertDynamic vertices;   // All asset vertices that will be passed to command buffers
    das_IndicesDynamic indices; // All asset indices that will be passed to command buffers
    das_OffsetData offsets;     // Offsets, which are taken into account, when allocating buffer storage
} das_Asset;


/// Main texture struct 
typedef struct das_Texture {
    deng_Id uuid;
    deng_Id vk_id;
    deng_Id gl_id;
    char *name;
    deng_bool_t no_reg_cleanup;         // Set this as true if no automatic cleanup is wanted in registry destruction
    das_PixelDataDynamic pixel_data;
} das_Texture;

#endif
