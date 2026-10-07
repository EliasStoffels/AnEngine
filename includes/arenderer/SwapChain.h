#ifndef ARENDERER_SWAP_CHAIN_H
#define ARENDERER_SWAP_CHAIN_H

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

namespace arenderer {

	class SwapChain {
    public:
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        std::vector<VkImage> swapChainImages{};
        VkFormat swapChainImageFormat{};
        VkExtent2D swapChainExtent{};
        std::vector<VkImageView> swapChainImageViews{};
        std::vector<VkFramebuffer> swapChainFramebuffers{};

        void Create(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow* window);
        void CreateImageViews(VkDevice device);
        void Destroy(VkDevice device);

    private:
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	};
}

#endif // !ARENDERER_SWAP_CHAIN_H
