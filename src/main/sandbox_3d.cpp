#define DENG_ENABLE_STD_INCL
#include <deng/deng.h>
#define SANDBOX_BACKGROUND {0.1411765f, 0.0431372f, 0.2313725f, 1.0f}

int main() {
    deng::WindowWrap window(1280, 980, "Game!");

    deng::EditorCamera ed_cam (
        0.2f,
        {0.0f, 0.0f, 0.0f}, 
        {0.7f, 0.7f},
        dengMath::Conversion::degToRad(65.0f), 
        DENG_DEFAULT_NEAR_PLANE, 
        DENG_DEFAULT_FAR_PLANE, 
        &window
    );

    //deng::FPPCamera fpp_cam (
    //    {1.0f, 1.0f, 1.0f}, 
    //    {0.7f, 0.7f}, 
    //    dengMath::Conversion::degToRad(65.0f), 
    //    DENG_DEFAULT_NEAR_PLANE, 
    //    DENG_DEFAULT_FAR_PLANE, 
    //    NULL, 
    //    NULL,
    //    &window
    //);

    deng::Renderer rend (
        &window, 
        &ed_cam,
        DENG_CAMERA_EDITOR
    );

    rend.setHints (
        DENG_RENDERER_HINT_SHOW_CMD_FPS_COUNTER |
        DENG_RENDERER_HINT_MSAA_4 |
        DENG_RENDERER_HINT_ENABLE_VALIDATION_LAYERS |
        DENG_RENDERER_HINT_MIPMAP_ENABLE
    );
    
    std::array<deng_Asset, 2> assets;
    dengMath::ModelMatrix viking_mat;
    viking_mat.setRotation(0, 0.0f, 0.0f);
    viking_mat.setScale(1.0f, 1.0f, 1.0f);

    // Load some example
    #ifdef _WIN32
        dasReadAsset (
            &assets[0], 
            "..\\viking_unor.das", 
            DENG_ASSET_MODE_3D_TEXTURE_MAPPED
        );
    #endif
    #ifdef __linux__
        dasReadAsset (
            &assets[0],
            "../viking_unor.das",
            DENG_ASSET_MODE_3D_TEXTURE_MAPPED
        )
    #endif

    assets[0].tex_id = (char*) "viking";
    assets[0].id = (char*) "viking_room";
    assets[0].is_shown = true;
    dengMath::mat4<deng_vec_t> mmat = viking_mat.getModelMatrix();
    dengMath::Transformer::apply3DModelMatrix (
        assets[0], 
        mmat
    );

    deng_Texture viking_tex;
    #ifdef _WIN32
        dasLoadTexture (
            &viking_tex,
            "..\\viking_room.tga"
        );
    #endif
    #ifdef __linux__  
        dasLoadTexture (
            &viking_tex,
            "../viking_room.tga"
        );
    #endif
    viking_tex.id = (char*) "viking";
    
    dengUtils::GridGenerator grid_gen (
        DENG_DEFAULT_GRID_COLOR, 
        50.0f, 
        150
    );

    assets[1] = grid_gen.getGrid();
    assets[1].is_shown = true;

    rend.submitAssets(assets.data(), assets.size());
    rend.submitTextures(&viking_tex, 1);

    rend.initRenderer (
        DENG_RENDERER_USAGE_MAP_EDITOR, 
        SANDBOX_BACKGROUND
    );

    rend.run();
    return EXIT_SUCCESS;
}
