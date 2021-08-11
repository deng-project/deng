/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: das_reader.c - das file format reader header file
/// author: Karl-Mihkel Ott

#define __DAS_READER_C
#include <data/das_loader.h>


/// Temporarely log every vertex and index
static void _db_Asset(das_Asset *asset, const char *file_name) {
    const char o_file = cm_GetLogFile();
    cm_OpenLogger(__ASSET_LOG_FILE, false);

    char buf[512];
    sprintf(buf, "\n\nFILE: %s", file_name);
    cm_LogWrite(buf);

    // Log every position vertex
    for(size_t i = 0; i < asset->vertices.v3d.mul.pn; i++) {
		ZERO_MEM(buf);
        sprintf(buf, "v %f %f %f", asset->vertices.v3d.mul.pos[i].vert_x, 
            asset->vertices.v3d.mul.pos[i].vert_y, asset->vertices.v3d.mul.pos[i].vert_z);

        cm_LogWrite(buf);
    }

    // Log every texture vertex
    for(size_t i = 0; i < asset->vertices.v3d.mul.tn; i++) {
		ZERO_MEM(buf);
        sprintf(buf, "vt %f %f", asset->vertices.v3d.mul.tex[i].tex_x, 
            asset->vertices.v3d.mul.tex[i].tex_y);

        cm_LogWrite(buf);
    }

    // Log every vertex normal
    for(size_t i = 0; i < asset->vertices.v3d.mul.nn; i++) {
		ZERO_MEM(buf);
        sprintf(buf, "vn %f %f %f", asset->vertices.v3d.mul.norm[i].nor_x, 
            asset->vertices.v3d.mul.norm[i].nor_y, asset->vertices.v3d.mul.norm[i].nor_z);

        cm_LogWrite(buf);
    }

    // Log every face
    for(size_t i = 0; i < asset->indices.n; i += 3) {
		ZERO_MEM(buf);
        sprintf(buf, "f %d/%d/%d %d/%d/%d %d/%d/%d", asset->indices.pos[i] + 1, 
            asset->indices.tex[i] + 1, asset->indices.norm[i] + 1, asset->indices.pos[i + 1] + 1,
            asset->indices.tex[i + 1] + 1, asset->indices.norm[i + 1] + 1, asset->indices.pos[i + 2] + 1,
            asset->indices.tex[i + 2] + 1, asset->indices.norm[i + 2] + 1);

        cm_LogWrite(buf);
    }

    cm_CloseLogger();
}


void _das_RewriteAssetLogs() {
    FILE* file = fopen(__ASSET_LOG_FILE, "wb");
    const char* emsg = "#ENTRY POINT\n";
    fwrite(emsg, strlen(emsg), 1, file);
    DENG_ASSERT(file, "Failed to open file stream for asset logs file");
    fclose(file);
}


/// Read .das binary asset file into das_Asset instance
void das_LoadAsset (
    das_Asset *asset, 
    das_AssetMode dst_mode,
    das_ObjColorData color,
    deng_bool_t read_meta,
    const char *tex_uuid,
    const char *file_name
) {
    asset->asset_mode = dst_mode;
    asset->tex_uuid = (char*) tex_uuid;
    asset->diffuse = DAS_DEFAULT_DIFFUSE_COLOR;
    asset->ambient = DAS_DEFAULT_AMBIENT_COLOR;
    asset->specular = DAS_DEFAULT_SPECULAR_COLOR;
    asset->is_shown = true;
    asset->force_unmap = false;

    // Write all file contents into a buffer
    __das_ReadAssetFile((const char*) file_name);

    das_INFO_HDR inf_hdr = { 0 };
    das_VERT_HDR vert_hdr = { 0 };
    das_INDX_HDR indx_hdr = { 0 };

    __das_ReadINFO_HDR(&inf_hdr, file_name);

    // Copy uuid from read info header instance to asset uuid instance
    asset->uuid = (char*) calloc(__DAS_UUID_LEN + 1, sizeof(char));
    strcpy(asset->uuid, inf_hdr.uuid);

    // Check if metadata should be read or ignored
    if(read_meta) 
        __das_ReadMeta(file_name);
    else __das_SkipMetaHeaders(file_name);

    // Read vertices header
    __das_ReadVERT_HDR(&vert_hdr, file_name);
    __das_CopyVertices(asset, file_name);

    // Read and copy index data
    __das_ReadINDX_HDR(&indx_hdr, file_name);
    __das_CopyIndices(asset, indx_hdr.ind_c, file_name);

    // Check if vertices are unnormalised and if they are then normalise
    if(inf_hdr.asset_type == __DAS_ASSET_MODE_3D_UNMAPPED_UNOR ||
       inf_hdr.asset_type == __DAS_ASSET_MODE_3D_TEXTURE_MAPPED_UNOR) {
        das_MkAssetNormals(asset);
    }

    if(asset->asset_mode == DAS_ASSET_MODE_UNDEFINED)
        asset->asset_mode = DAS_ASSET_MODE_3D_TEXTURE_MAPPED;

    __das_ReadCleanup();

	#ifdef __DEBUG
		_db_Asset(asset, file_name);
	#endif
}


