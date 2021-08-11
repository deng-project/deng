/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: dam.h - deng asset manager header file
/// author: Karl-Mihkel Ott


#ifndef __DAM_H
#define __DAM_H


/// DAM has different versioning than DENG
#define __DAM_VERSION "0.1.1"

#ifdef __DAM_C
    #define __DAM_PREVIEW_TITLE     "DAM"
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    
    #include <common/cerr_def.h>
    #include <common/base_types.h>
    #include <common/common.h>

    #include <data/assets.h>
    #include <data/das_file.h>
    #include <data/das_loader.h>
    #include <data/das_static_assembler.h>
    #include <data/wavefront_obj.h>
#endif


#ifdef __DAM_C
    #define __DAM_DEFAULT_PREVIEW_WIDTH     1280
    #define __DAM_DEFAULT_PREVIEW_HEIGHT    720
    #define __DAM_DEFAULT_ASSET_COLOR       {0.7f, 0.7f, 0.7f, 1.0f}

    // Error definition macros
    #define TO_HELP                                 printf("Use --help for more information\n")
    #define TO_VERSION                              printf("DENG asset manager %s\n", __DAM_VERSION)

    #define __DAM_INVALID_MODEL_FORMAT(ext)         fprintf(stderr, "Invalid 3D model file format %s\n", ext), \
                                                    exit(EXIT_FAILURE)
    #define __DAM_EXPECTED_PARAMETER(flag, param)   fprintf(stderr, "DAM usage error\n%s expects %s parameter\n", flag, param), \
                                                    TO_HELP, \
                                                    exit(EXIT_FAILURE)

    #define __DAM_INVALID_PARAMETER(param, cparam)  fprintf(stderr, "Invalid parameter %s, correct would be %s\n", param, cparam), \
                                                    TO_HELP, \
                                                    exit(EXIT_FAILURE)

    #define __DAM_INVALID_VERTICES_MODE(smode)      fprintf(stderr, "Invalid vertices mode %s\n", smode), \
                                                    exit(EXIT_FAILURE)

    #define __DAM_TOO_MANY_PARAMS                   fprintf(stderr, "Too many parameters\n"), \
                                                    exit(EXIT_FAILURE)
    
    #define __DAM_INVALID_FILE(file)                fprintf(stderr, "Invalid file '%s'\n", file), \
                                                    exit(EXIT_FAILURE)


    
    /*
     * List of all possible actions that can be done with dam
     */
    typedef enum dam_ActionList {
        DAM_ACTION_NONE             = 0,
        DAM_ACTION_SHOW_VERSION     = 1,
        DAM_ACTION_SHOW_HELP        = 2,
        DAM_ACTION_LIST_DATA        = 3,
        DAM_ACTION_ASSEMBLE         = 4
    } dam_ActionList;
    

    /*
     * Structure for containing all info about assembling a new
     * asset
     */
    typedef struct dam_AssemblyInfo {
        char *src_file;
        char *dst_file;
        das_AssetMode vert_mode;
        char **meta_files;
        deng_ui32_t meta_c;
    } dam_AssemblyInfo;


    /*
     * Structure for containing all information about 
     * asset information listing
     */
    typedef struct dam_ListInfo {
        char *file_name;
        deng_bool_t meta_enable;
    } dam_ListInfo;
    
    
    /*
     * Union for containing action specific data
     */
    typedef union dam_ActionData {
        dam_AssemblyInfo asm_info;
        dam_ListInfo list_info;
    } dam_ActionData;


    /*
     * Main structure for containing the information about required action
     */
    typedef struct dam_ActionInfo {
        dam_ActionData act_data;
        dam_ActionList type;
    } dam_ActionInfo;


    const char *__help_text = 
        "Basic usage of DAM: dam [INPUT] [OPTIONS]\n" \
        "[ASSET_FILE] -- display information about the asset\n" \
        "[ASSET_FILE] --no-meta -- display information about asset without any metadata\n" \
        "--version -- display the current version of dam\n" \
        "-h / --help -- view help text\n" \
        //"-p / --preview [ASSET] -- create preview for 3D asset\n" \
        //"   -g / --geometry [WIDTH]x[HEIGHT] -- specify the preview window size\n" \
        //"   --disable-menus -- disable all menus in previews\n" 
        "[INPUT] [OPTIONS] -o [OUTPUT] -- output file name\n" \
        "[INPUT] [OPTIONS] -v= / --vert=[vuu, vun, vmu, vmn] -- specify the new asset vertices mode\n" \
        "[INPUT] [OPTIONS] -m / --meta [FILES] -- read metadata from files\n";

    
    /*
     * Read metadata files
     */
    static void __dam_ReadMetaData (
        char ***p_out_meta, 
        size_t *p_out_len, 
        char **files, 
        size_t file_c
    );


    /*
     * Read information about the asset
     * Information is following:
     *  - Name of the asset
     *  - Date and time of the creation
     *  - Type of vertices
     *  - Total count of unique vertices
     *  - Total count of drawn vertices
     */
    static void __dam_ListAsset(dam_ListInfo li);


    /*
     * Assemble deng asset from specified asset 
     * assembly flags
     */
    static void __dam_AssembleAsset(dam_AssemblyInfo *p_info);

    
    /*
     * Parse all user input data
     */
    static dam_ActionInfo __dam_ParseInput(int argc, char *argv[]);
#endif

#endif

