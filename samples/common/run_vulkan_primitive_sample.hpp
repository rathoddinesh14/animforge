#pragma once

#include "Geometry.hpp"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace aga::samples
{
    inline void vkCheck(VkResult result, const char* operation)
    {
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                std::string(operation) + " failed with VkResult " +
                std::to_string(result));
        }
    }

    inline std::vector<char> readSpirv(const char* path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);

        if (!file)
        {
            throw std::runtime_error(
                std::string("Cannot open SPIR-V shader: ") + path);
        }

        const auto size = file.tellg();

        if (size <= 0)
        {
            throw std::runtime_error(
                std::string("Empty SPIR-V shader: ") + path);
        }

        std::vector<char> bytes(static_cast<std::size_t>(size));
        file.seekg(0);
        file.read(bytes.data(), size);

        if (!file)
        {
            throw std::runtime_error(
                std::string("Failed reading SPIR-V shader: ") + path);
        }

        return bytes;
    }

    class VulkanPrimitiveRenderer
    {
    public:
        VulkanPrimitiveRenderer(
            GLFWwindow* window,
            GeometryView geometry,
            VkPrimitiveTopology topology)
            : window_(window), geometry_(geometry), topology_(topology)
        {
            if (!window_ || !geometry_.data || geometry_.vertexCount == 0)
            {
                throw std::invalid_argument("Invalid window or geometry");
            }

            try
            {
                createInstance();
                createSurface();
                selectPhysicalDevice();
                createLogicalDevice();
                createSwapchain();
                createImageViews();
                createRenderPass();
                createPipeline();
                createFramebuffers();
                createCommandPool();
                createVertexBuffer();
                createCommandBuffers();
                createSyncObjects();
            }
            catch (...)
            {
                cleanup();
                throw;
            }
        }

        VulkanPrimitiveRenderer(const VulkanPrimitiveRenderer&) = delete;
        VulkanPrimitiveRenderer& operator=(const VulkanPrimitiveRenderer&) = delete;

        ~VulkanPrimitiveRenderer()
        {
            cleanup();
        }

        void run()
        {
            while (!glfwWindowShouldClose(window_))
            {
                glfwPollEvents();
                drawFrame();
            }

            vkCheck(vkDeviceWaitIdle(device_), "vkDeviceWaitIdle");
        }

    private:
        static constexpr std::uint32_t FramesInFlight = 2;

        GLFWwindow* window_ = nullptr;
        GeometryView geometry_{};
        VkPrimitiveTopology topology_{};

        VkInstance instance_ = VK_NULL_HANDLE;
        VkSurfaceKHR surface_ = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
        VkDevice device_ = VK_NULL_HANDLE;

        VkQueue graphicsQueue_ = VK_NULL_HANDLE;
        VkQueue presentQueue_ = VK_NULL_HANDLE;

        std::uint32_t graphicsFamily_ = 0;
        std::uint32_t presentFamily_ = 0;

        VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
        VkFormat swapchainFormat_ = VK_FORMAT_UNDEFINED;
        VkExtent2D swapchainExtent_{};
        std::vector<VkImage> swapchainImages_;
        std::vector<VkImageView> imageViews_;

        VkRenderPass renderPass_ = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
        VkPipeline pipeline_ = VK_NULL_HANDLE;
        std::vector<VkFramebuffer> framebuffers_;

        VkCommandPool commandPool_ = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers_;

        VkBuffer vertexBuffer_ = VK_NULL_HANDLE;
        VkDeviceMemory vertexMemory_ = VK_NULL_HANDLE;

        std::array<VkSemaphore, FramesInFlight> imageAvailable_{};
        std::array<VkSemaphore, FramesInFlight> renderFinished_{};
        std::array<VkFence, FramesInFlight> inFlightFences_{};
        std::vector<VkFence> imagesInFlight_;

        std::size_t currentFrame_ = 0;

        struct QueueFamilies
        {
            bool graphics = false;
            bool present = false;
            std::uint32_t graphicsIndex = 0;
            std::uint32_t presentIndex = 0;

            bool complete() const { return graphics && present; }
        };

        QueueFamilies findQueueFamilies(VkPhysicalDevice device)
        {
            QueueFamilies result;

            std::uint32_t count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

            std::vector<VkQueueFamilyProperties> families(count);
            vkGetPhysicalDeviceQueueFamilyProperties(
                device, &count, families.data());

            for (std::uint32_t i = 0; i < count; ++i)
            {
                if (families[i].queueCount > 0 &&
                    (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
                {
                    result.graphics = true;
                    result.graphicsIndex = i;
                }

                VkBool32 supported = VK_FALSE;
                vkCheck(
                    vkGetPhysicalDeviceSurfaceSupportKHR(
                        device, i, surface_, &supported),
                    "vkGetPhysicalDeviceSurfaceSupportKHR");

                if (families[i].queueCount > 0 && supported)
                {
                    result.present = true;
                    result.presentIndex = i;
                }

                if (result.complete())
                    break;
            }

            return result;
        }

        bool supportsSwapchain(VkPhysicalDevice device)
        {
            std::uint32_t count = 0;
            vkCheck(
                vkEnumerateDeviceExtensionProperties(
                    device, nullptr, &count, nullptr),
                "vkEnumerateDeviceExtensionProperties");

            std::vector<VkExtensionProperties> extensions(count);
            vkCheck(
                vkEnumerateDeviceExtensionProperties(
                    device, nullptr, &count, extensions.data()),
                "vkEnumerateDeviceExtensionProperties");

            return std::any_of(
                extensions.begin(), extensions.end(),
                [](const VkExtensionProperties& extension)
                {
                    return std::string(extension.extensionName) ==
                           VK_KHR_SWAPCHAIN_EXTENSION_NAME;
                });
        }

        bool suitableDevice(VkPhysicalDevice device)
        {
            const QueueFamilies queues = findQueueFamilies(device);

            if (!queues.complete() || !supportsSwapchain(device))
                return false;

            std::uint32_t formatCount = 0;
            std::uint32_t presentModeCount = 0;

            vkCheck(
                vkGetPhysicalDeviceSurfaceFormatsKHR(
                    device, surface_, &formatCount, nullptr),
                "vkGetPhysicalDeviceSurfaceFormatsKHR");

            vkCheck(
                vkGetPhysicalDeviceSurfacePresentModesKHR(
                    device, surface_, &presentModeCount, nullptr),
                "vkGetPhysicalDeviceSurfacePresentModesKHR");

            return formatCount > 0 && presentModeCount > 0;
        }

        void createInstance()
        {
            VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
            appInfo.pApplicationName = "AnimForge Vulkan Sample";
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "AnimForge";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_0;

            std::uint32_t extensionCount = 0;
            const char** extensions =
                glfwGetRequiredInstanceExtensions(&extensionCount);

            if (!extensions || extensionCount == 0)
            {
                throw std::runtime_error(
                    "GLFW did not provide Vulkan instance extensions");
            }

            VkInstanceCreateInfo createInfo{
                VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
            createInfo.pApplicationInfo = &appInfo;
            createInfo.enabledExtensionCount = extensionCount;
            createInfo.ppEnabledExtensionNames = extensions;

            vkCheck(
                vkCreateInstance(&createInfo, nullptr, &instance_),
                "vkCreateInstance");
        }

        void createSurface()
        {
            vkCheck(
                glfwCreateWindowSurface(
                    instance_, window_, nullptr, &surface_),
                "glfwCreateWindowSurface");
        }

        void selectPhysicalDevice()
        {
            std::uint32_t count = 0;
            vkCheck(
                vkEnumeratePhysicalDevices(instance_, &count, nullptr),
                "vkEnumeratePhysicalDevices");

            if (count == 0)
                throw std::runtime_error("No Vulkan physical devices found");

            std::vector<VkPhysicalDevice> devices(count);
            vkCheck(
                vkEnumeratePhysicalDevices(
                    instance_, &count, devices.data()),
                "vkEnumeratePhysicalDevices");

            for (VkPhysicalDevice candidate : devices)
            {
                if (suitableDevice(candidate))
                {
                    physicalDevice_ = candidate;
                    const QueueFamilies queues =
                        findQueueFamilies(candidate);
                    graphicsFamily_ = queues.graphicsIndex;
                    presentFamily_ = queues.presentIndex;
                    break;
                }
            }

            if (physicalDevice_ == VK_NULL_HANDLE)
            {
                throw std::runtime_error(
                    "No Vulkan device supports graphics, presentation, "
                    "and swapchains");
            }
        }

        void createLogicalDevice()
        {
            const std::array<std::uint32_t, 2> families = {
                graphicsFamily_, presentFamily_};

            std::vector<std::uint32_t> uniqueFamilies(
                families.begin(), families.end());

            std::sort(uniqueFamilies.begin(), uniqueFamilies.end());
            uniqueFamilies.erase(
                std::unique(uniqueFamilies.begin(), uniqueFamilies.end()),
                uniqueFamilies.end());

            const float priority = 1.0f;
            std::vector<VkDeviceQueueCreateInfo> queueInfos;

            for (std::uint32_t family : uniqueFamilies)
            {
                VkDeviceQueueCreateInfo queueInfo{
                    VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
                queueInfo.queueFamilyIndex = family;
                queueInfo.queueCount = 1;
                queueInfo.pQueuePriorities = &priority;
                queueInfos.push_back(queueInfo);
            }

            const char* extensions[] = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME};

            VkPhysicalDeviceFeatures features{};

            VkDeviceCreateInfo createInfo{
                VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
            createInfo.queueCreateInfoCount =
                static_cast<std::uint32_t>(queueInfos.size());
            createInfo.pQueueCreateInfos = queueInfos.data();
            createInfo.pEnabledFeatures = &features;
            createInfo.enabledExtensionCount = 1;
            createInfo.ppEnabledExtensionNames = extensions;

            vkCheck(
                vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_),
                "vkCreateDevice");

            vkGetDeviceQueue(
                device_, graphicsFamily_, 0, &graphicsQueue_);
            vkGetDeviceQueue(
                device_, presentFamily_, 0, &presentQueue_);
        }

        void createSwapchain()
        {
            VkSurfaceCapabilitiesKHR capabilities{};
            vkCheck(
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                    physicalDevice_, surface_, &capabilities),
                "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

            std::uint32_t formatCount = 0;
            vkCheck(
                vkGetPhysicalDeviceSurfaceFormatsKHR(
                    physicalDevice_, surface_, &formatCount, nullptr),
                "vkGetPhysicalDeviceSurfaceFormatsKHR");

            std::vector<VkSurfaceFormatKHR> formats(formatCount);
            vkCheck(
                vkGetPhysicalDeviceSurfaceFormatsKHR(
                    physicalDevice_, surface_, &formatCount, formats.data()),
                "vkGetPhysicalDeviceSurfaceFormatsKHR");

            if (formats.empty())
                throw std::runtime_error("No supported surface formats");

            VkSurfaceFormatKHR selectedFormat = formats.front();

            for (const auto& format : formats)
            {
                if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
                    format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    selectedFormat = format;
                    break;
                }
            }

            swapchainFormat_ = selectedFormat.format;

            if (capabilities.currentExtent.width !=
                std::numeric_limits<std::uint32_t>::max())
            {
                swapchainExtent_ = capabilities.currentExtent;
            }
            else
            {
                int width = 0;
                int height = 0;
                glfwGetFramebufferSize(window_, &width, &height);

                swapchainExtent_.width = std::clamp(
                    static_cast<std::uint32_t>(std::max(width, 1)),
                    capabilities.minImageExtent.width,
                    capabilities.maxImageExtent.width);

                swapchainExtent_.height = std::clamp(
                    static_cast<std::uint32_t>(std::max(height, 1)),
                    capabilities.minImageExtent.height,
                    capabilities.maxImageExtent.height);
            }

            std::uint32_t imageCount = capabilities.minImageCount + 1;

            if (capabilities.maxImageCount > 0 &&
                imageCount > capabilities.maxImageCount)
            {
                imageCount = capabilities.maxImageCount;
            }

            VkSwapchainCreateInfoKHR createInfo{
                VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
            createInfo.surface = surface_;
            createInfo.minImageCount = imageCount;
            createInfo.imageFormat = swapchainFormat_;
            createInfo.imageColorSpace = selectedFormat.colorSpace;
            createInfo.imageExtent = swapchainExtent_;
            createInfo.imageArrayLayers = 1;
            createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            if (graphicsFamily_ != presentFamily_)
            {
                const std::uint32_t queueIndices[] = {
                    graphicsFamily_, presentFamily_};

                createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices = queueIndices;
            }
            else
            {
                createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            }

            createInfo.preTransform = capabilities.currentTransform;
            createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
            createInfo.clipped = VK_TRUE;

            vkCheck(
                vkCreateSwapchainKHR(
                    device_, &createInfo, nullptr, &swapchain_),
                "vkCreateSwapchainKHR");

            vkCheck(
                vkGetSwapchainImagesKHR(
                    device_, swapchain_, &imageCount, nullptr),
                "vkGetSwapchainImagesKHR");

            swapchainImages_.resize(imageCount);

            vkCheck(
                vkGetSwapchainImagesKHR(
                    device_, swapchain_, &imageCount,
                    swapchainImages_.data()),
                "vkGetSwapchainImagesKHR");

            imagesInFlight_.assign(imageCount, VK_NULL_HANDLE);
        }

        void createImageViews()
        {
            imageViews_.resize(swapchainImages_.size());

            for (std::size_t i = 0; i < swapchainImages_.size(); ++i)
            {
                VkImageViewCreateInfo createInfo{
                    VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
                createInfo.image = swapchainImages_[i];
                createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                createInfo.format = swapchainFormat_;
                createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.subresourceRange.aspectMask =
                    VK_IMAGE_ASPECT_COLOR_BIT;
                createInfo.subresourceRange.baseMipLevel = 0;
                createInfo.subresourceRange.levelCount = 1;
                createInfo.subresourceRange.baseArrayLayer = 0;
                createInfo.subresourceRange.layerCount = 1;

                vkCheck(
                    vkCreateImageView(
                        device_, &createInfo, nullptr, &imageViews_[i]),
                    "vkCreateImageView");
            }
        }

        void createRenderPass()
        {
            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = swapchainFormat_;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout =
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colorReference{};
            colorReference.attachment = 0;
            colorReference.layout =
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorReference;

            VkSubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask =
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstStageMask =
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            VkRenderPassCreateInfo createInfo{
                VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = &colorAttachment;
            createInfo.subpassCount = 1;
            createInfo.pSubpasses = &subpass;
            createInfo.dependencyCount = 1;
            createInfo.pDependencies = &dependency;

            vkCheck(
                vkCreateRenderPass(
                    device_, &createInfo, nullptr, &renderPass_),
                "vkCreateRenderPass");
        }

        VkShaderModule createShaderModule(const std::vector<char>& code)
        {
            VkShaderModuleCreateInfo createInfo{
                VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
            createInfo.codeSize = code.size();
            createInfo.pCode =
                reinterpret_cast<const std::uint32_t*>(code.data());

            VkShaderModule module = VK_NULL_HANDLE;
            vkCheck(
                vkCreateShaderModule(device_, &createInfo, nullptr, &module),
                "vkCreateShaderModule");
            return module;
        }

        void createPipeline()
        {
            const auto vertexCode =
                readSpirv(AGA_VK_VERTEX_SHADER_PATH);
            const auto fragmentCode =
                readSpirv(AGA_VK_FRAGMENT_SHADER_PATH);

            VkShaderModule vertexShader = createShaderModule(vertexCode);
            VkShaderModule fragmentShader = VK_NULL_HANDLE;

            try
            {
                fragmentShader = createShaderModule(fragmentCode);

                VkPipelineShaderStageCreateInfo vertexStage{
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
                vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
                vertexStage.module = vertexShader;
                vertexStage.pName = "main";

                VkPipelineShaderStageCreateInfo fragmentStage{
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
                fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                fragmentStage.module = fragmentShader;
                fragmentStage.pName = "main";

                const VkPipelineShaderStageCreateInfo stages[] = {
                    vertexStage, fragmentStage};

                VkVertexInputBindingDescription binding{};
                binding.binding = 0;
                binding.stride = sizeof(float) * 7;
                binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                std::array<VkVertexInputAttributeDescription, 2> attributes{};
                attributes[0].location = 0;
                attributes[0].binding = 0;
                attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributes[0].offset = 0;

                attributes[1].location = 1;
                attributes[1].binding = 0;
                attributes[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
                attributes[1].offset = sizeof(float) * 3;

                VkPipelineVertexInputStateCreateInfo vertexInput{
                    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
                vertexInput.vertexBindingDescriptionCount = 1;
                vertexInput.pVertexBindingDescriptions = &binding;
                vertexInput.vertexAttributeDescriptionCount =
                    static_cast<std::uint32_t>(attributes.size());
                vertexInput.pVertexAttributeDescriptions = attributes.data();

                VkPipelineInputAssemblyStateCreateInfo inputAssembly{
                    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
                inputAssembly.topology = topology_;
                inputAssembly.primitiveRestartEnable = VK_FALSE;

                VkViewport viewport{};
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = static_cast<float>(swapchainExtent_.width);
                viewport.height = static_cast<float>(swapchainExtent_.height);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;

                VkRect2D scissor{};
                scissor.offset = {0, 0};
                scissor.extent = swapchainExtent_;

                VkPipelineViewportStateCreateInfo viewportState{
                    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
                viewportState.viewportCount = 1;
                viewportState.pViewports = &viewport;
                viewportState.scissorCount = 1;
                viewportState.pScissors = &scissor;

                VkPipelineRasterizationStateCreateInfo rasterizer{
                    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
                rasterizer.depthClampEnable = VK_FALSE;
                rasterizer.rasterizerDiscardEnable = VK_FALSE;
                rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
                rasterizer.lineWidth = 1.0f;
                rasterizer.cullMode = VK_CULL_MODE_NONE;
                rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                rasterizer.depthBiasEnable = VK_FALSE;

                VkPipelineMultisampleStateCreateInfo multisampling{
                    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
                multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

                VkPipelineColorBlendAttachmentState colorBlendAttachment{};
                colorBlendAttachment.colorWriteMask =
                    VK_COLOR_COMPONENT_R_BIT |
                    VK_COLOR_COMPONENT_G_BIT |
                    VK_COLOR_COMPONENT_B_BIT |
                    VK_COLOR_COMPONENT_A_BIT;
                colorBlendAttachment.blendEnable = VK_FALSE;

                VkPipelineColorBlendStateCreateInfo colorBlending{
                    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
                colorBlending.logicOpEnable = VK_FALSE;
                colorBlending.attachmentCount = 1;
                colorBlending.pAttachments = &colorBlendAttachment;

                VkPipelineLayoutCreateInfo layoutInfo{
                    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

                vkCheck(
                    vkCreatePipelineLayout(
                        device_, &layoutInfo, nullptr, &pipelineLayout_),
                    "vkCreatePipelineLayout");

                VkGraphicsPipelineCreateInfo pipelineInfo{
                    VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
                pipelineInfo.stageCount = 2;
                pipelineInfo.pStages = stages;
                pipelineInfo.pVertexInputState = &vertexInput;
                pipelineInfo.pInputAssemblyState = &inputAssembly;
                pipelineInfo.pViewportState = &viewportState;
                pipelineInfo.pRasterizationState = &rasterizer;
                pipelineInfo.pMultisampleState = &multisampling;
                pipelineInfo.pColorBlendState = &colorBlending;
                pipelineInfo.layout = pipelineLayout_;
                pipelineInfo.renderPass = renderPass_;
                pipelineInfo.subpass = 0;

                vkCheck(
                    vkCreateGraphicsPipelines(
                        device_, VK_NULL_HANDLE, 1, &pipelineInfo,
                        nullptr, &pipeline_),
                    "vkCreateGraphicsPipelines");
            }
            catch (...)
            {
                if (fragmentShader != VK_NULL_HANDLE)
                    vkDestroyShaderModule(
                        device_, fragmentShader, nullptr);

                vkDestroyShaderModule(device_, vertexShader, nullptr);
                throw;
            }

            vkDestroyShaderModule(device_, fragmentShader, nullptr);
            vkDestroyShaderModule(device_, vertexShader, nullptr);
        }

        void createFramebuffers()
        {
            framebuffers_.resize(imageViews_.size());

            for (std::size_t i = 0; i < imageViews_.size(); ++i)
            {
                VkFramebufferCreateInfo createInfo{
                    VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
                createInfo.renderPass = renderPass_;
                createInfo.attachmentCount = 1;
                createInfo.pAttachments = &imageViews_[i];
                createInfo.width = swapchainExtent_.width;
                createInfo.height = swapchainExtent_.height;
                createInfo.layers = 1;

                vkCheck(
                    vkCreateFramebuffer(
                        device_, &createInfo, nullptr, &framebuffers_[i]),
                    "vkCreateFramebuffer");
            }
        }

        std::uint32_t findMemoryType(
            std::uint32_t typeFilter,
            VkMemoryPropertyFlags properties)
        {
            VkPhysicalDeviceMemoryProperties memoryProperties{};
            vkGetPhysicalDeviceMemoryProperties(
                physicalDevice_, &memoryProperties);

            for (std::uint32_t i = 0;
                 i < memoryProperties.memoryTypeCount; ++i)
            {
                if ((typeFilter & (1u << i)) &&
                    (memoryProperties.memoryTypes[i].propertyFlags &
                     properties) == properties)
                {
                    return i;
                }
            }

            throw std::runtime_error("No suitable Vulkan memory type found");
        }

        void createCommandPool()
        {
            VkCommandPoolCreateInfo createInfo{
                VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
            createInfo.flags =
                VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            createInfo.queueFamilyIndex = graphicsFamily_;

            vkCheck(
                vkCreateCommandPool(
                    device_, &createInfo, nullptr, &commandPool_),
                "vkCreateCommandPool");
        }

        void createVertexBuffer()
        {
            const VkDeviceSize bufferSize =
                geometry_.vertexCount * sizeof(float) * 7;

            VkBufferCreateInfo bufferInfo{
                VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
            bufferInfo.size = bufferSize;
            bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            vkCheck(
                vkCreateBuffer(
                    device_, &bufferInfo, nullptr, &vertexBuffer_),
                "vkCreateBuffer");

            VkMemoryRequirements requirements{};
            vkGetBufferMemoryRequirements(
                device_, vertexBuffer_, &requirements);

            VkMemoryAllocateInfo allocation{
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
            allocation.allocationSize = requirements.size;
            allocation.memoryTypeIndex = findMemoryType(
                requirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            vkCheck(
                vkAllocateMemory(
                    device_, &allocation, nullptr, &vertexMemory_),
                "vkAllocateMemory");

            vkCheck(
                vkBindBufferMemory(
                    device_, vertexBuffer_, vertexMemory_, 0),
                "vkBindBufferMemory");

            void* mapped = nullptr;
            vkCheck(
                vkMapMemory(
                    device_, vertexMemory_, 0, bufferSize, 0, &mapped),
                "vkMapMemory");

            std::copy_n(
                geometry_.data,
                geometry_.vertexCount * 7,
                static_cast<float*>(mapped));

            vkUnmapMemory(device_, vertexMemory_);
        }

        void createCommandBuffers()
        {
            commandBuffers_.resize(framebuffers_.size());

            VkCommandBufferAllocateInfo allocation{
                VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
            allocation.commandPool = commandPool_;
            allocation.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocation.commandBufferCount =
                static_cast<std::uint32_t>(commandBuffers_.size());

            vkCheck(
                vkAllocateCommandBuffers(
                    device_, &allocation, commandBuffers_.data()),
                "vkAllocateCommandBuffers");
        }

        void createSyncObjects()
        {
            VkSemaphoreCreateInfo semaphoreInfo{
                VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

            VkFenceCreateInfo fenceInfo{
                VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for (std::size_t i = 0; i < FramesInFlight; ++i)
            {
                vkCheck(
                    vkCreateSemaphore(
                        device_, &semaphoreInfo, nullptr,
                        &imageAvailable_[i]),
                    "vkCreateSemaphore(imageAvailable)");

                vkCheck(
                    vkCreateSemaphore(
                        device_, &semaphoreInfo, nullptr,
                        &renderFinished_[i]),
                    "vkCreateSemaphore(renderFinished)");

                vkCheck(
                    vkCreateFence(
                        device_, &fenceInfo, nullptr,
                        &inFlightFences_[i]),
                    "vkCreateFence");
            }
        }

        void recordCommandBuffer(
            VkCommandBuffer commandBuffer,
            std::uint32_t imageIndex)
        {
            VkCommandBufferBeginInfo beginInfo{
                VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};

            vkCheck(
                vkBeginCommandBuffer(commandBuffer, &beginInfo),
                "vkBeginCommandBuffer");

            VkClearValue clearColor{};
            clearColor.color = {{0.04f, 0.04f, 0.06f, 1.0f}};

            VkRenderPassBeginInfo renderInfo{
                VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
            renderInfo.renderPass = renderPass_;
            renderInfo.framebuffer = framebuffers_[imageIndex];
            renderInfo.renderArea.offset = {0, 0};
            renderInfo.renderArea.extent = swapchainExtent_;
            renderInfo.clearValueCount = 1;
            renderInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(
                commandBuffer, &renderInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(
                commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);

            const VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(
                commandBuffer, 0, 1, &vertexBuffer_, &offset);

            vkCmdDraw(
                commandBuffer,
                static_cast<std::uint32_t>(geometry_.vertexCount),
                1, 0, 0);

            vkCmdEndRenderPass(commandBuffer);

            vkCheck(
                vkEndCommandBuffer(commandBuffer),
                "vkEndCommandBuffer");
        }

        void drawFrame()
        {
            vkCheck(
                vkWaitForFences(
                    device_, 1, &inFlightFences_[currentFrame_],
                    VK_TRUE, UINT64_MAX),
                "vkWaitForFences");

            std::uint32_t imageIndex = 0;
            const VkResult acquireResult = vkAcquireNextImageKHR(
                device_, swapchain_, UINT64_MAX,
                imageAvailable_[currentFrame_], VK_NULL_HANDLE, &imageIndex);

            if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
            {
                glfwSetWindowShouldClose(window_, GLFW_TRUE);
                return;
            }

            if (acquireResult != VK_SUCCESS &&
                acquireResult != VK_SUBOPTIMAL_KHR)
            {
                vkCheck(acquireResult, "vkAcquireNextImageKHR");
            }

            if (imagesInFlight_[imageIndex] != VK_NULL_HANDLE)
            {
                vkCheck(
                    vkWaitForFences(
                        device_, 1, &imagesInFlight_[imageIndex],
                        VK_TRUE, UINT64_MAX),
                    "vkWaitForFences(image)");
            }

            imagesInFlight_[imageIndex] =
                inFlightFences_[currentFrame_];

            vkCheck(
                vkResetFences(
                    device_, 1, &inFlightFences_[currentFrame_]),
                "vkResetFences");

            vkCheck(
                vkResetCommandBuffer(
                    commandBuffers_[imageIndex], 0),
                "vkResetCommandBuffer");

            recordCommandBuffer(
                commandBuffers_[imageIndex], imageIndex);

            const VkPipelineStageFlags waitStage =
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

            VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores =
                &imageAvailable_[currentFrame_];
            submitInfo.pWaitDstStageMask = &waitStage;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffers_[imageIndex];
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores =
                &renderFinished_[currentFrame_];

            vkCheck(
                vkQueueSubmit(
                    graphicsQueue_, 1, &submitInfo,
                    inFlightFences_[currentFrame_]),
                "vkQueueSubmit");

            VkPresentInfoKHR presentInfo{
                VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores =
                &renderFinished_[currentFrame_];
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = &swapchain_;
            presentInfo.pImageIndices = &imageIndex;

            const VkResult presentResult =
                vkQueuePresentKHR(presentQueue_, &presentInfo);

            if (presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
                presentResult == VK_SUBOPTIMAL_KHR)
            {
                glfwSetWindowShouldClose(window_, GLFW_TRUE);
            }
            else
            {
                vkCheck(presentResult, "vkQueuePresentKHR");
            }

            currentFrame_ = (currentFrame_ + 1) % FramesInFlight;
        }

        void cleanup()
        {
            if (device_ != VK_NULL_HANDLE)
                vkDeviceWaitIdle(device_);

            if (device_ != VK_NULL_HANDLE)
            {
                for (std::size_t i = 0; i < FramesInFlight; ++i)
                {
                    if (imageAvailable_[i] != VK_NULL_HANDLE)
                        vkDestroySemaphore(
                            device_, imageAvailable_[i], nullptr);

                    if (renderFinished_[i] != VK_NULL_HANDLE)
                        vkDestroySemaphore(
                            device_, renderFinished_[i], nullptr);

                    if (inFlightFences_[i] != VK_NULL_HANDLE)
                        vkDestroyFence(
                            device_, inFlightFences_[i], nullptr);
                }

                if (!commandBuffers_.empty() &&
                    commandPool_ != VK_NULL_HANDLE)
                {
                    vkFreeCommandBuffers(
                        device_, commandPool_,
                        static_cast<std::uint32_t>(commandBuffers_.size()),
                        commandBuffers_.data());
                }

                if (vertexBuffer_ != VK_NULL_HANDLE)
                    vkDestroyBuffer(device_, vertexBuffer_, nullptr);

                if (vertexMemory_ != VK_NULL_HANDLE)
                    vkFreeMemory(device_, vertexMemory_, nullptr);

                for (VkFramebuffer framebuffer : framebuffers_)
                    vkDestroyFramebuffer(device_, framebuffer, nullptr);

                if (pipeline_ != VK_NULL_HANDLE)
                    vkDestroyPipeline(device_, pipeline_, nullptr);

                if (pipelineLayout_ != VK_NULL_HANDLE)
                    vkDestroyPipelineLayout(
                        device_, pipelineLayout_, nullptr);

                if (renderPass_ != VK_NULL_HANDLE)
                    vkDestroyRenderPass(device_, renderPass_, nullptr);

                for (VkImageView view : imageViews_)
                    vkDestroyImageView(device_, view, nullptr);

                if (swapchain_ != VK_NULL_HANDLE)
                    vkDestroySwapchainKHR(device_, swapchain_, nullptr);

                if (commandPool_ != VK_NULL_HANDLE)
                    vkDestroyCommandPool(device_, commandPool_, nullptr);

                vkDestroyDevice(device_, nullptr);
                device_ = VK_NULL_HANDLE;
            }

            if (surface_ != VK_NULL_HANDLE && instance_ != VK_NULL_HANDLE)
                vkDestroySurfaceKHR(instance_, surface_, nullptr);

            if (instance_ != VK_NULL_HANDLE)
                vkDestroyInstance(instance_, nullptr);
        }
    };

    inline int runVulkanPrimitiveSample(
        const char* title,
        GeometryView geometry,
        VkPrimitiveTopology topology)
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW\n";
            return 1;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        GLFWwindow* window = glfwCreateWindow(
            800, 600, title, nullptr, nullptr);

        if (!window)
        {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return 1;
        }

        int result = 0;

        try
        {
            VulkanPrimitiveRenderer renderer(window, geometry, topology);
            renderer.run();
        }
        catch (const std::exception& exception)
        {
            std::cerr << "Vulkan sample failed: "
                      << exception.what() << '\n';
            result = 1;
        }

        glfwDestroyWindow(window);
        glfwTerminate();
        return result;
    }

    inline int runVulkanTriangleSample(GeometryView geometry)
    {
        return runVulkanPrimitiveSample(
            "AnimForge - 10 VK Triangle",
            geometry,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    }

    inline int runVulkanLineSample(
        const char* title,
        GeometryView geometry)
    {
        return runVulkanPrimitiveSample(
            title, geometry, VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
    }

    inline int runVulkanPointSample(
        const char* title,
        GeometryView geometry)
    {
        return runVulkanPrimitiveSample(
            title, geometry, VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
    }
}