// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ImageHeaders.h - Image headers for BMP and TGA files (deprecated)
// author: Karl-Mihkel Ott

#ifndef __IMAGE_HEADERS_H
#define __IMAGE_HEADERS_H


/*******************************************/
/********** BMP Image Info Headers *********/
/*******************************************/
#define __BMPFileHeader_Size 14
typedef struct __BMPFileHeader {
    deng_ui16_t file_type;
    deng_ui32_t file_size;
    deng_ui16_t reserved1;
    deng_ui16_t reserved2;
    deng_ui32_t offset_data;
} __BMPFileHeader;


#define __BMPInfoHeader_Size    40
typedef struct __BMPInfoHeader {
    deng_ui32_t size;
    deng_i32_t width;
    deng_i32_t height;

    deng_ui16_t planes;
    deng_ui16_t bit_count;
    deng_ui32_t compression;
    deng_ui32_t size_image;
    deng_i32_t x_pixels_per_meter;
    deng_i32_t y_pixels_per_meter; 
    deng_ui32_t colors_used;
    deng_ui32_t colors_important;
} __BMPInfoHeader;


#define __BMPColorHeader_Size   24
typedef struct __BMPColorHeader {
    deng_ui32_t red_mask;
    deng_ui32_t green_mask;
    deng_ui32_t blue_mask;
    deng_ui32_t alpha_mask;
    deng_ui32_t color_space_type;
    deng_ui32_t unused[16];
} __BMPColorHeader;


/*******************************************/
/********** BMP Image Info Headers *********/
/*******************************************/
#define __TGATypeHeader_Size    3
typedef struct __TGATypeHeader {
    deng_ui8_t id_length;
    deng_ui8_t colormap_type;
    deng_ui8_t image_type;
} __TGATypeHeader;


#define __TGAColorMapHeader_Size    5
typedef struct __TGAColorMapHeader {
    deng_ui16_t first_colormap_index;
    deng_ui16_t colormap_length;
    deng_ui8_t entity_size;
} __TGAColorMapHeader;


#define __TGAInfoHeader_Size    10
typedef struct __TGAInfoHeader {
    deng_ui16_t x_origin;
    deng_ui16_t y_origin;
    deng_ui16_t width;
    deng_ui16_t height;
    deng_ui8_t bit_count;
    deng_ui8_t image_descriptor;
} __TGAInfoHeader;

#endif
