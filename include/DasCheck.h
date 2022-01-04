// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: DasCheck.h - Deprecated DAS file checking header (C) 
// author: Karl-Mihkel Ott

#ifndef DAS_CHECK_H
#define DAS_CHECK_H


#ifdef DAS_CHECK_C
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>

    #include <common/base_types.h>
    #include <common/cerr_def.h>

    #include <data/das_file.h>
#endif


/*
 * Enumeral of all possible DAS format error codes
 */
typedef enum das_Error {
    DAS_ERROR_SUCCESS                           = 0,
    DAS_ERROR_FILE_CORRUPTION                   = 1,
    DAS_ERROR_INVALID_MAGIC_NUMBER              = 2,
    DAS_ERROR_SIZE_OUT_OF_BOUNDS                = 3,
    DAS_ERROR_INVALID_UUID                      = 4,
    DAS_ERROR_INVALID_REQUESTED_VERTEX_FORMAT   = 5,
    DAS_ERROR_INVALID_VERTICES_COUNT            = 6,
    DAS_ERROR_INVALID_INDICES_COUNT             = 7,
    DAS_ERROR_INVALID_FRAME_COUNT               = 8,
    DAS_ERROR_INVALID_ANIMATION_FRAME_COUNT     = 9
} das_Error;


// Read error definition macros
#define __DAS_READ_ERROR(file)                  fprintf(stderr, "Failed to read from file %s, possible file corruption\n", file), \
                                                exit(DAS_ERROR_FILE_CORRUPTION)

#define __DAS_READ_INVALID_VERT_FORMAT(file)    fprintf(stderr, "Invalid requested asset vertices mode in file: %s\n", file), \
                                                exit(DAS_ERROR_FILE_CORRUPTION)

#define __DAS_READ_INVALID_MAGIC_NUM(file)      fprintf(stderr, "Invalid magic number for DAS format in file %s\n", file), \
                                                exit(DAS_ERROR_INVALID_MAGIC_NUMBER)

#define __DAS_READ_CORRUPT_ERROR(file)          fprintf(stderr, "Corrupt DENG asset file: %s\n", file), \
                                                exit(DAS_ERROR_FILE_CORRUPTION)

// Write error definition macros


// Vertices and indices count check macros
#define das_VertCountCheck(vc) ( vc <= 0 ? DAS_ERROR_INVALID_VERTICES_COUNT : DAS_ERROR_SUCCESS )
#define das_IndicesCountCheck(ic) ( ic <= 0 ? DAS_ERROR_INVALID_INDICES_COUNT : DAS_ERROR_SUCCESS )
#define das_FrameCountCheck(fc) ( fc <= 0 ? DAS_ERROR_INVALID_ANIMATION_FRAME_COUNT : DAS_ERROR_SUCCESS )


/// Check if the requested memory size is available for reading otherwise
/// throw a runtime error
void das_ErrBufferReadCheck(size_t req_size, size_t max_size, const char *file_name);


/// Check if the given header specifier is valid
void das_CheckHdrName(char *name, char *exp_name, const char *file_name);


/// Check if provided uuid is valid
das_Error das_UuidCheck(char *uuid);


/// Check if magic number provided is valid or not
das_Error das_MagicNumberCheck(deng_ui64_t num);

#endif
