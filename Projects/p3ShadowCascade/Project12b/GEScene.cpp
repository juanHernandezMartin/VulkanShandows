#include "GEScene.h"

#include "GECube.h"
#include "GEPyramid.h"
#include "GECone.h"
#include "GECylinder.h"
#include "GESphere.h"
#include "GEDisk.h"
#include "GETorus.h"
#include "GEIcosahedron.h"
#include "GEGround.h"
#include "GETransform.h"
#include "GEMaterial.h"
#include "GELight.h"
#include <windows.h>
#include "resource.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>


#define SKYBOX_PIPELINE 0
#define SCENE_PIPELINE 1
#define SHADOW_PIPELINE 0

//
// FUNCIÓN: GEScene::GEScene(GEGraphicsContext* gc, GEDrawingContext* dc)
//
// PROPÓSITO: Crea la escena
//
GEScene::GEScene(GEGraphicsContext* gc, GEDrawingContext* dc, GECommandContext* cc)
{
	VkExtent2D extent = dc->getExtent();
	double aspect = (double)extent.width / (double)extent.height;
	aspect_ratio(aspect);

	GEPipelineConfig* skybox_config = createSkyboxPipelineConfig(dc->getExtent());
	GEPipelineConfig* scene_config = createScenePipelineConfig(dc->getExtent());
	rc = new GERenderingContext(gc, dc, skybox_config);
	rc->addGraphicsPipeline(gc, scene_config);

	VkExtent2D shadowExtent = { 1024, 1024 };
	GEPipelineConfig* shadow_config = createShadowPipelineConfig(shadowExtent);
	shadow_rc = new GERenderingContext(gc, dc->getImageCount(), shadowExtent, shadow_config);

	this->camera = new GECamera();
	this->camera->setPosition(glm::vec3(0.0f, 20.0f, 100.0f));
	this->camera->setMoveStep(0.0f);

	rc->setActivePipeline(SKYBOX_PIPELINE);
	this->skybox = new GESkybox(gc,rc);

	rc->setActivePipeline(SCENE_PIPELINE);

	textures.resize(3);
	textures[0] = new GETexture(gc, "textures/stone.jpg");
	textures[1] = new GETexture(gc, "textures/moon.jpg");
	textures[2] = new GETexture(gc, "textures/wood.jpg");

	GELight light = {};
	light.Ldir = glm::normalize(glm::vec3(1.0f, -0.8f, -0.7f));
	light.La = glm::vec3(0.1f, 0.1f, 0.1f);
	light.Ld = glm::vec3(0.9f, 0.9f, 0.9f);
	light.Ls = glm::vec3(1.0f, 1.0f, 1.0f);

	GEMaterial groundMat = {};
	groundMat.Ka = glm::vec3(1.0f, 1.0f, 1.0f);
	groundMat.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
	groundMat.Ks = glm::vec3(0.8f, 0.8f, 0.8f);
	groundMat.Shininess = 16.0f;

	GEFigure* ground = new GEGround(50.0f, 50.0f);
	ground->setTexture(textures[2]);
	ground->initialize(gc, rc, shadow_rc);
	ground->setMaterial(groundMat);
	ground->setLight(light);

	GEMaterial fig1Mat = {};
	fig1Mat.Ka = glm::vec3(1.0f, 0.0f, 1.0f);
	fig1Mat.Kd = glm::vec3(1.0f, 0.0f, 1.0f);
	fig1Mat.Ks = glm::vec3(0.8f, 0.8f, 0.8f);
	fig1Mat.Shininess = 16.0f;

	GEFigure* fig1 = new GECone(5, 20, 8.0f, 5.0f);
	fig1->setTexture(textures[0]);
	fig1->initialize(gc, rc, shadow_rc);
	fig1->translate(glm::vec3(25.0f, 8.0f, 25.0f));
	fig1->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	fig1->setMaterial(fig1Mat);
	fig1->setLight(light);

	GEMaterial fig2Mat = {};
	fig2Mat.Ka = glm::vec3(1.0f, 1.0f, 1.0f);
	fig2Mat.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
	fig2Mat.Ks = glm::vec3(0.8f, 0.8f, 0.8f);
	fig2Mat.Shininess = 16.0f;

	GEFigure* fig2 = new GECube(6.0f);
	fig2->setTexture(textures[0]);
	fig2->initialize(gc, rc, shadow_rc);
	fig2->translate(glm::vec3(-25.0f, 6.0f, 25.0f));
	fig2->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	fig2->setMaterial(fig2Mat);
	fig2->setLight(light);

	GEMaterial fig3Mat = {};
	fig3Mat.Ka = glm::vec3(0.0f, 0.0f, 1.0f);
	fig3Mat.Kd = glm::vec3(0.0f, 0.0f, 1.0f);
	fig3Mat.Ks = glm::vec3(0.8f, 0.8f, 0.8f);
	fig3Mat.Shininess = 16.0f;

	GEFigure* fig3 = new GECylinder(20, 20, 5.0f, 8.0f);
	fig3->setTexture(textures[0]);
	fig3->initialize(gc, rc, shadow_rc);
	fig3->translate(glm::vec3(25.0f, 8.0f, 0.0f));
	fig3->rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	fig3->setMaterial(fig3Mat);
	fig3->setLight(light);

	GEMaterial fig4Mat = {};
	fig4Mat.Ka = glm::vec3(1.0f, 0.0f, 0.0f);
	fig4Mat.Kd = glm::vec3(1.0f, 0.0f, 0.0f);
	fig4Mat.Ks = glm::vec3(0.8f, 0.8f, 0.8f);
	fig4Mat.Shininess = 16.0f;

	GEFigure* fig4 = new GETorus(20, 40, 3.0f, 5.0f);
	fig4->setTexture(textures[0]);
	fig4->initialize(gc, rc, shadow_rc);
	fig4->translate(glm::vec3(-25.0f, 8.0f, 0.0f));
	fig4->setMaterial(fig4Mat);
	fig4->setLight(light);

	GEMaterial fig5Mat = {};
	fig5Mat.Ka = glm::vec3(1.0f, 1.0f, 1.0f);
	fig5Mat.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
	fig5Mat.Ks = glm::vec3(0.8f, 0.8f, 0.8f);
	fig5Mat.Shininess = 16.0f;

	GEFigure* fig5 = new GESphere(20, 40, 8.0f);
	fig5->setTexture(textures[1]);
	fig5->initialize(gc, rc, shadow_rc);
	fig5->translate(glm::vec3(25.0f, 8.0f, -25.0f));
	fig5->setMaterial(fig5Mat);
	fig5->setLight(light);

	GEMaterial fig6Mat = {};
	fig6Mat.Ka = glm::vec3(0.0f, 1.0f, 1.0f);
	fig6Mat.Kd = glm::vec3(0.0f, 1.0f, 1.0f);
	fig6Mat.Ks = glm::vec3(0.8f, 0.8f, 0.8f);
	fig6Mat.Shininess = 16.0f;

	GEFigure* fig6 = new GEIcosahedron(5.0f);
	fig6->setTexture(textures[0]);
	fig6->initialize(gc, rc, shadow_rc);
	fig6->translate(glm::vec3(-25.0f, 8.0f, -25.0f));
	fig6->setMaterial(fig6Mat);
	fig6->setLight(light);

	figures.resize(7);
	figures[0] = ground;
	figures[1] = fig1;
	figures[2] = fig2;
	figures[3] = fig3;
	figures[4] = fig4;
	figures[5] = fig5;
	figures[6] = fig6;

	fillCommandBuffers(cc);
}

