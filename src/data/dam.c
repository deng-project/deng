/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: dam.h - deng asset manager header file
/// author: Karl-Mihkel Ott

#define __DAM_C
#include <data/dam.h>


/*
 * Read information about the asset
 * Information is following:
 *  - UUID of the asset
 *  - Date and time of the creation
 *  - Type of vertices
 *  - Total count of unique vertices
 *  - Total count of drawn vertices
 */
void __dam_ListAsset(dam_ListInfo li) {
    das_INFO_HDR inf_hdr = { 0 };
    das_VERT_HDR vert_hdr = { 0 };
    das_INDX_HDR indx_hdr = { 0 };

    // Specific vertex header reading
    das_VPOS_HDR vpos_hdr = { 0 };
    das_VTEX_HDR vtex_hdr = { 0 };
    das_VNOR_HDR vnor_hdr = { 0 };

    __das_ReadAssetFile(li.file_name);
    __das_ReadINFO_HDR(&inf_hdr, li.file_name);
 
    char *date; char *time;
    cm_FormatTimeFromEpoch(DATE_FORMAT_YMD_SP_HYPHEN, TIME_FORMAT_24_HOUR_SP_COLON,
        inf_hdr.time_st, &date, &time);

    // Read metadata
    __das_ReadMeta(li.file_name);

    // Create padded strings
    char pad_uuid[37] = { 0 };
    strncpy(pad_uuid, inf_hdr.uuid, 36);

    // Read the initial part of the VERT_HDR
    __das_ReadVERT_HDR(&vert_hdr, li.file_name);
    __das_ReadGenVertHdr(&vpos_hdr, (char*) "VPOS_HDR", li.file_name);
    __das_IncrementOffset(vpos_hdr.vert_c * sizeof(das_ObjPosData));

    // Increment reading offset for file buffer
    switch(inf_hdr.asset_type) {
    case DAS_ASSET_MODE_3D_UNMAPPED:
        // Read vertex normals header
        __das_ReadGenVertHdr(&vnor_hdr, (char*) __DAS_NORM_POSITION_HEADER_NAME, li.file_name);
        __das_IncrementOffset(vnor_hdr.vert_c * sizeof(das_ObjNormalData));
        break;

    case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
        // Read texture vertex header
        __das_ReadGenVertHdr(&vtex_hdr, (char*) __DAS_TEX_POSITION_HEADER_NAME, li.file_name);
        __das_IncrementOffset(vtex_hdr.vert_c * sizeof(das_ObjTextureData));

        // Read vertex normals header
        __das_ReadGenVertHdr(&vnor_hdr, (char*) __DAS_NORM_POSITION_HEADER_NAME, li.file_name);
        __das_IncrementOffset(vnor_hdr.vert_c * sizeof(das_ObjNormalData));
        break;

    case __DAS_ASSET_MODE_3D_TEXTURE_MAPPED_UNOR:
        // Read texture data
        __das_ReadGenVertHdr(&vtex_hdr, (char*) __DAS_NORM_POSITION_HEADER_NAME, li.file_name);
        __das_IncrementOffset(vtex_hdr.vert_c * sizeof(das_ObjTextureData));
        break;

    default:
        break;
    }

    // Read indices
    __das_ReadINDX_HDR(&indx_hdr, li.file_name);

    cm_FormatTimeFromEpoch (
        DATE_FORMAT_YMD_SP_HYPHEN,
        TIME_FORMAT_24_HOUR_SP_COLON,
        inf_hdr.time_st,
        &date,
        &time
    );

    printf("Magic number: 0x%16lx\n", inf_hdr.magic_number);
    printf("UUID: %s\n", pad_uuid);
    printf("Date and time of creation: %s %s(UTC)\n", date, time);

    switch(inf_hdr.asset_type) {
    case DAS_ASSET_MODE_3D_UNMAPPED:
        printf("Vertex type: unmapped vertices\n");
        printf("Unique position vertices count: %d\n", vpos_hdr.vert_c);
        printf("Unique vertex normals count: %d\n", vnor_hdr.vert_c);
        break;

    case __DAS_ASSET_MODE_3D_UNMAPPED_UNOR:
        printf("Vertex type: unnormalised unmapped vertices\n");
        printf("Unique position vertices count: %d\n", vpos_hdr.vert_c);
        break;

    case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
        printf("Vertex type: textured vertices\n");
        printf("Unique position vertices count: %d\n", vpos_hdr.vert_c);
        printf("Unique texture vertices count: %d\n", vtex_hdr.vert_c);
        printf("Unique vertex normals count: %d\n", vnor_hdr.vert_c);
        break;
    
    case __DAS_ASSET_MODE_3D_TEXTURE_MAPPED_UNOR:
        printf("Vertex type: unnormalised textured vertices\n");
        printf("Unique texture vertices count: %d\n", vtex_hdr.vert_c);
        break;

    default:
        break;
    }

    printf("Total vertices count: %d\n", indx_hdr.ind_c);

    // Print metadata
    size_t meta_c = 0;
    char **meta_data = das_GetMeta(&meta_c);

    if(meta_c && li.meta_enable) {
        printf("Additional metadata: \n");
        for(size_t i = 0; i < meta_c; i++) {
            // Check if newline is present in metadata
            if(meta_data[i][strlen(meta_data[i]) - 1] != '\n')
                printf("%s\n", meta_data[i]);
            else printf("%s", meta_data[i]);
        }
    }
    __das_ReadCleanup();
}


