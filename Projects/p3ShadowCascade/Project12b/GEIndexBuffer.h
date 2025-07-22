#pragma once

#include <vulkan/vulkan.h>
#include "GEGraphicsContext.h"

//
// CLASE: GEIndexBuffer
//
// DESCRIPCI�N: Clase que describe un buffer de �ndices
//
class GEIndexBuffer
{
public:
	VkBuffer buffer;
	VkDeviceMemory memory;

	GEIndexBuffer(GEGraphicsContext* gc, size_t size, const void* data);
	void destroy(GEGraphicsContext* gc);
};

