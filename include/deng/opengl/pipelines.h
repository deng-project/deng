/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: pipelines.h - OpenGL pipeline manager class header
/// author: Karl-Mihkel Ott


#ifndef __GL_PIPELINES_H
#define __GL_PIPELINES_H


#ifdef __GL_PIPELINES_CPP
    #include <stdlib.h>
    #include <stdio.h>
    #include <string>
    #include <cstring>
    #include <array>
    #include <vector>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <data/assets.h>
	#include <math/deng_math.h>
	#include <deng/window.h>
     
    #include <math/deng_math.h>
    #include <deng/cross_api/shader_def.h>

	#ifdef __DEBUG
		#define db_PipelineMsg(pindex, msg)   __gl_Pipelines::_db_PipelineMsg(pindex, msg)
	#else
		#define db_PipelineMsg(pindex, msg)
	#endif

#endif

namespace deng {
    namespace opengl {

        class __gl_Pipelines {
        private:
            std::array<deng_ugl_t, PIPELINE_C> m_programs;
            void (*lglErrorCheck)(const std::string &func_name, const std::string &file, const deng_ui32_t line);

        private:
            /// Load shader data from file to a buffer
            char *__loadShaderFromFile(const char *file_name);


            /// Check if the shader compilation was successful
            void __checkCompileStatus(const deng_ui32_t shader_id, const char *file_name);


            /// Check if shader program linking was successful
            void __checkLinkingStatus(const deng_ui32_t program_id, const deng_ui32_t program_index);


            /// Compile shader files at given index to shader programs
            void __compileShadersToProgram(const deng_ui32_t index);

        public:
            __gl_Pipelines(void (*gl_error_check)(const std::string &func_name, const std::string &file, const deng_ui32_t line));


            /// Getters
            const deng_ugl_t getShaderProgram(const deng_ui32_t pipeline_id);
            const std::vector<deng_ugl_t> getShaderPrograms();
        };
    }
}


#endif