//
// FUNCIÓN: GEScene::destroy(GEGraphicsContext* gc)
//
// PROPÓSITO: Reconstruye los componentes gráficos de la escena
//
void GEScene::destroy(GEGraphicsContext* gc)
{
	rc->destroy(gc);
	delete rc;

	shadow_rc->destroy(gc);
	delete shadow_rc;

	skybox->destroy(gc);
	delete skybox;

	for (int i = 0; i < figures.size(); i++)
	{
		figures[i]->destroy(gc);
		delete figures[i];
	}

	for (int i = 0; i < textures.size(); i++)
	{
		textures[i]->destroy(gc);
		delete textures[i];
	}
}

//
// FUNCIÓN: GEScene::recreate(GEGraphicsContext* gc, GEDrawingContext* dc)
//
// PROPÓSITO: Reconstruye los componentes gráficos de la escena
//
void GEScene::recreate(GEGraphicsContext* gc, GEDrawingContext* dc, GECommandContext* cc)
{
	rc->destroy(gc);
	GEPipelineConfig* skybox_config = createSkyboxPipelineConfig(dc->getExtent());
	GEPipelineConfig* scene_config = createScenePipelineConfig(dc->getExtent());
	this->rc = new GERenderingContext(gc, dc, skybox_config);
	this->rc->addGraphicsPipeline(gc, scene_config);

	VkExtent2D shadowExtent = { 1024, 1024 };
	GEPipelineConfig* shadow_config = createShadowPipelineConfig(shadowExtent);
	shadow_rc = new GERenderingContext(gc, dc->getImageCount(), shadowExtent, shadow_config);

	fillCommandBuffers(cc);
}

