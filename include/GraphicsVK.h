#ifndef _GRAPHICS_VK_H
#define _GRAPHICS_VK_H

// Forcing this define for 32bit typesafe conversions, as in
// being able to construct c++ classes based of vulkan c handles
// theoretically this is unsafe - check vulkan.hpp for more info
#define VULKAN_HPP_TYPESAFE_CONVERSION
#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Graphics.h"
#include "Common.h"
#include "Game.h"

class GraphicsVK : public Graphics
{
public:
	GraphicsVK() {}

	void Init() override;
	void BeginGather() override;
	void Render(const Camera *cam, GameObject *go, const uint32_t threadId) override;
	void Display() override;
	void CleanUp() override;

	// this is a trampoline to the actual implementation
	static void OnWindowResized(GLFWwindow *window, int width, int height);

	vec3 GetModelCenter(ModelId m) override { return vec3(0, 0, 0); }
private:
	void LoadResources();

	static GraphicsVK *activeGraphics;
	void WindowResized(int width, int height);

	// Instance related
	vk::Instance instance;
	void CreateInstance();
	
	// Device related
	const static vector<const char *> requiredLayers;
	const static vector<const char *> requiredExtensions;
	vk::Device device;
	vk::PhysicalDevice physDevice;
	void CreateDevice();
	bool IsSuitable(const vk::PhysicalDevice &device);

	// Queues from Device
	struct QueuesInfo {
		uint32_t graphicsFamIndex, computeFamIndex, transferFamIndex, presentFamIndex;
		vk::Queue graphicsQueue, computeQueue, transferQueue, presentQueue;
	};
	QueuesInfo queues;

	// Window surface
	vk::SurfaceKHR surface;
	void CreateSurface();

	// Swapchain
	struct SwapchainSupportInfo {
		vk::SurfaceCapabilitiesKHR surfCaps;
		vector<vk::SurfaceFormatKHR> suppFormats;
		vector<vk::PresentModeKHR> presentModes;

		// used swapchain settings
		vk::Format imgFormat;
		vk::Extent2D swapExtent;
	};
	SwapchainSupportInfo swapInfo;
	vk::SwapchainKHR swapchain;
	vector<vk::Image> images; // images to render to, auto-destroyed by swapchain
	vector<vk::ImageView> imgViews; // views to acess images through, needs to be destroyed
	void CreateSwapchain();

	// Render Passes
	vk::RenderPass renderPass;
	void CreateRenderPass();

	// Graphic Pipeline
	vector<vk::Pipeline> pipelines;
	vk::PipelineLayout pipelineLayout;
	vk::ShaderModule vertShader, fragShader;
	vk::Pipeline CreatePipeline(string name);

	// Render Frame Buffers
	uint32_t currImgIndex;
	vector<vk::Framebuffer> swapchainFrameBuffers;
	void CreateFrameBuffers();

	// Command Pool, Buffers and Semaphores
	vk::Semaphore imgAvailable, renderFinished;
	array<vk::Fence, 3> cmdFences;
	vk::CommandPool graphCmdPool, transfCmdPool;
	vector<vk::CommandPool> graphSecCmdPools;
	vector<vk::CommandBuffer> cmdBuffers; // for now we have a buffer per swapchain fbo
	// it's a little strange, but essentially per each pipeline there's a secondary command buffer
	// each thread has it's own version of pipeline-linked cmd buffer
	// while also supporting triple buffering
	vector<vk::CommandBuffer> secCmdBuffers[maxThreads][3]; // for recording 
	void CreateCommandResources();

	// Descriptor Sets
	struct MatUBO {
		mat4 model;
		mat4 mvp;
	};
	vk::DescriptorSetLayout descriptorLayout;
	void CreateDescriptorSetLayout();
	vk::DescriptorPool descriptorPool;
	array<vk::DescriptorSet, Game::maxObjects> descriptorSets;
	void CreateDescriptorPool();
	void CreateDescriptorSet();
	size_t uboSize = 0;
	void *mappedUboMem = nullptr;
	vk::Buffer ubo;
	vk::DeviceMemory uboMem;
	void CreateUBO();
	void DestroyUBO();
	size_t alignment = 0;
	size_t GetAlignedOffset(size_t ind, size_t step)
	{
		size_t actualStep = ceil(step * 1.f / alignment) * alignment;
		size_t currSize = ind * actualStep;
		return currSize;
	}

	// Renderable resources
	vk::VertexInputBindingDescription GetBindingDescription() const;
	array<vk::VertexInputAttributeDescription, 3> GetAttribDescriptions() const;
	// we're gonna push everything to a single vbo/ibo
	// LoadResources handles the vbo/ibo creation
	vk::Buffer vbo, ibo;
	vk::DeviceMemory vboMem, iboMem;
	vector<Model> models;
	void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memProps, vk::Buffer &buff, vk::DeviceMemory &mem);
	void CopyBuffer(vk::Buffer from, vk::Buffer to, vk::DeviceSize size);
	uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags props);

	// Validation Layers related
	// Vulkan C++ binding doesn't have complete extension linking yet, so have to do it manually
	PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback;
	PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback;

	VkDebugReportCallbackEXT debugCallback;
	bool LayersAvailable(const vector<const char*> &validationLayers);
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData);
};

#endif // !_GRAPHICS_VK_H
