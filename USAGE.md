# Getting started with DENG


## Setting up a new project with DENG

After successfully building DENG you can find either `libdeng.so` or `libdeng.lib` (depending on your
target system) in the final build directory. You should copy the build library to your project directory
alongside with `shaders` and `headers` directories from the root of the DENG repository.  
After the copying step is done you should also make sure that the include and library linking directories
are set correctly according to your build environment / IDE of your choice.


## Compiling asset(s) and making sure that textures are in correct format

**NOTE: This section describes the usage of DENG asset manager. This step can be generally skipped if you prefer to
use included sample assets and textures in the root repository's assets and texture directories correspondingly**

Along with the actual runtime library, you should have also `dam` executable built, during the building process. DAM stands for
DENG asset manager and is used as a CLI compiler for compiling 3D model data into binary DENG readable format, specfied with
.das file format. Generating .das files is quite easy, assuming that the input model is triangulated and the compilation can be 
done with following command:  
`dam model.obj -o model.das`
**PS! Currently it is possible to only use Wavefront OBJ files for dam input.**

As long as textures are consired you can currently use only 32bit or 24bit uncompressed tga or bmp files.


## Writing your first program using DENG 

**NOTE: This tutorial focuses on rendering 3D models on screen with appropriate camera systems. Rendering
2D objects on screen is theoretically supported but requires more complex steps to be taken to be achieved**

First of all include all necessary headers for DENG components, this can be done by including deng.h metaheader
that already has includes for all DENG components and third party features.  
```C++
#include <deng/deng.h>
using namespace dengMath;
```

Generally it is not advised to use `using namespace` in your C++ code for for numerous reasons, but for the sake
of simplicity we are going to include `dengMath` namespace in our example code. The namespace `dengMath` contains
multiple vector and matrix classes, specific camera matrix helper class and conversion namespace that contains
multiple simple conversion methods.

**NOTE: deng.h metaheader contains multiple STL includes. Those includes can be disabled by defining
DENG\_NO\_STL\_INCLUDES before including deng.h, however keep in mind that it is necessary to have std::vector,
std::array, std::queue, std::shared\_ptr, std::unique\_ptr and namespace std::chrono declared for the
program to compile properly**


### Setting up the window API and creating new scope for rendering

Before actually proceeding to Window and Camera instances creation, window backend API must be initialised beforehand
for this we can use `deng_InitWindowAPI` macro. As for deinitialising `deng_DeinitWindowAPI` macro is used. However keep
in mind that the renderer class must be destructed before deinitialising window API, otherwise invalid backend API calls 
cause segmentation fault. For this example lets create a specific scope just for the DENG code then initialise and deinitialise
window API before and after the scope.

```C++
deng_InitWindowAPI();

{
    // DENG renderer code here
}

dengDeinitWindowAPI();
```

### Create new window and camera instances

In order to start using DENG renderer, one must firstly create Window and Camera3D instances. Lets start in
our example program by firstly initialising the Window class.
```C++
deng::Window win = deng::Window(1280, 720, DENG_WINDOW_HINT_API_VULKAN | DENG_WINDOW_HINT_FIXED_SIZE, "DENG example");
```