//
// FUNCIÓN: GEScene::update(GEGraphicsContext* gc, uint32_t index)
//
// PROPÓSITO: Actualiza la información para generar la imagen 
//
void GEScene::update(GEGraphicsContext* gc, uint32_t index)
{
	calculateDeltaTime();
	camera->update(deltaTime);
	glm::mat4 view = camera->getViewMatrix();

	skybox->update(gc, index, view, projection);

	glm::mat4 lightView = getLightViewMatrix();
	glm::mat4 lightPerspective = glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, -300.0f, 300.0f);
	lightPerspective[1][1] *= -1.0f;

	glm::mat4 lightVP = lightPerspective * lightView;
	for (int i = 0; i < figures.size(); i++)
	{
		figures[i]->updateShadow(gc, index, lightView, lightPerspective);
		figures[i]->update(gc, index, view, projection, lightVP);
	}
}

//
// FUNCIÓN: GEScene::key_action(int key, bool pressed)
//
// PROPÓSITO: Respuesta a acciones de teclado
//
void GEScene::key_action(int key, bool pressed)
{
	keyStates[key] = pressed;

	// Restablecer movimiento antes de aplicar cambios
	camera->cameraMoveDirection = glm::vec3(0.0f);

	// Aplicar movimiento en función de teclas activas
	if (keyStates[GLFW_KEY_W]) camera->cameraMoveDirection.z = 1.0f;
	if (keyStates[GLFW_KEY_S]) camera->cameraMoveDirection.z = -1.0f;
	if (keyStates[GLFW_KEY_A]) camera->cameraMoveDirection.x = 1.0f;
	if (keyStates[GLFW_KEY_D]) camera->cameraMoveDirection.x = -1.0f;
	if (keyStates[GLFW_KEY_Q]) camera->cameraMoveDirection.y = 1.0f;
	if (keyStates[GLFW_KEY_E]) camera->cameraMoveDirection.y = -1.0f;

	if (keyStates[GLFW_KEY_UP]) camera->turnDown();
	if (keyStates[GLFW_KEY_DOWN]) camera->turnUp();
	if (keyStates[GLFW_KEY_LEFT]) camera->turnLeft();
	if (keyStates[GLFW_KEY_RIGHT]) camera->turnRight();
}