/// Get all the metadata which was read from the last read_meta specified das_LoadAsset call
char **das_GetMeta(size_t *p_meta_c) {
    *p_meta_c = __meta_c;
    return __meta_infos;
}


/// Clean all memory allocated for meta data
void das_CleanMeta() {
    for(size_t i = 0; i < __meta_c; i++)
        free(__meta_infos[i]);

    if(__meta_c) {
        free(__meta_infos);
        __meta_infos = NULL;
        __meta_c = 0;
    }
}


/**********************************************/
/********** Header reading functions **********/
/**********************************************/


/// Read generic of data from __cbuf with bounds checking
void __das_DataRead(void *buf, size_t chunk_size, size_t n, const char *file_name) {
    das_ErrBufferReadCheck(chunk_size * n, __buf_size, file_name);
    memcpy(buf, __cbuf + __offset, chunk_size * n);
    __offset += chunk_size * n;
}


/// Read asset information from INFO_HDR
void __das_ReadINFO_HDR(das_INFO_HDR *out_hdr, const char *file_name) {
    // INFO_HDR is always the first header in .das file
    __das_DataRead(out_hdr, sizeof(das_INFO_HDR), 1, file_name);

    // Copy header name into larger array for verification purposes
    char pad_name[9] = {0};
    strncpy(pad_name, out_hdr->hdr_name, 8);

    // Verify that file magic's number matches any of the supported format magic numbers
    if(das_MagicNumberCheck(out_hdr->magic_number) == DAS_ERROR_INVALID_MAGIC_NUMBER)
        __DAS_READ_INVALID_MAGIC_NUM(file_name);

    // Verify info header integrity
    if(strcmp(pad_name, __DAS_INFO_HEADER_NAME) || out_hdr->cmpr) 
        __DAS_READ_CORRUPT_ERROR(file_name);
}


/// Verify that all custom headers are skipped from reading
void __das_SkipMetaHeaders(const char *file_name) {
    deng_ui32_t *indices;
    deng_ui32_t ind_c = 0; 

    // Find all instances for custom header beginnings
    cm_FindnAllSubstrings (
        __cbuf,
        __buf_size,
        __DAS_META_HEADER_NAME,
        strlen(__DAS_META_HEADER_NAME),
        &indices,
        &ind_c
    ); 

    if(ind_c) {
        __offset = indices[ind_c - 1];
        deng_ui32_t hdr_size = 0;

        // Buffer bounds check
        das_ErrBufferReadCheck(__offset + sizeof(deng_ui32_t) + 8, __buf_size, file_name);
        memcpy(&hdr_size, __cbuf + __offset + 8, sizeof(deng_ui32_t));
        das_ErrBufferReadCheck(__offset + hdr_size, __buf_size, file_name);

        __offset += hdr_size;
        free(indices);
    }
}


