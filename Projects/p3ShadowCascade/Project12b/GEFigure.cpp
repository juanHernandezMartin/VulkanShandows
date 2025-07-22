#include "GEFigure.h"

#include "GEVertex.h"
#include "GETransform.h"
#include "GEMaterial.h"
#include "GELight.h"
#include "GEShadowTransform.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

//
// FUNCI�N: GEFigure::initialize(GEGraphicsContext* gc, GERenderingContext* rc)
//
// PROP�SITO: Crea la figura
//
void GEFigure::initialize(GEGraphicsContext* gc, GERenderingContext* rc, GERenderingContext* shadow_rc)
{
	size_t vertexSize = sizeof(GEVertex) * vertices.size();
	vbo = new GEVertexBuffer(gc, vertexSize, vertices.data());

	size_t indexSize = sizeof(indices[0]) * indices.size();
	ibo = new GEIndexBuffer(gc, indexSize, indices.data());

	size_t transformBufferSize = sizeof(GETransform);
	transformBuffer = new GEUniformBuffer(gc, rc->imageCount, transformBufferSize);

	size_t materialBufferSize = sizeof(GEMaterial);
	materialBuffer = new GEUniformBuffer(gc, rc->imageCount, materialBufferSize);

	size_t lightBufferSize = sizeof(GELight);
	lightBuffer = new GEUniformBuffer(gc, rc->imageCount, lightBufferSize);

	std::vector<GEUniformBuffer*> ubos(3);
	ubos[0] = transformBuffer;
	ubos[1] = materialBuffer;
	ubos[2] = lightBuffer;

	std::vector<GETexture*> tex(1);
	tex[0] = texture;
	dset = new GEDescriptorSet(gc, rc, ubos, tex, shadow_rc);

	size_t shadowTransformBufferSize = sizeof(GEShadowTransform);
	shadowTransformBuffer = new GEUniformBuffer(gc, rc->imageCount, shadowTransformBufferSize);

	std::vector<GEUniformBuffer*> shadow_ubos(1);
	shadow_ubos[0] = shadowTransformBuffer;

	std::vector<GETexture*> shadow_tex(0);

	shadowDset = new GEDescriptorSet(gc, shadow_rc, shadow_ubos, shadow_tex, nullptr);

	location = glm::mat4(1.0f);
}

//
// FUNCI�N: GEFigure::destroy(GEGraphicsContext* gc)
//
// PROP�SITO: Libera los buffers de la figura
//
void GEFigure::destroy(GEGraphicsContext* gc)
{
	vbo->destroy(gc);
	ibo->destroy(gc);
	transformBuffer->destroy(gc);
	materialBuffer->destroy(gc);
	lightBuffer->destroy(gc);
	shadowTransformBuffer->destroy(gc);
	dset->destroy(gc);
	shadowDset->destroy(gc);

	delete vbo;
	delete ibo;
	delete transformBuffer;
	delete materialBuffer;
	delete lightBuffer;
	delete shadowTransformBuffer;
	delete dset;
	delete shadowDset;
}

//
// FUNCI�N: CAFigure::addCommands(VkCommandBuffer commandBuffer, int index)
//
// PROP�SITO: A�ade los comandos de renderizado al command buffer
//
void GEFigure::addCommands(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &(vbo->buffer), &offset);
	vkCmdBindIndexBuffer(commandBuffer, ibo->buffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &(dset->descriptorSets[index]), 0, nullptr);
	vkCmdDrawIndexed(commandBuffer, (uint32_t)indices.size(), 1, 0, 0, 0);
}

//
// FUNCI�N: CAFigure::addShadowCommands(VkCommandBuffer commandBuffer, int index)
//
// PROP�SITO: A�ade los comandos de renderizado al command buffer
//
void GEFigure::addShadowCommands(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &(vbo->buffer), &offset);
	vkCmdBindIndexBuffer(commandBuffer, ibo->buffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &(shadowDset->descriptorSets[index]), 0, nullptr);
	vkCmdDrawIndexed(commandBuffer, (uint32_t)indices.size(), 1, 0, 0, 0);
}

//
// FUNCI�N: GEFigure::update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection)
//
// PROP�SITO: Actualiza las variables uniformes sobre una imagen del swapchain
//
void GEFigure::update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection, glm::mat4 lightView)
{
	GETransform transform;
	transform.MVP = projection * view * location;
	transform.ModelViewMatrix = view * location;
	transform.ViewMatrix = view;
	transform.ShadowMatrix = lightView * location;

	transformBuffer->update(gc, index, sizeof(GETransform), &transform);
	materialBuffer->update(gc, index, sizeof(GEMaterial), &material);
	lightBuffer->update(gc, index, sizeof(GELight), &light);
}

//
// FUNCI�N: GEFigure::updateShadow(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection)
//
// PROP�SITO: Actualiza las variables uniformes sobre una imagen del swapchain
//
void GEFigure::updateShadow(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection)
{
	GEShadowTransform transform;
	transform.MVP = projection * view * location;

	shadowTransformBuffer->update(gc, index, sizeof(GEShadowTransform), &transform);
}

//
// FUNCI�N: GEFigure::resetLocation()
//
// PROP�SITO: Resetea la matriz de localizaci�n (Model).
//
void GEFigure::resetLocation()
{
	location = glm::mat4(1.0f);
}

//
// FUNCI�N: GEFigure::setLocation()
//
// PROP�SITO: Resetea la matriz de localizaci�n (Model).
//
void GEFigure::setLocation(glm::mat4 m)
{
	location = glm::mat4(m);
}

//
// FUNCI�N: GEFigure::translate(glm::vec3 t)
//
// PROP�SITO: A�ade un desplazamiento la matriz de localizaci�n (Model).
//
void GEFigure::translate(glm::vec3 t)
{
	location = glm::translate(location, t);
}

//
// FUNCI�N: GEFigure::rotate(float angle, glm::vec3 axis)
//
// PROP�SITO: A�ade una rotaci�n la matriz de localizaci�n (Model).
//
void GEFigure::rotate(float angle, glm::vec3 axis)
{
	location = glm::rotate(location, glm::radians(angle), axis);
}

//
// FUNCI�N: GEFigure::setMaterial(GEMaterial m)
//
// PROP�SITO: Asigna las propiedades del material del que est� formada la figura
//
void GEFigure::setMaterial(GEMaterial m)
{
	this->material = m;
}

//
// FUNCI�N: GEFigure::setLight(GELight l)
//
// PROP�SITO: Asigna las propiedades de la luz que ilumina la figura
//
void GEFigure::setLight(GELight l)
{
	this->light = l;
}

//
// FUNCI�N: GEFigure::setTexture(GETexture* tex)
//
// PROP�SITO: Asigna la textura
//
void GEFigure::setTexture(GETexture* tex)
{
	this->texture = tex;
}