The constructor for Window class takes 4 parameters.
First and second parameters specify the window dimentions. Third parameter specifies window generation hints, 
that are compatible with [nekowin](https://github.com/inugami-dev64/nekowin) window hints.  
Window hints are following:  
* DENG\_WINDOW\_HINT\_API\_OPENGL - generate window specifically for OpenGL context  
* DENG\_WINDOW\_HINT\_API\_VULKAN - generate window specifically for Vulkan surface  
* DENG\_WINDOW\_HINT\_FULL\_SCREEN - generate window in fullscreen mode (currently only works in OpenGL properly)  
* DENG\_WINDOW\_HINT\_FIXED\_SIZE - generate window in fixed size  
* DENG\_WINDOW\_HINT\_RESIZEABLE - generate window to be resizeable (currently only works in OpenGL properly)  
* Fourth parameter specifies the title and the name of the window  

Window hints can be combined as long as you mix only a single api related hint with only a single sizing
related hint.


```C++
deng::Camera3D cam = deng::Camera3D(DENG_CAMERA_TYPE_FPP, static_cast<deng_vec_t>(conversion::degToRad(65.0)), vec2<deng_vec_t>(-0.1f, -25.0f), 
                                    vec3<deng_vec_t>(0.7f, 0.7f, 0.7f), vec2<deng_vec_t>(0.3f, 0.3f), &win);
```

The constructor for the class `Camera3D` takes 6 parameters. First parameter specifies the camera type, which
in the current case is FPP or first person perspective camera type. Second parameter specifies field of view angle in radians
which in our case is 65 degrees or roughly about 1.134 radians. We use the included `conversion::degToRad` function 
to calculate more precise estimation of the wanted field of view angle in radians. Third parameter is a `dengMath::vec2`
instance that has its first element as a near plane and the second element as a far plane.  

**PS! DENG attempts to use right hand coordinate system by default, so the Z coordinate in front of camera view
is always negative and thus the near and far plane instances are negative**  

Fourth and fifth parameters specify camera control sensitivities. The fourth parameter's sensitivity is specific for 
keyboard controls in x, y, z axes and the fifth parameter's sensitivity is specific for mouse controls in x, y axes.  
Sixth parameter is just a valid pointer to `deng::Window` class instance.

Now that we have camera class created we can proceed to creating control binding for the camera system.


### Creating camera bindings

When designing a game's camera system, one must keep in mind to design camera's control bindings. Usually in most first person games the 
camera can be rotated around its own coordinate axes using the mouse and the camera (or in the game example the player) can be moved
relative to camera's coordinate system with either WASD keys, with arrow keys or with HJKL keys if you are a hardcore vim fan. In 
any case assigning camera control keybindings is all up to the developer to choose.  

DENG offers very universal way to assinging input keybindings to its camera systems. In case of 3D camera bindings
`deng::Camera3DBindings` structure can be used. The bindings of `deng::Camera3DBindings` are following:  
* mov\_u - move camera towards the u coordinate  
* mov\_nu - move camera towards the negative u coordinate  
* mov\_v - move camera towards the v coordinate  
* mov\_nv - move camera towards the negative v coordinate  
* mov\_w - move camera towards the w coordinate  
* mov\_nw - move camera towards the negative w coordinate  

* mov\_x - move camera towards the x coordinate  
* mov\_nx - move camera towards the negative x coordinate  
* mov\_y - move camera towards the v coordinate  
* mov\_ny - move camera towards the negative v coordinate  
* mov\_z - move camera towards the w coordinate  
* mov\_nz - move camera towards the negative w coordinate  

* rot\_u - rotate camera around the u axis
* rot\_nu - rotate camera around the negative u axis  
* rot\_v - rotate camera around the v axis  
* rot\_nv - rotate camera around the negative v axis  
* rot\_w - rotate camera around the w axis  
* rot\_nw - rotate camera around the negative w axis  

* rot\_x - rotate camera around the x axis
* rot\_nx - rotate camera around the negative x axis  
* rot\_y - rotate camera around the y axis  
* rot\_ny - rotate camera around the negative y axis  
* rot\_z - rotate camera around the z axis  
* rot\_nz - rotate camera around the negative z axis  

* ch\_vcp - toggle virtual cursor mode

**NOTE: xyz coordinates represent global world coordinates, while uvw coordinates are relative to the camera itself**

The last binding is for toggling virtual cursor mode. Virtual cursor mode in DENG is basically just explicitly 
setting cursor position to the center of the window by the window API and calculating delta movement from the 
previous frame in the current frame. Delta mouse movement calculations allow the camera system to perform rotation 
calculations and ultimately calculate the final view, projection matrix calculations for the shader.  

Since this tutorial is focused on creating first person camera controls the bindings can be set up as follows:  
```C++
deng::Camera3DBindings bindings;
bindings.mov_u = deng_CreateInputMask(1, NEKO_KEY_D);
bindings.mov_nu = deng_CreateInputMask(1, NEKO_KEY_A);
bindings.mov_v = deng_CreateInputMask(1, NEKO_KEY_SPACE);
bindings.mov_nv = deng_CreateInputMask(1, NEKO_KEY_LEFT_CTRL);
bindings.mov_w = deng_CreateInputMask(1, NEKO_KEY_S);
bindings.mov_nw = deng_CreateInputMask(1, NEKO_KEY_W);

bindings.rot_u = deng_CreateInputMask(1, NEKO_MOUSE_DELTA_NY);
bindings.rot_nu = deng_CreateInputMask(1, NEKO_MOUSE_DELTA_Y);
bindings.rot_v = deng_CreateInputMask(1, NEKO_MOUSE_DELTA_NX);
bindings.rot_nv = deng_CreateInputMask(1, NEKO_MOUSE_DELTA_X);
bindings.ch_vcp = deng_CreateInputMask(1, NEKO_KEY_ESCAPE);

cam.setBindings(bindings);
```

`deng_CreateInputMask()` function is used to to create a mask of keybindings for certain camera action. The function
takes an integer number as a parameter that specifies the number of keybindings in generated keymask and the following n 
parameters must be specified as [nekowin's neko\_InputEv members](https://github.com/inugami-dev64/nekowin/blob/master/include/key_translation.h).  

Finally when the keybindings are defined, the `deng::Camera3D::setBindings()` method is used to submit bindings data into
the camera system.  


### Loading textures and assets

In DENG there are universal texture and asset structures, which store information about vertices, indices, pixels etc.
The structures are `das_Asset` and `das_Texture`. In the following example snippet we are going to load a texture and an asset
information from their files.  

```C++
das_Texture = {};
das_LoadTexture(&texture, "example_tex.bmp");

das_Asset asset = {};
das_LoadAsset(&asset, DAS_ASSET_MODE_UNDEFINED, {1.0f, 1.0f, 1.0f, .5f}, 
              false, texture.uuid, "example_model.das");
```

In order to load texture data from an image file `das_LoadTexture()` function can be used. This function
takes two parameters, first parameter being a valid pointer to `das_Texture` instance and the second parameter
being a valid texture name. To load a texture mapped asset, the texture must always be loaded first before loading 
an asset itself. In order to load an asset `das_LoadAsset()` function can be used. This function has 6 parameters.  
The first parameter is a valid pointer to `das_Asset` instance, second parameter is the requested asset mode, which
the loader uses to perform asset data conversions from the original mode to the requested mode.  
Valid asset modes for this example can be following:  
* DAS\_ASSET\_MODE\_UNDEFINED - keep the original asset mode    
* DAS\_ASSET\_MODE\_3D\_TEXTURE\_MAPPED - explicitly specify the target asset mode as 3D texture mapped (original asset mode must be either 
DAS\_ASSET\_MODE\_3D\_TEXTURE\_MAPPED or \_\_DAS\_ASSET\_MODE\_3D\_TEXTURE\_MAPPED\_UNOR)  
* DAS\_ASSET\_MODE\_3D\_UNMAPPED - explicitly specify the target asset mode as 3D unmapped (original asset mode must be
either DAS\_ASSET\_MODE\_3D\_TEXTURE\_MAPPED, \_\_DAS\_ASSET\_MODE\_3D\_TEXTURE\_MAPPED\_UNOR, \_\_DAS\_ASSET\_MODE\_3D\_UNMAPPED or 
\_\_DAS\_ASSET\_MODE\_3D\_UNMAPPED\_UNOR)  

Third parameter is RGBA asset color, which is used whenever the asset mode is unmapped or when `force_unmap` boolean
member is set to true in `das_Asset` structure. Fourth parameter is boolean that specifies if `force_unmap` should be 
true or false. Fifth parameter is the UUID value of the loaded texture and the sixth parameter is the asset's file name.


### Creating Renderer class object and setting it up for frame generation

DENG offers abstracted renderer class for both Vulkan and OpenGL backends. The following snippet demonstrates
the correct proceedure for setting up DENG renderer.

```C++
deng::Renderer rend = deng::Renderer(DENG_RENDERER_HINT_MIPMAP_ENABLE | DENG_RENDERER_HINT_MSAA_4, vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f));
rend.submitTexture(texture);
rend.submitAsset(asset);
rend.setup(cam, win);
rend.run();
```

The constructor for `deng::Renderer` takes two parameters. The first parameter specifies renderer hints which are all listed below:  
* DENG\_RENDERER\_HINT\_ENABLE\_VSYNC - enable VSync on rendering
* DENG\_RENDERER\_HINT\_SHOW\_CMD\_FPS\_COUNTER - write the amount of rendered frames to stdout
* DENG\_RENDERER\_HINT\_SHOW\_GUI\_FPS\_COUNTER - create an ImGUI overlay for displaying fps
* DENG\_RENDERER\_HINT\_MSAA\_MAX\_HARDWARE\_SUPPORTED - set MSAA level to maximum supported by the graphics hardware
* DENG\_RENDERER\_HINT\_MSAA\_64 - set MSAA level to 64
* DENG\_RENDERER\_HINT\_MSAA\_32 - set MSAA level to 32
* DENG\_RENDERER\_HINT\_MSAA\_16 - set MSAA level to 16
* DENG\_RENDERER\_HINT\_MSAA\_8 - set MSAA level to 8
* DENG\_RENDERER\_HINT\_MSAA\_4 - set MSAA level to 4
* DENG\_RENDERER\_HINT\_MSAA\_2 - set MSAA level to 2
* DENG\_RENDERER\_HINT\_MSAA\_1 - set MSAA level to 1
* DENG\_RENDERER\_HINT\_MIPMAP\_ENABLE - enable texture mipmapping for better texture quality

Second parameter is a `vec4<deng_vec_t>` instance that specifies the background color of the window.

After successfully setting up the renderer object, textures and assets can be submitted. For this purpose 
`deng::Renderer::submitTexture()` and `deng::Renderer::submitAsset()` methods can be used accordingly. 
To finalise the renderer setup process `deng::Renderer::setup()` method must be called. This method prepares
the backend API, for final rendering. As for parameters `deng::Renderer::setup()` method takes a reference 
to Camera3D and Window.  

After all the initialisation steps are complete the `run()` method can be called and you should have your texture mapped
asset drawn to the window.


### Code example

Full source code for the given example can be found [here](examples/3d_deng.cpp)
