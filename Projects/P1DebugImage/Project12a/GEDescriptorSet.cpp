#include "GEDescriptorSet.h"

#include "GEUniformBuffer.h"
#include "GETexture.h"
#include <iostream>

//
// FUNCIÓN:GEDescriptorSet::GEDescriptorSet(GEGraphicsContext* gc, std::vector<GEUniformBuffer> ubos)
// 
// PROPÓSITO: Crea los conjuntos de descriptores asociados a los buffers
//
GEDescriptorSet::GEDescriptorSet(GEGraphicsContext* gc, GERenderingContext* rc, std::vector<GEUniformBuffer*> ubos, std::vector<GETexture*> tex, GERenderingContext* shadow_rc)
{
	uint32_t bufferCount = (uint32_t) ubos.size();
	uint32_t textureCount = (uint32_t)tex.size();
	uint32_t imageCount = rc->imageCount;
	if (shadow_rc != nullptr) textureCount++;

	std::vector<VkDescriptorPoolSize> poolSizes(bufferCount+textureCount);

	for (uint32_t i = 0; i < bufferCount; i++)
	{
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = imageCount;
		poolSizes[i] = poolSize;
	}
	for (uint32_t i = 0; i < textureCount; i++)
	{
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = imageCount;
		poolSizes[bufferCount+i] = poolSize;
	}

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = imageCount;

	if (vkCreateDescriptorPool(gc->device, &poolInfo, nullptr, &this->descriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}

	std::vector<VkDescriptorSetLayout> layouts(imageCount, rc->getActiveDescriptorSetLayout());
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = this->descriptorPool;
	allocInfo.descriptorSetCount = imageCount;
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(imageCount);

	if (vkAllocateDescriptorSets(gc->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (uint32_t i = 0; i < imageCount; i++)
	{
		std::vector<VkDescriptorBufferInfo> buffersInfo;
		buffersInfo.resize(bufferCount);

		for (uint32_t j = 0; j < bufferCount; j++)
		{
			buffersInfo[j] = {};
			buffersInfo[j].buffer = ubos[j]->buffers[i];
			buffersInfo[j].offset = 0;
			buffersInfo[j].range = ubos[j]->bufferSize;
		}

		std::vector<VkDescriptorImageInfo> imageInfo;
		imageInfo.resize(textureCount);

		for (uint32_t j = 0; j < tex.size(); j++)
		{
			imageInfo[j] = {};
			imageInfo[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo[j].imageView = tex[j]->textureImageView;
			imageInfo[j].sampler = tex[j]->textureSampler;
		}
		if (shadow_rc != nullptr)
		{
			uint32_t j = (uint32_t) tex.size();
			imageInfo[j] = {};
			imageInfo[j].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			imageInfo[j].imageView = shadow_rc->getDepthView(i);
			imageInfo[j].sampler = shadow_rc->getDepthSampler(i);
		}

		std::vector<VkWriteDescriptorSet> descriptorWrites;
		descriptorWrites.resize(2);

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = this->descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = (uint32_t)buffersInfo.size();
		descriptorWrites[0].pBufferInfo = buffersInfo.data();

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = this->descriptorSets[i];
		descriptorWrites[1].dstBinding = bufferCount;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;;
		descriptorWrites[1].descriptorCount = textureCount;
		descriptorWrites[1].pImageInfo = imageInfo.data();

		vkUpdateDescriptorSets(gc->device, 2, descriptorWrites.data(), 0, nullptr);
	}
}

//
// FUNCIÓN: GEDescriptorSet::destroy(GEGraphicsContext* gc) 
//
// PROPÓSITO: Destruye los conjuntos de descriptores
//
void GEDescriptorSet::destroy(GEGraphicsContext* gc) 
{
	vkFreeDescriptorSets(gc->device, descriptorPool, (uint32_t)descriptorSets.size(), descriptorSets.data());
	vkDestroyDescriptorPool(gc->device, descriptorPool, nullptr);
}
