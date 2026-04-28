#pragma once

#include "GEGraphicsContext.h"
#include "GERenderingContext.h"
#include "GEVertex.h"
#include "GETransform.h"
#include "GEMaterial.h"
#include "GELight.h"
#include "GEVertexBuffer.h"
#include "GEIndexBuffer.h"
#include "GEUniformBuffer.h"
#include "GEDescriptorSet.h"
#include "GETexture.h"
#include <glm/glm.hpp>
#include <vector>

//
// CLASE: GEFigure
//
// DESCRIPCIÓN: Clase que describe una figura formada por una malla de vértices
//
class GEFigure
{
protected:
	std::vector<GEVertex> vertices;
	std::vector<uint16_t> indices;
	glm::mat4 location;
	GEMaterial material;
	GELight light;
	GETexture* texture;

public:
	void initialize(GEGraphicsContext* gc, GERenderingContext* rc, GERenderingContext* shadow_rc1, GERenderingContext* shadow_rc2, GERenderingContext* shadow_rc3);
	void destroy(GEGraphicsContext* gc);
	void addCommands(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index);
	void addShadowCommands1(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index);
	void addShadowCommands2(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index);
	void addShadowCommands3(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index);
	void update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection, glm::mat4 lightView1, glm::mat4 lightView2, glm::mat4 lightView3);
	void updateShadow(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection1, glm::mat4 projection2, glm::mat4 projection3);
	void resetLocation();
	void setLocation(glm::mat4 m);
	void translate(glm::vec3 t);
	void rotate(float angle, glm::vec3 axis);
	void setMaterial(GEMaterial m);
	void setLight(GELight l);
	void setTexture(GETexture* texture);

private:
	GEVertexBuffer* vbo;
	GEIndexBuffer* ibo;
	GEUniformBuffer* transformBuffer;
	GEUniformBuffer* materialBuffer;
	GEUniformBuffer* lightBuffer;
	GEUniformBuffer* shadowTransformBuffer1;
	GEUniformBuffer* shadowTransformBuffer2;
	GEUniformBuffer* shadowTransformBuffer3;
	GEDescriptorSet* dset;
	GEDescriptorSet* shadowDset1;
	GEDescriptorSet* shadowDset2;
	GEDescriptorSet* shadowDset3;
};

