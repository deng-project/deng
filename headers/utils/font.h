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


#ifndef FONT_H
#define FONT_H

#ifdef __FONT_CPP
    #include <vector>
    #include <array>
    #include <string>
    #include <stdlib.h>
    #ifdef __linux__
        #include <dirent.h>
    #endif
    #ifdef _WIN32
        #include <windows.h>
    #endif
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/common.h>
    #include <common/err_def.h>
    #include <das/assets.h>
    
    #include <math/deng_math.h>
    #include <deng/surface_window.h>
    #include <deng/window.h>
    #include <utils/shape_gen.h>
#endif

#include <ft2build.h>
#include <freetype/ftbitmap.h>

#ifdef __linux__
    #define DEFAULT_FONT_PATH "/usr/share/fonts"
#endif

#ifdef _WIN32
    #define DEFAULT_FONT_PATH "C:\\Windows\\Fonts"
#endif

namespace dengUtils {
    struct BitmapChar {
        char ascii_ch;
        deng_ui16_t glyph_id; // ID of glyph texture
    };

    struct BitmapGlyphData {
        dengMath::vec2<deng_i32_t> bearings;
        dengMath::vec2<deng_i32_t> advance;
        FT_Bitmap bitmap;
    };
    

    /*
     * Store information about rasterised text
     */
    struct BitmapStr {
        const char *text;
        const char *font_file;
        const char *uuid;

        std::vector<BitmapGlyphData> unique_glyphs;
        std::vector<deng_ui8_t> tex_data;
        std::array<VERT_MAPPED_2D, 4> vert_pos;
        std::array<deng_ui32_t, 6> vert_indices;
        dengMath::vec2<deng_i32_t> box_size;

        BitmapChar *rend_text = NULL;
        FT_Face font_face;

        void operator=(const char* input) { text = input; }
    };


    // Class for handling font rendering
    class StringRasterizer {
    private:
        FT_Library m_library_instance;
        deng::Window *m_p_win;
        std::vector<std::string> m_fonts;

    protected:
        deng_bool_t __verifyFont (
            BitmapStr &str, 
            std::string &out_path
        );

        deng_px_t __findTextSizePx(BitmapStr &str);
        deng_vec_t __findTextSizeVec(BitmapStr &str);
        std::vector<char> __indexGlyphs(BitmapStr &str);
        void __findFontFiles(std::string custom_path);
        
        // Generic text box creationg function
        void __mkGlyphs (
            BitmapStr &str, 
            deng_px_t px_size,
            dengMath::vec2<deng_vec_t> pos, 
            dengMath::vec3<unsigned char> color
        );

        void __mkTextbox (
            BitmapStr &str, 
            deng_px_t text_width,
            deng_ui32_t hier_level,
            dengMath::vec3<unsigned char> color,
            dengMath::vec2<deng_vec_t> pos,
            dengMath::vec2<deng_vec_t> origin
        );

    public:
        // If custom font path is not needed then just pass nullptr
        StringRasterizer (
            std::string custom_font_path, 
            deng::Window *p_window_wrap
        );
        ~StringRasterizer();
        
        /* Create new text box instance from pixel size */
        void newPxStr ( 
            BitmapStr &str,
            const char *font_name,
            deng_px_t px_size,
            dengMath::vec3<unsigned char> color,
            dengMath::vec2<deng_vec_t> pos,
            deng_ui32_t hier_level,
            dengMath::vec2<deng_vec_t> origin
        );
        
        /* Create new text box instance from vector size */
        void newVecStr (
            BitmapStr &str,
            const char *font_name,
            deng_vec_t vec_size,
            dengMath::vec3<unsigned char> color,
            dengMath::vec2<deng_vec_t> pos,
            deng_ui32_t hier_level,
            dengMath::vec2<deng_vec_t> origin
        );

        /* Check for the width of rasterised textbox and reduce string length if needed */
        char *strRasterWidthTrim (
            const char *str,
            const char *font_name,
            deng_vec_t vec_height,
            deng_vec_t max_vec_width,
            dengMath::vec2<deng_ui32_t> deng_window_size,
            deng_vec_t *p_out_width
        );

        /* Get the maximum height of text from specified maximum width */
        deng_vec_t getMaxHeight (
            const char *str,
            const char *font_name,
            deng_vec_t max_width
        );


        /* Rasterise absolutely positioned label for ui elements */
        BitmapStr renderAbsLabel (
            deng_px_t px_padding,
            dengMath::vec2<deng_vec_t> parent_elem_top_left,
            dengMath::vec2<deng_vec_t> top_left_margin,
            dengMath::vec2<deng_vec_t> size,
            dengMath::vec2<deng_vec_t> origin,
            dengMath::vec3<unsigned char> color,
            deng_ui32_t hier_level,
            const char *font_file,
            const char *label
        );


        /* Rasterise relatively positioned label for ui elements */
        BitmapStr renderRelLabel (
            dengMath::vec2<deng_vec_t> parent_cont_size,
            deng_px_t px_padding,
            dengMath::vec2<deng_vec_t> parent_elem_top_left,
            dengMath::vec2<deng_vec_t> top_left_pos,
            dengMath::vec2<deng_vec_t> size,
            dengMath::vec2<deng_vec_t> origin,
            dengMath::vec3<unsigned char> color,
            deng_ui32_t hier_level,
            const char *font_file,
            const char *label
        );


        /* Clean rasterised string */
        void cleanBitmapString(BitmapStr &str);
    };
}

#endif
