#pragma once

#include <vulkan/vulkan.h>
#include "GEGraphicsContext.h"

//
// CLASE: GEVertexBuffer
//
// DESCRIPCI�N: Clase que describe un buffer de v�rtices
//
class GEVertexBuffer
{
public:
	VkBuffer buffer;
	VkDeviceMemory memory;

	GEVertexBuffer(GEGraphicsContext* gc, size_t size, const void* data);
	void destroy(GEGraphicsContext* gc);
};

