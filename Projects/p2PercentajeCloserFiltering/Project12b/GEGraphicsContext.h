#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

//
// CLASE: GEGraphicsContext
//
// DESCRIPCI�N: Clase que almacena el contexto gr�fico de vulkan (instancia y dispositivo)
//
class GEGraphicsContext
{
public:
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	uint32_t graphicsQueueFamilyIndex;
	uint32_t presentQueueFamilyIndex;

private:
	VkPhysicalDeviceMemoryProperties memProperties;

public:
	GEGraphicsContext(GLFWwindow* window);
	~GEGraphicsContext();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	VkFormat findDepthFormat();

private:
	// M�todos de inicializaci�n de Vulkan
	void createInstance();
	void createSurface(GLFWwindow* window);
	void pickPhysicalDevice();
	void createLogicalDevice();

	// M�todos auxiliares
	void showInstanceProperties();
	bool isDeviceSuitable(VkPhysicalDevice pDevice);
	void showDevices();
	void resumeDeviceProperties(VkPhysicalDevice pDevice, int index);
};

