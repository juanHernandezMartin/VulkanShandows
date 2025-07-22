#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "GEGraphicsContext.h"

//
// CLASE: GECommandContext
//
// DESCRIPCI�N: Clase que almacena los buffers de comandos vinculados a cada imagen de la swapchain
//
class GECommandContext
{
public:
	std::vector<VkCommandBuffer> commandBuffers;

	GECommandContext(GEGraphicsContext* gc, uint32_t imageCount);
	void destroy(GEGraphicsContext* gc);
	void beginCommandBuffers();
	void endCommandBuffers();

private:
	VkCommandPool commandPool;
	
	// M�todos de inicializaci�n 
	void createCommandPool(GEGraphicsContext* gc);
	void createCommandBuffers(GEGraphicsContext* gc, uint32_t imageCount);
};

