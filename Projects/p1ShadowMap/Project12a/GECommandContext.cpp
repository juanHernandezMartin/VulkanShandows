#include "GECommandContext.h"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////                               M�todos p�blicos                                  /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCI�N: GECommandContext::GECommandContext(GEGraphicsContext* gc, uint32_t imageCount)
//
// PROP�SITO: Construye los buffers de comnandos
//
GECommandContext::GECommandContext(GEGraphicsContext* gc, uint32_t imageCount)
{
	createCommandPool(gc);
	createCommandBuffers(gc, imageCount);
}

//
// FUNCI�N: GECommandContext::destroy(GEGraphicsContext* gc)
//
// PROP�SITO: Destruye los buffers de comnandos
//
void GECommandContext::destroy(GEGraphicsContext* gc)
{
	size_t bufferCount = commandBuffers.size();

	vkFreeCommandBuffers(gc->device, commandPool, (uint32_t) bufferCount, commandBuffers.data());
	vkDestroyCommandPool(gc->device, commandPool, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////                     M�todos de creaci�n de los componentes                      /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCI�N: GECommandContext::createCommandPool(GEGraphicsContext* gc)
//
// PROP�SITO: Crea el command pool vinculado a la familia de colas para gr�ficos
//
void GECommandContext::createCommandPool(GEGraphicsContext* gc)
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = gc->graphicsQueueFamilyIndex;

	if (vkCreateCommandPool(gc->device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

//
// FUNCI�N: GECommandContext::createCommandBuffers(GEGraphicsContext* gc, uint32_t imageCount)
//
// PROP�SITO: Crea los buffers de comandos que se enviar�n a la cola gr�fica
//            El contenido de los buffers incluye la orden de dibujar.
//
void GECommandContext::createCommandBuffers(GEGraphicsContext* gc, uint32_t imageCount)
{
	commandBuffers.resize(imageCount);

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = imageCount;

	if (vkAllocateCommandBuffers(gc->device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

//
// FUNCI�N: GECommandContext::beginCommandBuffers()
//
// PROP�SITO: Comienza a rellenar los command buffers
//
void GECommandContext::beginCommandBuffers()
{
	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}
	}
}

//
// FUNCI�N: GECommandContext::endCommandBuffers()
//
// PROP�SITO: Termina de rellenar los command buffers
//
void GECommandContext::endCommandBuffers()
{
	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}