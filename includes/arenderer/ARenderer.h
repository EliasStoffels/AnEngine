#ifndef ARENDERER_A_RENDERER_H
#define ARENDERER_A_RENDERER_H

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "arenderer/Vertex.h"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>
#include <algorithm>
#include <fstream>
#include <set>
#include <array>

// structs
//==============================================================================================================================================
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats{};
    std::vector<VkPresentModeKHR> presentModes{};
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

// global static functions
//==============================================================================================================================================

// proxy function for vkCreateDebugUtilsMessengerEXT (function is not automatically loaded so this one looks for it)
inline VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

// proxy function for vkDestroyDebugUtilsMessengerEXT (function is not automatically loaded so this one looks for it)
inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

namespace arenderer {
	class ARenderer {
    public:
        void Run();
    private:
        // members
        GLFWwindow* window = nullptr;
        VkInstance instance = nullptr;
        VkDebugUtilsMessengerEXT debugMessenger = nullptr;
        VkSurfaceKHR surface = nullptr;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = nullptr;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentQueue = nullptr;
        VkSwapchainKHR swapChain = nullptr;
        std::vector<VkImage> swapChainImages{};
        VkFormat swapChainImageFormat{};
        VkExtent2D swapChainExtent{};
        std::vector<VkImageView> swapChainImageViews{};
        VkRenderPass renderPass = nullptr;
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        VkPipelineLayout pipelineLayout = nullptr;
        VkPipeline graphicsPipeline = nullptr;
        std::vector<VkFramebuffer> swapChainFramebuffers{};
        VkCommandPool commandPool = nullptr;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        bool framebufferResized = false;
        uint32_t currentFrame = 0;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        VkBuffer vertexBuffer = nullptr;
        VkDeviceMemory vertexBufferMemory = nullptr;
        VkBuffer indexBuffer = nullptr;
        VkDeviceMemory indexBufferMemory = nullptr;

        std::vector<VkBuffer> uniformBuffers{};
        std::vector<VkDeviceMemory> uniformBuffersMemory{};
        std::vector<void*> uniformBuffersMapped{};

        VkDescriptorPool descriptorPool = nullptr;
        std::vector<VkDescriptorSet> descriptorSets;

        std::uint32_t mipLevels;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

        VkImage colorImage;
        VkDeviceMemory colorImageMemory;
        VkImageView colorImageView;

        // required extensions
        std::vector<const char*> GetRequiredExtensions();

        // extension callback
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData) {

            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

            return VK_FALSE;
        }

        // window init
        //==============================================================================================================================================
        void InitWindow();

        static void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
            auto app = reinterpret_cast<ARenderer*>(glfwGetWindowUserPointer(window));
            app->framebufferResized = true;
        }

        // vulkan init
        //==============================================================================================================================================
        void InitVulkan();
        void LoadModel();
        VkSampleCountFlagBits GetMaxUsableSampleCount();

        // MainLoop
        //==============================================================================================================================================
        void MainLoop();

        // FRAME !!!!!!
        //==============================================================================================================================================
        void DrawFrame();
        void UpdateUniformBuffer(uint32_t currentImage);

        // cleanup
        //==============================================================================================================================================
        void Cleanup();
        void CleanupSwapChain();
        void CreateDescriptorSetLayout();

        // sync objects
        //==============================================================================================================================================
        void CreateSyncObjects();

        // vertex buffer
        //==============================================================================================================================================
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void CreateVertexBuffer();
        void CreateIndexBuffer();
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void CreateUniformBuffers();
        void CreateDescriptorPool();
        void CreateDescriptorSets();
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        // command buffer
        //==============================================================================================================================================
        void CreateCommandBuffers();
        void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

        // command pool
        //==============================================================================================================================================
        void CreateCommandPool();

        // renderpass
        //==============================================================================================================================================
        void CreateRenderPass();

        // image
        //==============================================================================================================================================
        void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        void CreateTextureImage();
        void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void CreateDepthResources();
        void CreateColorResources();
        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkFormat FindDepthFormat();
        bool HasStencilComponent(VkFormat format);

        // framebuffers
        //==============================================================================================================================================
        void CreateFramebuffers();
        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
        void CreateTextureImageView();
        void CreateTextureSampler();

        // pipelline
        //==============================================================================================================================================
        void CreateGraphicsPipeline();

        //create surface
        //==============================================================================================================================================
        void CreateSurface();

        // instance
        //==============================================================================================================================================
        void CreateInstance();
        // validation layers
        bool CheckValidationLayerSupport();
        // populate the debugMessenger struct
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        // debug setup (obv)
        void SetupDebugMessenger();

        // choose physical device
        //==============================================================================================================================================
        void PickPhysicalDevice();
        // device suitablity check
        bool IsDeviceSuitable(VkPhysicalDevice device);
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

        //swap chain select functions
        //==========================================================================================================================================
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        //create swapchain
        //==========================================================================================================================================
        void CreateSwapChain();
        void CreateImageViews();
        void RecreateSwapChain();
        // find queue familys obv
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
        void CreateLogicalDevice();

        // shaders
        //==============================================================================================================================================
        static std::vector<char> ReadFile(const std::string& filename);
        VkShaderModule CreateShaderModule(const std::vector<char>& code);
	};

} // namespace arenderer

#endif // ARENDERER_A_RENDERER_H