//
// FUNCIÓN: GEScene::aspect_ratio(double)
//
// PROPÓSITO: Modifica la relación anchura/altura del modelo
//
void GEScene::aspect_ratio(double aspect)
{
	constexpr double fov = glm::radians(30.0f);
	double sin_fov = sin(fov);
	double cos_fov = cos(fov);
	float wHeight = (float)(sin_fov * 0.2 / cos_fov);
	float wWidth = (float)(wHeight * aspect);

	projection = glm::perspective((float)fov, (float)aspect, 0.2f, 400.0f);
	projection[1][1] *= -1.0f;
}

//
// FUNCIÓN: CAScene::fillCommandBuffers(GECommandContext* cc)
//
// PROPÓSITO: Añade los comandos de renderizado al command buffer
//
void GEScene::fillCommandBuffers(GECommandContext* cc)
{
	cc->beginCommandBuffers();

	/* Renderpass del shadowmap */

	shadow_rc->insertBeginCommands(cc->commandBuffers, true);

	shadow_rc->setActivePipeline(SHADOW_PIPELINE);
	shadow_rc->insertPipelineCommands(cc->commandBuffers);
	for (int i = 0; i < cc->commandBuffers.size(); i++)
	{
		for (int j = 0; j < figures.size(); j++)
		{
			figures[j]->addShadowCommands(cc->commandBuffers[i], shadow_rc->getActivePipelineLayout(), i);
		}
	}

	shadow_rc->insertEndCommands(cc->commandBuffers);
	
	/* Fin del renderpass del shadowmap */

	/* Renderpass del skybox y la escena */

	rc->insertBeginCommands(cc->commandBuffers, false);

	rc->setActivePipeline(SKYBOX_PIPELINE);
	rc->insertPipelineCommands(cc->commandBuffers);
	for (int i = 0; i < cc->commandBuffers.size(); i++)
	{
		skybox->addCommands(cc->commandBuffers[i], rc->getActivePipelineLayout(), i);
	}

	rc->setActivePipeline(SCENE_PIPELINE);
	rc->insertPipelineCommands(cc->commandBuffers);
	for (int i = 0; i < cc->commandBuffers.size(); i++)
	{
		for (int j = 0; j < figures.size(); j++)
		{
			figures[j]->addCommands(cc->commandBuffers[i], rc->getActivePipelineLayout(), i);
		}
	}

	rc->insertEndCommands(cc->commandBuffers);

	/* Fin del renderpass del skybox y la escena */

	cc->endCommandBuffers();
}

//
// FUNCIÓN: GEScene::createSkyboxPipelineConfig()
//
// PROPÓSITO: Obtiene la configuración del pipeline de renderizado para el skybox
//
GEPipelineConfig* GEScene::createSkyboxPipelineConfig(VkExtent2D extent)
{
	GEPipelineConfig* config = new GEPipelineConfig();
	config->vertex_shader = IDR_HTML1;
	config->fragment_shader = IDR_HTML2;

	config->attrStride = sizeof(GESkyboxVertex);
	config->attrOffsets.resize(1);
	config->attrOffsets[0] = offsetof(GESkyboxVertex, pos);
	config->attrFormats.resize(1);
	config->attrFormats[0] = VK_FORMAT_R32G32B32_SFLOAT;

	config->descriptorTypes.resize(2);
	config->descriptorTypes[0] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	config->descriptorTypes[1] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	config->descriptorStages.resize(2);
	config->descriptorStages[0] = VK_SHADER_STAGE_VERTEX_BIT;
	config->descriptorStages[1] = VK_SHADER_STAGE_FRAGMENT_BIT;

	config->depthTestEnable = VK_FALSE;
	config->cullMode = VK_CULL_MODE_NONE;
	config->extent = extent;

	return config;
}

