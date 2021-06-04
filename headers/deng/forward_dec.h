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


#ifndef FORWARD_DEC_H
#define FORWARD_DEC_H

/* Header file for all forward class and struct declarations */

namespace deng {
    // deng/deng_math.h
    struct vec4;
    struct vec3;
    struct vec2;

    class mat2;
    class mat3;
    class mat4;

    // deng/renderer.h
    namespace vulkan {
        struct __vk_InstanceInfo;
        struct __vk_SwapChainInfo;
        struct __vk_DescriptorInfo;
        struct __vk_ResourceInfo;
        class __vk_InstanceCreator;
        class __vk_SwapChainCreator;
        class __vk_DescriptorCreator;
        class __vk_ResourceAllocator;
        class __vk_DrawCaller;
        class __vk_Renderer;

        struct __vk_PipelineData;
        struct __vk_PipelineCreateinfoSpecifiers;
        struct __vk_DeviceInfo;
        class __vk_PipelineCreator;
        class __vk_SwapChainDetails;
        struct __vk_CommandBufferRecorder;
        class __vk_QueueManager;
    }

    // deng/window.h
    class Window;

    // deng/camera.h
    struct CameraBaseValues;
    struct EventBaseValues;
    class __FPPCameraEv;
    class __FPPCamera;
    class __EditorCameraEv;
    class __EditorCamera;
    class Camera3D;

    // deng/rend_helpers.h
    struct BufferData;
    struct TextureImage;
    struct HardwareSpecs;
    struct BufferCreator;
}

namespace dengui {
    // dengui/dengui_child.h
    struct ParentInfo;
    struct PushButtonInfo;
    class ChildPushButton;
    
    // dengui/dengui_events.h
    struct MouseInputInfo;
    struct EventInfo;
    class Events;

    // dengui/dengui_window.h
    struct WindowUpdateInfo;
    struct SharedWindowUpdateInfos;
    struct ElementBaseData;
    struct WindowElement;
    struct DDMElement;
    struct WindowInfo;
    struct WindowShapeInfo;
    class BaseWindowShapes;
    class Window;
    typedef void(*DDMCallback)(DDMElement*, Events*);
    typedef void(*WindowCallback)(WindowElement*, Events*);

    // dengui/collision.h
    struct Collision2D;
}

#endif