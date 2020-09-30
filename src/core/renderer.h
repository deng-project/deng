#ifndef RENDERER_H
#define RENDERER_H

namespace deng {

    class Renderer
    {   
    private:
        std::vector<const char*> m_required_extension_names;
        const float m_near_plane = 0.1f;
        const float m_far_plane = 25.0f;

        const char *m_p_validation_layer = "VK_LAYER_KHRONOS_validation";

        const int m_MAX_FRAMES_IN_FLIGHT = 2; 
        size_t m_current_frame = 0;

        // 1) main pipeline;
        // 2) grid pipeline
        // 3) reverse grid pipeline
        // 4) dengui pipeline
        std::array<PipelineData, 4> m_pipelines;

        vec2<std::pair<dengPipelineType, VkDescriptorPool>> m_descriptor_pool_sets;
        vec2<std::pair<dengPipelineType, std::vector<VkDescriptorSet>>> m_descriptor_sets;
        vec2<std::pair<dengPipelineType, VkDescriptorSetLayout>> m_descriptor_set_layouts;
        vec2<std::pair<dengPipelineType, VkPipelineLayout>> m_pipeline_layouts;

        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_debug_messenger;

        VkPhysicalDevice m_gpu;
        VkDevice m_device;
        VkSurfaceKHR m_surface;
        VkSwapchainKHR m_swapchain;

        //swap chain settings
        VkSurfaceFormatKHR m_surface_format;
        VkPresentModeKHR m_present_mode;
        VkExtent2D m_extent;

        VkRenderPass m_renderpass;

        std::vector<VkFramebuffer> m_swapchain_framebuffers;
        VkCommandPool m_commandpool;

        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;
        std::vector<const char*> m_required_extensions_name;

        std::vector<VkSemaphore> m_image_available_semaphore_set;
        std::vector<VkSemaphore> m_render_finished_semaphore_set;
        std::vector<VkFence> m_flight_fences;

        std::vector<VkCommandBuffer> m_commandbuffers;

        Buffers m_buffers;

        SwapChainDetails *m_p_device_swapchain_details;
        Camera *m_p_camera;
        Events *m_p_ev;
        Timer m_timer;
        Window *m_p_window;
        QueueFamilies m_queue_families;
        Queues m_queues;
        FileManager m_fm;
        DepthImageData m_depthimage_data;
        GridManager *m_p_grid_manager;

        // config structs
        DengUIConf m_dengui_conf;
        EditorEnvironmentConf m_environment_conf;
        EditorCameraConf m_camera_conf;

        //game objects (currently just hardcoded, soon to be added from editor in form of objects vector)
        GameObject m_sample_object;
        SpecifiedObject m_grid;
        dengUI::Window *m_p_dengui_window;
        
        // std::vector<dengUI::UIVerticesData> m_dengui_window_vertices;
        // std::vector<uint16_t> m_dengui_window_indices;

    private:
        void loadDataFromConf(const dengBool &load_camera_conf, const dengBool &load_environment_conf, const dengBool &load_dengUI_conf);
        void initInstance();
        void initDebugMessenger();
        bool checkValidationLayerSupport();
        void selectPhysicalDevice();
        void initLogicalDevice();
        void initWindowSurface();
        void initSwapChainSettings();
        void initSwapChain();
        VkImageViewCreateInfo getImageViewInfo(VkImage &image, const VkFormat &format, const VkImageAspectFlags &aspect_flags);
        void initImageView();
        void initRenderPass();
        void initDescriptorSetLayouts();
        void initPipelineLayouts();
        void initGraphicsPipelines();
        void initDengUI();
        void initFrameBuffers();
        void initDepthResources();
        void initTextureImage(GameObject &obj);
        void initTextureSampler(GameObject &obj);
        void initBuffers(GameObject &obj);
        void initDescriptorPool();
        void initDescriptorSets();
        void initTextureBuffer();
        void initCommandPool();
        void initCommandBuffers();
        void initSemaphores();
        
        void initObjects(GameObject &obj, const std::string &obj_source_filepath, const std::string &tex_source_filepath, const dengCoordinateMode &coordinate_mode);
        void initGrid();

        void deleteTextureImage(GameObject &obj);
        void deleteCommandBuffers();
        void deleteSemaphores();
        void deleteCommandPool();
        void deleteFrameBuffers();
        void deletePipelines();
        void deleteRenderPass();
        void deleteImageViews();
        void deleteSwapChain();
        void deleteDescriptorSetLayout();
        void deleteBuffers();
        void deleteDepthImageData();
        void deleteDebugMessenger();
        void deleteInstance();
        void deleteSurface();
        void deleteDevice();

        /* frame update */
        void makeFrame();
        void updateUniformBufferData(const uint32_t &current_image, GameObject &obj);

        /* VkImage related functions */
        void makeImage(const VkFormat &format, const VkImageTiling &tiling, const VkImageUsageFlags &usage, const VkMemoryPropertyFlags &properties, GameObject *p_obj, const dengImageType &image_type);
        void transitionImageLayout(VkImage &image, const VkFormat &format, const VkImageLayout &old_layout, const VkImageLayout &new_layout); 
        void copyBufferToImage(VkBuffer &src_buffer, VkImage &dst_image, const uint32_t &width, const uint32_t &height);
        
        /* VkBuffer related functions */
        static void makeBuffer(VkDevice *p_device, VkPhysicalDevice *p_gpu, VkDeviceSize *p_size, const VkBufferUsageFlags &usage, const VkMemoryPropertyFlags &properties, VkBuffer *p_buffer, VkDeviceMemory *p_buffer_memory, size_t *p_buffer_index);
        static void populateBufferMem(VkDevice *p_device, VkPhysicalDevice *p_gpu, VkDeviceSize *p_size, const void *p_src_data, VkBuffer *p_buffer, VkDeviceMemory *p_buffer_memory);
        static void copyBufferToBuffer(VkDevice *p_device, VkCommandPool *p_commandpool, VkQueue *p_graphics_queue, VkBuffer *p_src_buffer, VkBuffer *p_dst_buffer, VkDeviceSize *p_size);

        /* single commandbuffer command recorder function */
        static void beginCommandBufferSingleCommand(VkDevice *device, VkCommandPool *commandpool, VkCommandBuffer &commandbuffer);
        static void endCommandBufferSingleCommand(VkDevice *device, VkQueue *graphics_queue, VkCommandPool *commandpool, VkCommandBuffer &commandBuffer);

        VkResult makeDebugMessenger(const VkDebugUtilsMessengerCreateInfoEXT  *p_messenger_createinfo);
        std::vector<const char*> getRequiredExtensions();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data, void *p_user_data);

    public:
        void run();
        Renderer(Window &win);
        ~Renderer();
    };
}


#endif