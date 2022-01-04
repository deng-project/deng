// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: UUID.h - Deprecated UUID generation header (C)
// author: Karl-Mihkel Ott

#ifndef __UUID_H
#define __UUID_H

#ifdef __cplusplus
extern "C" {
#endif

    #ifdef __UUID_C
        #include <stdlib.h>
        #include <stdint.h>
        #include <stdio.h>
        #include <time.h>
        #include <string.h>
        #include <limits.h>

        #ifdef __linux__
            #include <dirent.h>
            #include <linux/random.h>
		#elif _WIN32
			#include <Windows.h>
			#include <iphlpapi.h>
        #endif


        #include <common/base_types.h>  
		#include <common/common.h>
        #include <common/cerr_def.h>
        
        #define __UUID_VERSION 0x10
        #define __UUID_VARIANT 0x40
        #define __UUID_VARIANT_C    2
        #define __LINUX_NET_DEV_DIR     "/sys/class/net"
        
        /*
         * Basic structure for 16 byte UUID storage byte per byte
         */
        typedef struct UUIDType {
            deng_ui8_t bytes[10];
        } UUIDType;

        UUIDType __uuid = {0};
        deng_ui8_t __clock_seq[2];


        /*
         * Find first non lo network device's mac address
         */
        char *__uuid_GetDevMacAddress();
    #endif

    /*
     * Init new uuid instance
     */
    char *uuid_Generate();


#ifdef __cplusplus
}
#endif
#endif
