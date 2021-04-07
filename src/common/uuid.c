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


#include <stdint.h>
#define __UUID_C
#include <common/uuid.h>


/*
 * Find first non lo network device's mac address
 */
char *__uuid_GetDevMacAddress() {
    static char out[13] = {0};
    char buf[18] = {0};

    #if defined(__linux__)
        // Read the mac address of the first network device that isn't lo
        DIR *dir;
        FILE *file;
        struct dirent *c;
         
        dir = opendir(__LINUX_NET_DEV_DIR);
        if(!dir) DIR_ERR(__LINUX_NET_DEV_DIR);
        while((c = readdir(dir))) {
            if (
                c->d_type == DT_LNK && 
                strcmp(c->d_name, ".") && 
                strcmp(c->d_name, "..") && 
                strcmp(c->d_name, "lo")
            ) {
                char addr[128] = {0};
                sprintf (
                    addr,
                    "%s%s/address",
                    __LINUX_NET_DEV_DIR,
                    c->d_name
                );

                file = fopen(addr, "rb");
                if(!file) FILE_ERR(addr);

                fread(buf, sizeof(char), 17, file);
                fclose(file);
                break;
            }
        }

        closedir(dir);

        if(buf[0] == 0x00)
            perror("No mac address found!\n");
    #elif defined(_WIN32)
    #endif

    // Parse MAC address into byte array
    char *ptr = buf;
    char *end = buf + 18;
    char *br = buf;
    size_t offset = 0;
    while(ptr < end && offset < 12) {
        br = strchr(ptr, ':') ? strchr(ptr, ':') : end;
        strncpy (
            out + offset,
            ptr,
            br - ptr
        );

        offset += 2;
        ptr = br + 1;
    }

    return out;
}   


/*
 * Init new uuid instance
 */
char *uuid_Generate() {
    time_t time_val = time(NULL);
    srand(time_val);

    // Pseudo randomise the time value
    deng_ui32_t tval[2];
    tval[0] = rand() % UINT32_MAX;
    tval[1] = rand() % UINT32_MAX;

    deng_ui8_t *time_s = (deng_ui8_t*) tval;
    
    // Copy lower 4 bytes of the time to uuid instance
    memcpy(__uuid.bytes, time_s + 4, 4);

    // Copy medium 2 bytes of the time to uuid instance
    memcpy(__uuid.bytes + 4, time_s + 2, 2);

    // Copy highest 2 bytes of the time to uuid instance
    memcpy(__uuid.bytes + 6, time_s, 2);

    // Shift bits to right and add UUID version specier bits
    __uuid.bytes[6] >>= 4;
    __uuid.bytes[7] >>= 4;
    __uuid.bytes[6] |= __UUID_VERSION;

    #if defined(__linux__)
        // Read random clock sequence from kernel random number generator
        FILE *file;
        file = fopen("/dev/urandom", "rb");
        if(!file) FILE_ERR("/dev/urandom");

        fread(__clock_seq, 2, sizeof(deng_ui8_t), file);
        fclose(file);
    #elif defined(_WIN32)
    #endif

    // Copy clock sequence to uuid instance 
    memcpy(__uuid.bytes + 8, __clock_seq, 2);
    __uuid.bytes[8] >>= __UUID_VARIANT_C;
    __uuid.bytes[9] >>= __UUID_VARIANT_C;
    __uuid.bytes[8] |= __UUID_VARIANT;
    
    char *mac = __uuid_GetDevMacAddress();
    char *out = calloc(33, sizeof(char));

    sprintf (
        out,
        "%08x-%04x-%04x-%04x-%s",
        *(deng_ui32_t*) (__uuid.bytes),
        *(deng_ui16_t*) (__uuid.bytes + 4),
        *(deng_ui16_t*) (__uuid.bytes + 6),
        *(deng_ui16_t*) (__uuid.bytes + 8),
        mac
    );

    return out;
}
