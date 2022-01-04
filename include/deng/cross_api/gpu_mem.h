/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: gpu_mem.h - cross api gpu memory offset and usage structure definitions header
/// author: Karl-Mihkel Ott


#ifndef __GPU_MEMPROPS_H
#define __GPU_MEMPROPS_H

#define DENG_DEF_ASSET_CAP      65536   // Initialise with 64KB of memory for assets
#define DENG_DEF_UI_CAP         32768   // Initialise with 32KB of memory for ui elements
#define DENG_DEFAULT_ASSET_CAP  32


#define DENG_DEF_TEX_WIDTH      512
#define DENG_DEF_TEX_HEIGHT     512
#define DENG_DEF_TEX_SIZE       1048576 // bytes
#define DENG_DEF_TEX_MEM_CAP    8388608 // bytes

namespace deng {

    struct BufferSectionInfo {
        // Asset size information
        deng_ui64_t asset_cap = 0;
        deng_ui64_t asset_size = 0;

        // Indices size information
        deng_ui64_t indices_cap = 0;
        deng_ui64_t indices_size = 0;

        // Image data size information
        deng_ui64_t img_cap = 0;
        deng_ui64_t img_size = 0;

        // UI data size information
        deng_ui64_t ui_cap = 0;
        deng_ui64_t ui_size = 0;

        // Uniform buffer data size information
        // NOTE: Data alignment
        // Data in uniform buffers is aligned as follows:
        //  * Uniform data for different pipelines
        //      Data is stored as follows:
        //          - 3D uniform object transformation
        //          - 2D object transformation
        //          - Light source data
        //  * Asset data: 
        //      Each asset contains various coloring information about that certain asset
        //      NOTE: Asset color data structure is different for 3D and 2D assets
        deng_ui64_t ubo_cap = 0;
        deng_ui64_t ubo_non_asset_size = 0;
        deng_ui64_t ubo_asset_size = 0;
        deng_ui64_t ubo_asset_cap = 0;
    };
}

#endif