/*
 * Read metadata files
 */
void __dam_ReadMetaData (
    char ***p_out_meta, 
    size_t *p_out_len, 
    char **files, 
    size_t file_c
) {
    *p_out_meta = (char**) calloc(file_c, sizeof(char*));
    *p_out_len = file_c;
    for(size_t i = 0; i < file_c; i++) {
        FILE *file = fopen(files[i], "rb");
        fseek(file, 0, SEEK_END);
        size_t fsize = (size_t) ftell(file);
        fseek(file, 0, SEEK_SET);

        (*p_out_meta)[i] = (char*) calloc(fsize + 1, sizeof(char));
        fread((*p_out_meta)[i], sizeof(char), fsize, file);

        fclose(file);
    }
}


/*
 * Create a valid asset instance with sorted indices
 */
void __dam_AssembleAsset(dam_AssemblyInfo *p_info) {
    char *dst_ext = cm_ExtractFileExtName(p_info->dst_file);
    char *model_ext = cm_ExtractFileExtName(p_info->src_file);
    char ext_file[128] = {0};
    
    // Check if file extension needs to be added
    if(!dst_ext || (dst_ext && strcmp(dst_ext, "das")))
        sprintf(ext_file, "%s.das", p_info->dst_file);
    else
        sprintf(ext_file, "%s", p_info->dst_file);


    das_Asset asset = {0};
    asset.asset_mode = p_info->vert_mode;
    // Check for supported 3D model formats and parse them
    if(model_ext && !strcmp(model_ext, "obj")) {
        size_t entity_c = 0;
        das_WavefrontObjEntity *entities;
        
        // Parse the Wavefront OBJ file into entities
        das_ParseWavefrontOBJ(&entities, &entity_c, p_info->src_file);

        // Prompt the user about the object he would like to use
        das_WavefrontObjEntityWritePrompt(&asset, entities,
            entity_c, p_info->src_file);

        // Perform final cleanup of the entities
        das_WavefrontObjDestroyEntities(entities, entity_c);
    }

    else __DAM_INVALID_MODEL_FORMAT(model_ext);


    // Check if the asset file should contain any meta data
    if(p_info->meta_c) {
        char **meta = NULL;
        size_t meta_c = 0;
        __dam_ReadMetaData(&meta, &meta_c, p_info->meta_files, p_info->meta_c); 
        das_StaticAssemble(&asset, ext_file, meta, meta_c);

        // Perform metadata cleanup
        for(size_t i = 0; i < meta_c; i++)
            free(meta[i]);
        free(meta);
    }

    else das_StaticAssemble(&asset, ext_file, NULL, 0);

    // Free position vertices and indices
    free(asset.vertices.v3d.mul.pos);
    free(asset.indices.pos);

    // Asset mode specific cleanup
    switch(asset.asset_mode) {
    case DAS_ASSET_MODE_3D_UNMAPPED:
        // Free vertex normals
        free(asset.vertices.v3d.mul.norm);
        free(asset.indices.norm);
        break;

    case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
        free(asset.vertices.v3d.mul.tex);
        free(asset.indices.tex);
        free(asset.vertices.v3d.mul.norm);
        free(asset.indices.norm);
        break;

    case __DAS_ASSET_MODE_3D_TEXTURE_MAPPED_UNOR:
        free(asset.vertices.v3d.mul.tex);
        free(asset.indices.tex);
        break;

    default:
        break;
    }
}


/*
 * Parse user required actions
 */
