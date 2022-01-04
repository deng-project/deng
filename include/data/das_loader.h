/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: das_loader.h - das file format reader header file
/// author: Karl-Mihkel Ott


#ifndef __DAS_READER_H
#define __DAS_READER_H

#ifdef __cplusplus
extern "C" {
#endif
#if defined(__DAS_READER_C) || defined(__DAM_C)
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>

    #include <common/base_types.h>
    #include <common/common.h>
    #include <common/cerr_def.h>

    #include <data/assets.h>
    #include <data/das_file.h>
    #include <data/das_check.h>
    #include <data/das_runtime.h>
    
    /**********************************************/
    /********** Header reading functions **********/
    /**********************************************/

    /// Read generic of data from __sbuf with bounds checking
    void __das_DataRead(void *buf, size_t chunk_size, size_t n, const char *file_name);

    /// Read asset information from INFO_HDR
    void __das_ReadINFO_HDR (
        das_INFO_HDR *out_hdr,
        const char *file_name
    );


    /// Verify that all custom headers are skipped from reading
    void __das_SkipMetaHeaders();
    

    /// Read meta information contained between BEG_HDR and END_HDR
    void __das_ReadMeta(const char *file_name);


    /// Read information from VERT_HDR
    void __das_ReadVERT_HDR (
        das_VERT_HDR *out_hdr,
        const char *file_name
    );

    
    /// Read information about one vertex element header type
    void __das_ReadGenVertHdr (
        __das_VertTemplate *out_hdr,
        char *hdr_name,
        const char *file_name
    );


    /// Read information from INDX_HDR
    void __das_ReadINDX_HDR (
        das_INDX_HDR *out_hdr,
        const char *file_name
    );


    /**************************************************/
    /******** File stream initialiser functions *******/
    /**************************************************/

    /// Read all data from asset file into buffer for reading
    void __das_ReadAssetFile(const char *file_name); 


    /// Free all resources allocated in asset reading 
    void __das_ReadCleanup();


    /*******************************************/
    /******** Vertex copying functions *********/
    /*******************************************/


    /// Copy asset vertices from buffer to out_vert
    /// This functions assumes that the asset has correct vertices count set and the offset is at the beginning of vertex data sequence
    static void __das_CopyVertices(das_Asset *asset, const char *file_name);


    /// Copy all indices from buffer to p_out_ind
    static void __das_CopyIndices(das_Asset *asset, deng_ui32_t ind_c, const char *file_name);


    /// Increment the reading offset by n bytes
    void __das_IncrementOffset(size_t n);


    /// Normalise all position vertices
    static void __das_VertNormalise(das_Asset *asset);


    /// Debugging
	#ifdef __DEBUG
		#define __ASSET_LOG_FILE        "assets.log"
		static void _db_Asset(das_Asset *asset, const char *file_name);
	#else
        #define __ASSET_LOG_FILE
	#endif


    
    // Global static variables for DAS reader
    static FILE *__sfile = NULL;
    static char *__cbuf = NULL;
    static deng_ui64_t __buf_size = 0;
    static deng_ui64_t __offset = 0;

    static size_t __meta_c = 0;
    static char **__meta_infos = NULL;
#endif


#ifdef __DEBUG
    void _das_RewriteAssetLogs();
	#define das_RewriteAssetLogs() _das_RewriteAssetLogs()
#else
	#define das_RewriteAssetLogs()
#endif


/// Read .das binary asset file into das_Asset instance
void das_LoadAsset (
    das_Asset *asset, 
    das_AssetMode dst_mode,
    das_ObjColorData color,
    deng_bool_t read_meta,
    const char *tex_uuid,
    const char *file_name
);


/// Get all the metadata which was read from the last read_meta specified das_LoadAsset call
char **das_GetMeta(size_t *p_meta_c);


/// Clean all memory allocated for meta data
void das_CleanMeta();

#ifdef __cplusplus
}
#endif
#endif
