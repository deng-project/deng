/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: assets.h - DENG asset and texture structure definitions 
/// author: Karl-Mihkel Ott

#ifndef ASSETS_H
#define ASSETS_H

#define DAS_DEFAULT_DIFFUSE_COLOR       (das_ObjColorData) {0.6f, 0.6f, 0.6f, 1.0f}   
#define DAS_DEFAULT_AMBIENT_COLOR       (das_ObjColorData) {0.4f, 0.4f, 0.4f, 1.0f}   
#define DAS_DEFAULT_SPECULAR_COLOR       (das_ObjColorData) {0.0f, 0.0f, 0.0f, 1.0f}   
#define FNAME_LEN       256

/// Universal asset element offset container struct 
typedef struct das_OffsetData {
    // Merged vertex data offset
    deng_ui64_t mer_offset;

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


/// Vertex data types for OpenGL
typedef struct das_GL3DVertex {
    das_ObjPosData pos;
    das_ObjTextureData tex;
    das_ObjNormalData norm;
} das_GL3DVertex;


typedef struct das_GL3DVertexUnmapped {
    das_ObjPosData pos;
    das_ObjNormalData norm;
} das_GL3DVertexUnmapped;


typedef struct das_GL2DVertex {
    das_ObjPosData2D pos;
    das_ObjTextureData tex;
} das_GL2DVertex;


typedef das_ObjPosData2D das_GL2DVertexUnmapped;


/// Dynamic vertices structures
typedef struct __das_VertDynamic3D {
    struct {
        das_ObjPosData *pos;
        size_t pn;
        das_ObjTextureData *tex;
        size_t tn;
        das_ObjNormalData *norm;
        size_t nn;
    } mul;

    struct {
        das_GL3DVertex *vert;
        das_GL3DVertexUnmapped *uvert;
        size_t n;
    } mer;
} __das_VertDynamic3D;


typedef union __das_VertDynamic2D {
    struct {
        das_ObjPosData2D *pos;
        size_t pn;
        das_ObjTextureData *tex;
        size_t tn;
        deng_ui32_t hier;
    } mul;

    struct {
        das_GL2DVertex *vert;
        das_GL2DVertexUnmapped *uvert;
        size_t n;
    } mer;
} __das_VertDynamic2D;


/// Universal dynamic vertices container union for both 3D and 2D assets
typedef union das_VertDynamic {
    __das_VertDynamic2D v2d;
    __das_VertDynamic3D v3d;
} das_VertDynamic;


/// Structure for universal heap allocated indices data storage
typedef struct das_IndicesDynamic {
    deng_idx_t *pos;
    deng_idx_t *tex;
    deng_idx_t *norm;
    deng_idx_t *gl;
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
    char src[FNAME_LEN];                 // Asset name that is displayed in editors 

    // Temporary variables, since no material system is present
    das_ObjColorData diffuse;   // Asset diffuse color property
    das_ObjColorData ambient;   // Asset ambient light intensity
    das_ObjColorData specular;  // Specular light intensity
    deng_vec_t phong_exp;       // Phong exponent that is used, with Blinn-Phong shading
    

    das_AssetMode asset_mode;       // Asset mode specifier
    deng_bool_t is_shown;           // False if asset is ignored from draw calls, otherwise True
    deng_bool_t ignore_transform;   // Toggle transformation on and off
    deng_bool_t is_opengl;          // Flag to determine if the asset is rendered in OpenGL backend renderer 
    deng_bool_t force_unmap;        // Force the asset not to use texture image and texture mapping
    das_VertDynamic vertices;       // All asset vertices that will be passed to command buffers
    das_IndicesDynamic indices;     // All asset indices that will be passed to command buffers
    das_OffsetData offsets;         // Offsets, which are taken into account, when allocating buffer storage
} das_Asset;


/// DENG texture struct 
typedef struct das_Texture {
    deng_Id uuid;
    deng_Id vk_id;
    deng_Id gl_id;
    char src[FNAME_LEN];
    deng_bool_t no_reg_cleanup;         // Set this as true if no automatic cleanup is wanted in registry destruction
    das_PixelDataDynamic pixel_data;
} das_Texture;

#endif