dam_ActionInfo __dam_ParseInput(int argc, char *argv[]) {
    dam_ActionInfo out = { { { 0 } } };
    char *ext = NULL;

    if(argc <= 1) out.type = DAM_ACTION_NONE;
    else if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
        out.type = DAM_ACTION_SHOW_HELP;
    else if(!strcmp(argv[1], "--version")) 
        out.type = DAM_ACTION_SHOW_VERSION;
    // List properties about the asset file
    else if((ext = cm_ExtractFileExtName(argv[1])) && !strcmp(ext, "das")) {
        out.type = DAM_ACTION_LIST_DATA;
        out.act_data.list_info.file_name = argv[1];
        if(argc > 2 && !strcmp(argv[2], "--no-meta"))
            out.act_data.list_info.meta_enable = false;
        else if(argc == 2) 
            out.act_data.list_info.meta_enable = true;
        else __DAM_TOO_MANY_PARAMS;
    }
    // Error invalid file
    else if(!ext)
        __DAM_INVALID_FILE(argv[1]);
    // Assemble new asset file
    else if(argc > 2) {
        // Metadata capacity variable
        size_t meta_cap = 4;

        // Set asset assembly data structs
        out.type = DAM_ACTION_ASSEMBLE;
        out.act_data.asm_info.src_file = argv[1];
        out.act_data.asm_info.dst_file = (char*) "auto.das";
        out.act_data.asm_info.vert_mode = DAS_ASSET_MODE_3D_TEXTURE_MAPPED;
        out.act_data.asm_info.meta_files = (char**) calloc (
            meta_cap,
            sizeof(char*)
        );
        out.act_data.asm_info.meta_c = 0;


        deng_bool_t read_meta = false;
        for(int i = 2; i < argc; i++) {
            deng_bool_t out_spec = false, vert_spec = false;
            // Check for output file specification
            if((out_spec = !strcmp(argv[i], "-o")) && i != argc - 1) {
                read_meta = false;
                i++;
                out.act_data.asm_info.dst_file = argv[i];
            }
            
            // Error no output file specified
            else if(out_spec && i == argc - 1)
                __DAM_EXPECTED_PARAMETER("-o", "output file");

            // Check for vertices mode specification
            else if((vert_spec = !strncmp(argv[i], "-v=", 3)) && strlen(argv[i]) == 6) {
                read_meta = false;
                if(!strcmp(argv[i] + 3, "vuu")) 
                    out.act_data.asm_info.vert_mode = __DAS_ASSET_MODE_3D_UNMAPPED_UNOR;
                else if(!strcmp(argv[i] + 3, "vun"))
                    out.act_data.asm_info.vert_mode = DAS_ASSET_MODE_3D_UNMAPPED;
                else if(!strcmp(argv[i] + 3, "vmu"))
                    out.act_data.asm_info.vert_mode = __DAS_ASSET_MODE_3D_TEXTURE_MAPPED_UNOR;
                else if(!strcmp(argv[i] + 3, "vmn"))
                    out.act_data.asm_info.vert_mode = DAS_ASSET_MODE_3D_TEXTURE_MAPPED;
                else __DAM_INVALID_VERTICES_MODE(argv[i] + 3);
            }

            // Read metadata files
            else if(read_meta) {
                out.act_data.asm_info.meta_c++;

                // Metadata capacity check
                if(out.act_data.asm_info.meta_c >= meta_cap) {
                    meta_cap <<= 1;
                    char **mem = (char**) realloc (
                        out.act_data.asm_info.meta_files,
                        meta_cap * sizeof(char*)
                    );

                    if(!mem) 
                        MEM_ERR("metadata files");
                    out.act_data.asm_info.meta_files = mem;
                }

                out.act_data.asm_info.meta_files[out.act_data.asm_info.meta_c - 1] = argv[i];
            }

            // Start reading metadata
            else if(!strcmp(argv[i], "-m") || !strcmp(argv[i], "--meta"))
                read_meta = true;

            // Error invalid vertices mode specifier by length
            else if(vert_spec && strlen(argv[i]) != 6)
                __DAM_INVALID_VERTICES_MODE("");
        }
    }

    return out;
}



int main(int argc, char *argv[]) {
    // Parse user input into input actions
    dam_ActionInfo act_inf = __dam_ParseInput(argc, argv);

    // Check what to do according to the action info given
    switch(act_inf.type) {
    case DAM_ACTION_NONE:
        TO_VERSION;
        TO_HELP;
        break;

    case DAM_ACTION_SHOW_HELP:
        printf("%s", __help_text);
        break;

    case DAM_ACTION_SHOW_VERSION:
        TO_VERSION;
        break;

    case DAM_ACTION_LIST_DATA:
        __dam_ListAsset(act_inf.act_data.list_info);
        break;

    case DAM_ACTION_ASSEMBLE:
        __dam_AssembleAsset(&act_inf.act_data.asm_info);
        break;

    default:
        break;
    }

    return EXIT_SUCCESS;
}
