#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "GEGraphicsContext.h"
#include "GERenderingContext.h"
#include "GEUniformBuffer.h"
#include "GETexture.h"

//
// CLASE: GEDescriptorSet
//
// DESCRIPCI�N: Clase que describe un conjunto de descriptores 
//
class GEDescriptorSet
{
private:
	VkDescriptorPool descriptorPool;

public:
	std::vector<VkDescriptorSet> descriptorSets;

public:
	GEDescriptorSet(GEGraphicsContext* gc, GERenderingContext* rc, std::vector<GEUniformBuffer*> ubos, std::vector<GETexture*> tex, GERenderingContext* shadow_rc);
	void destroy(GEGraphicsContext* gc);
};