//
// FUNCIÓN: GEScene::createScenePipelineConfig()
//
// PROPÓSITO: Obtiene la configuración del pipeline de renderizado para las figuras
//
GEPipelineConfig* GEScene::createScenePipelineConfig(VkExtent2D extent)
{
	GEPipelineConfig* config = new GEPipelineConfig();
	config->vertex_shader = IDR_HTML3;
	config->fragment_shader = IDR_HTML4;

	config->attrStride = sizeof(GEVertex);
	config->attrOffsets.resize(3);
	config->attrOffsets[0] = offsetof(GEVertex, pos);
	config->attrOffsets[1] = offsetof(GEVertex, norm);
	config->attrOffsets[2] = offsetof(GEVertex, tex);
	config->attrFormats.resize(3);
	config->attrFormats[0] = VK_FORMAT_R32G32B32_SFLOAT;
	config->attrFormats[1] = VK_FORMAT_R32G32B32_SFLOAT;
	config->attrFormats[2] = VK_FORMAT_R32G32_SFLOAT;

	config->descriptorTypes.resize(5);
	config->descriptorTypes[0] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	config->descriptorTypes[1] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	config->descriptorTypes[2] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	config->descriptorTypes[3] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	config->descriptorTypes[4] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	config->descriptorStages.resize(5);
	config->descriptorStages[0] = VK_SHADER_STAGE_ALL_GRAPHICS;
	config->descriptorStages[1] = VK_SHADER_STAGE_FRAGMENT_BIT;
	config->descriptorStages[2] = VK_SHADER_STAGE_FRAGMENT_BIT;
	config->descriptorStages[3] = VK_SHADER_STAGE_FRAGMENT_BIT;
	config->descriptorStages[4] = VK_SHADER_STAGE_FRAGMENT_BIT;

	config->depthTestEnable = VK_TRUE;
	config->cullMode = VK_CULL_MODE_BACK_BIT;
	config->extent = extent;

	return config;
}

//
// FUNCIÓN: GEScene::createShadowPipelineConfig()
//
// PROPÓSITO: Obtiene la configuración del pipeline de renderizado para el shadowmap
//
GEPipelineConfig* GEScene::createShadowPipelineConfig(VkExtent2D extent)
{
	GEPipelineConfig* config = new GEPipelineConfig();
	config->vertex_shader = IDR_HTML5;
	config->fragment_shader = IDR_HTML6;

	config->attrStride = sizeof(GEVertex);
	config->attrOffsets.resize(3);
	config->attrOffsets[0] = offsetof(GEVertex, pos);
	config->attrOffsets[1] = offsetof(GEVertex, norm);
	config->attrOffsets[2] = offsetof(GEVertex, tex);
	config->attrFormats.resize(3);
	config->attrFormats[0] = VK_FORMAT_R32G32B32_SFLOAT;
	config->attrFormats[1] = VK_FORMAT_R32G32B32_SFLOAT;
	config->attrFormats[2] = VK_FORMAT_R32G32_SFLOAT;

	config->descriptorTypes.resize(1);
	config->descriptorTypes[0] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	config->descriptorStages.resize(1);
	config->descriptorStages[0] = VK_SHADER_STAGE_VERTEX_BIT;


	config->depthTestEnable = VK_TRUE;
	config->cullMode = VK_CULL_MODE_FRONT_BIT;
	config->extent = extent;

	return config;
}

//
// FUNCIÓN: GEGcene::getLightViewMatrix()
//
// PROPÓSITO: Obtiene la matriz de posicionamiento de la luz
//
glm::mat4 GEScene::getLightViewMatrix()
{
	glm::vec3 Ldir = glm::normalize(glm::vec3(1.0f, -0.8f, -0.7f));

	glm::vec3 Zdir = -Ldir;
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 Xdir = glm::normalize(glm::cross(Up, Zdir));
	glm::vec3 Ydir = glm::normalize(glm::cross(Zdir, Xdir));
	glm::vec3 Zpos = 100.0f * Zdir;
	glm::vec3 Center = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::mat4 view = glm::lookAt(Zpos, Center, Ydir);
	return view;
}


void GEScene::calculateDeltaTime()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
	lastTime = currentTime;
}