/// Read meta information contained between BEG_HDR and END_HDR
/// This function returns pointer to heap allocated memory, manual cleanup is necessary
void __das_ReadMeta(const char *file_name) {
    // Check if previous metadata has been allocated and if it has 
    // then perform cleanup
    if(__meta_c && __meta_infos) {
        for(size_t i = 0; i < __meta_c; i++)
            free(__meta_infos[i]);
        free(__meta_infos);

        __meta_infos = NULL;
        __meta_c = 0;
    }

    deng_ui32_t *indices = NULL;
    deng_ui32_t ind_c = 0;
    
    // Find all instances of custom headers
    cm_FindnAllSubstrings (
        __cbuf,
        __buf_size,
        __DAS_META_HEADER_NAME,
        strlen(__DAS_META_HEADER_NAME),
        &indices,
        &ind_c
    );

    __meta_c = (deng_ui64_t) ind_c;
    __meta_infos = (char**) malloc(ind_c * sizeof(char*));

    for(deng_ui32_t i = 0; i < __meta_c; i++) {
        // Skip header size reading and read data size directly
        __offset = indices[i] + 12;
        deng_ui32_t data_len = 0;
        memcpy(&data_len, __cbuf + __offset, sizeof(deng_ui32_t));
        __das_DataRead(&data_len, sizeof(deng_ui32_t), 1, file_name);
        
        // Copy all metadata information to __meta_infos buffer
        __offset += 4;
        __meta_infos[i] = (char*) calloc(data_len + 1, sizeof(char));
        memcpy(__meta_infos[i], __cbuf + __offset, data_len * sizeof(char));
        
        __offset += data_len;
    }
}


/// Read asset information from VERT_HDR
void __das_ReadVERT_HDR (
    das_VERT_HDR *out_hdr,
    const char *file_name
) {
    if(__offset + sizeof(das_VERT_HDR) >= __buf_size)
        __DAS_READ_CORRUPT_ERROR(file_name);

    // Copy the first part of the buffer into das_VERT_HDR struct
    memcpy(out_hdr, __cbuf + __offset, sizeof(das_VERT_HDR));
    __offset += sizeof(das_VERT_HDR);

    char pad_hdr[9] = {0};
    strncpy(pad_hdr, out_hdr->hdr_name, 8);

    // Check if header name is correct
    if(strcmp(pad_hdr, __DAS_VERTICES_HEADER_NAME))
        __DAS_READ_CORRUPT_ERROR(file_name);
}


/// Read information about one vertex element header type
void __das_ReadGenVertHdr (
    __das_VertTemplate *out_hdr,
    char *hdr_name,
    const char *file_name
) {
    __das_DataRead(out_hdr, sizeof(__das_VertTemplate), 1, file_name);
    char pad_hdr[8] = {0};
    strncpy(pad_hdr, out_hdr->hdr_name, 8);

    // Check if header name is correct
    if(strcmp(pad_hdr, hdr_name))
        __DAS_READ_CORRUPT_ERROR(file_name);
}


/// Read asset information from INDX_HDR
void __das_ReadINDX_HDR (
    das_INDX_HDR *out_hdr,
    const char *file_name
) {
    __das_DataRead(out_hdr, sizeof(das_INDX_HDR), 1, file_name);
    char pad_hdr[9] = {0};
    strncpy(pad_hdr, out_hdr->hdr_name, 8);

    // Check if header name is correct
    if(strcmp(pad_hdr, __DAS_INDICES_HEADER_NAME)) 
        __DAS_READ_CORRUPT_ERROR(file_name);
}


/// Read all data from asset file into buffer for reading
void __das_ReadAssetFile(const char *file_name) {
    FILE *file;
    __offset = 0;
    file = fopen(file_name, "rb");

    if(!file) FILE_ERR(file_name);

    fseek(file, 0, SEEK_END);
    __buf_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    __cbuf = (char*) calloc (
        __buf_size + 1,
        sizeof(char)
    );

    size_t res = fread(__cbuf, sizeof(char), __buf_size, file);
    if(!res) FILE_ERR(file_name);

    fclose(file);
}


/// Free all resources allocated in asset reading 
void __das_ReadCleanup() {
    free(__cbuf);
}


