#include "GESkybox.h"

#include "GESkyboxVertex.h"
#include "GESkyboxTransform.h"
#include "GETexture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//
// FUNCI�N: GESkybox::CASkybox(GEGraphicsContext* gc, GERenderingContext* rc)
//
// PROP�SITO: Crea la imagen de contorno
//
GESkybox::GESkybox(GEGraphicsContext* gc, GERenderingContext* rc)
{
	vertices = {
		{{-1.0f, -1.0f, 1.0f}},
		{{1.0f, -1.0f, 1.0f}},
		{{1.0f, 1.0f, 1.0f}},
		{{-1.0f, 1.0f, 1.0f}}
	};

	indices = {
		0,1,2,
		0,2,3
	};


	size_t vertexSize = sizeof(GESkyboxVertex) * vertices.size();
	vbo = new GEVertexBuffer(gc, vertexSize, vertices.data());

	size_t indexSize = sizeof(indices[0]) * indices.size();
	ibo = new GEIndexBuffer(gc, indexSize, indices.data());

	size_t transformBufferSize = sizeof(GESkyboxTransform);
	transformBuffer = new GEUniformBuffer(gc, rc->imageCount, transformBufferSize);

	const char* filenames[6];
	filenames[0] = "textures/negz.jpg";
	filenames[1] = "textures/posz.jpg";
	filenames[2] = "textures/posy.jpg";
	filenames[3] = "textures/negy.jpg";
	filenames[4] = "textures/posx.jpg";
	filenames[5] = "textures/negx.jpg";

	cubemap = new GETexture(gc, filenames);

	std::vector<GEUniformBuffer*> ubos(1);
	ubos[0] = transformBuffer;

	std::vector<GETexture*> tex(1);
	tex[0] = cubemap;

	dset = new GEDescriptorSet(gc, rc, ubos, tex, nullptr);
}

//
// FUNCI�N: GESkybox::destroy(GEGraphicsContext* gc)
//
// PROP�SITO: Libera los buffers de la figura
//
void GESkybox::destroy(GEGraphicsContext* gc)
{
	vbo->destroy(gc);
	ibo->destroy(gc);
	transformBuffer->destroy(gc);
	cubemap->destroy(gc);
	dset->destroy(gc);

	delete vbo;
	delete ibo;
	delete transformBuffer;
	delete cubemap;
	delete dset;
}

//
// FUNCI�N: GESkybox::addCommands(VkCommandBuffer commandBuffer, int index)
//
// PROP�SITO: A�ade los comandos de renderizado al command buffer
//
void GESkybox::addCommands(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &(vbo->buffer), &offset);
	vkCmdBindIndexBuffer(commandBuffer, ibo->buffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &(dset->descriptorSets[index]), 0, nullptr);
	vkCmdDrawIndexed(commandBuffer, (uint32_t)indices.size(), 1, 0, 0, 0);
}

//
// FUNCI�N: GESkybox::update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection)
//
// PROP�SITO: Actualiza las variables uniformes sobre una imagen del swapchain
//
void GESkybox::update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection)
{
	glm::mat3 rot3 = glm::mat3(view); // Parte rotacional de la matriz View
	glm::mat4 rot4 = glm::mat4(rot3);
	glm::mat4 mvp = projection * rot4; // Transformaci�n del Skybox a coordenadas Clip
	glm::mat4 inv = glm::inverse(mvp); // Transformaci�n de coordenadas Clip a coordenadas de modelo del Skybox

	GESkyboxTransform ubo = {};
	ubo.inverse =  inv ;

	transformBuffer->update(gc, index, sizeof(GESkyboxTransform), &ubo);
}
