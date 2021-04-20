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


#ifndef __CERR_H
#define __CERR_H

/*
 * This file is pretty much the same as err_def.h except that this file
 * uses C standard library functions instead of C++ one, also graphics 
 * api specific error macros are kept out since renderer implementation
 * is done in C++
 */


// Logging
#define LOG(msg, ...)           printf("LOG: %s\n", __VA_ARGS__)
#define WARNME(msg, ...)        printf("WARNING: %s\n", __VA_ARGS__)

// Wavefront OBJ parsing errors
#ifdef __WAVEFRONT_OBJ_C
    #define __WV_PARSE_ERROR(line, msg)         fprintf(stderr, "Wavefront OBJ file parsing error on line %ld, %s\n", line, msg), \
                                                exit(EXIT_FAILURE)
#endif

#ifdef __DAS_LOADER_C
    #define __DAS_CORRUPT_ERROR(file)               fprintf(stderr, "Corrupt DENG asset file: %s\n", file), \
                                                    exit(EXIT_FAILURE)

    #define __DAS_INVALID_ASSET_FORMAT(file)        fprintf(stderr, "Invalid requested asset file vertices mode in file: %s\n", file), \
                                                    exit(EXIT_FAILURE)
#endif

#ifdef __DAM_C
    #define TO_HELP printf("Use --help for more information\n")
    #define __DAM_INVALID_MODEL_FORMAT(ext)         fprintf(stderr, "Invalid 3D model file format %s\n", ext), \
                                                    exit(EXIT_FAILURE)
    #define __DAM_EXPECTED_PARAMETER(flag, param)   fprintf(stderr, "DAM usage error\n%s expects %s parameter\n", flag, param), \
                                                    TO_HELP, \
                                                    exit(EXIT_FAILURE)

    #define __DAM_INVALID_PARAMETER(param, cparam)  fprintf(stderr, "Invalid parameter %s, correct would be %s\n", param, cparam), \
                                                    TO_HELP, \
                                                    exit(EXIT_FAILURE)
#endif

// Runtime errors
#define MEM_ERR(purpose)        fprintf(stderr, "Failed to allocate memory: %s\n", purpose), \
                                exit(EXIT_FAILURE)

#define FILE_ERR(file)          fprintf(stderr, "Failed to read file: %s\n", file), \
                                exit(EXIT_FAILURE)

#define DIR_ERR(dir)            fprintf(stderr, "Failed to open directory: %s\n", dir), \
                                exit(EXIT_FAILURE)

#define MATH_ERR(msg)           fprintf(stderr, "Math error: %s\n", x), \
                                exit(EXIT_FAILURE)

#define FONT_ERR(msg)           fprintf(stderr, "Font rasterisation error: %s\n", x), \
                                exit(EXIT_FAILURE)

#define RUN_ERR(func, msg)      fprintf(stderr, "%s: %s\n", func, msg), \
                                exit(EXIT_FAILURE)
#endif