/// Copy asset vertices from buffer to out_vert
void __das_CopyVertices (
    das_Asset *asset,
    const char *file_name
) {
    das_VPOS_HDR pos_hdr = { 0 };
    das_VTEX_HDR tex_hdr = { 0 };
    das_VNOR_HDR nor_hdr = { 0 };

    // Read the position vertices header
    __das_DataRead(&pos_hdr, sizeof(das_VPOS_HDR), 1, file_name);
    
    // Allocate enough memory for position vertices
    asset->vertices.v3d.mul.pn = pos_hdr.vert_c;
    asset->vertices.v3d.mul.pos = (das_ObjPosData*) malloc(cm_ToPow2I64(asset->vertices.v3d.mul.pn * 
        sizeof(das_ObjPosData)));

    // Read position vertices
    __das_DataRead(asset->vertices.v3d.mul.pos, sizeof(das_ObjPosData), asset->vertices.v3d.mul.pn, file_name);

    // Check if vertex textures should be read
    if(asset->asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED ||
       asset->asset_mode == __DAS_ASSET_MODE_3D_TEXTURE_MAPPED_UNOR || 
       asset->asset_mode == DAS_ASSET_MODE_UNDEFINED) {
        // Read header info
        __das_DataRead(&tex_hdr, sizeof(das_VTEX_HDR), 1, file_name);
        das_CheckHdrName(tex_hdr.hdr_name, __DAS_TEX_POSITION_HEADER_NAME, file_name);

        // Allocate memory for texture vertices
        asset->vertices.v3d.mul.tn = tex_hdr.vert_c;
        asset->vertices.v3d.mul.tex = (das_ObjTextureData*) malloc(cm_ToPow2I64(asset->vertices.v3d.mul.tn * 
            sizeof(das_ObjTextureData)));

        // Read texture vertices
        __das_DataRead(asset->vertices.v3d.mul.tex, sizeof(das_ObjTextureData), asset->vertices.v3d.mul.tn, file_name);
    }

    // 
    if(asset->asset_mode == DAS_ASSET_MODE_3D_UNMAPPED ||
       asset->asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED ||
       asset->asset_mode == DAS_ASSET_MODE_UNDEFINED) {
        // Read header info
        __das_DataRead(&nor_hdr, sizeof(das_VNOR_HDR), 1, file_name);
        das_CheckHdrName(nor_hdr.hdr_name, __DAS_NORM_POSITION_HEADER_NAME, file_name);

        // Allocate memory for vertex normals
        asset->vertices.v3d.mul.nn = nor_hdr.vert_c;
        asset->vertices.v3d.mul.norm = (das_ObjNormalData*) malloc(cm_ToPow2I64(asset->vertices.v3d.mul.nn) *
            sizeof(das_ObjNormalData));

        // Read vertex normals
        __das_DataRead(asset->vertices.v3d.mul.norm, sizeof(das_ObjNormalData), asset->vertices.v3d.mul.nn, file_name);
    }
}


/// Copy all indices from buffer to p_out_ind. This function allocates memory and copies the data from file buffer to *p_out_ind
void __das_CopyIndices (
    das_Asset *asset,
    deng_ui32_t ind_c,
    const char *file_name
) {
    asset->indices.n = ind_c;

    // Read all position indices
    asset->indices.pos = (deng_ui32_t*) malloc(cm_ToPow2I64(asset->indices.n * sizeof(deng_ui32_t)));
    __das_DataRead(asset->indices.pos, sizeof(deng_ui32_t), asset->indices.n, file_name);

    // Check if texture indices should be read
    if(asset->asset_mode == __DAS_ASSET_MODE_3D_TEXTURE_MAPPED_UNOR || 
       asset->asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED ||
       asset->asset_mode == DAS_ASSET_MODE_UNDEFINED) {
        asset->indices.tex = (deng_ui32_t*) malloc(cm_ToPow2I64(asset->indices.n * sizeof(deng_ui32_t)));
        __das_DataRead(asset->indices.tex, sizeof(deng_ui32_t), asset->indices.n, file_name);
    }

    // Check if vertex normal indices should be read
    if(asset->asset_mode == DAS_ASSET_MODE_3D_UNMAPPED ||
       asset->asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED ||
       asset->asset_mode == DAS_ASSET_MODE_UNDEFINED) {
        asset->indices.norm = (deng_ui32_t*) malloc(cm_ToPow2I64(asset->indices.n * sizeof(deng_ui32_t)));
        __das_DataRead(asset->indices.norm, sizeof(deng_ui32_t), asset->indices.n, file_name);
    }
}


/// Increment the reading offset by n bytes
void __das_IncrementOffset(size_t n) {
    __offset += n;
}
