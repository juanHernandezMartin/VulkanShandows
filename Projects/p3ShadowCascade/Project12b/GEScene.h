#pragma once

#include "GEGraphicsContext.h"
#include "GEDrawingContext.h"
#include "GECommandContext.h"
#include "GERenderingContext.h"

#include "GESkybox.h"
#include "GEFigure.h"
#include "GETexture.h"
#include "GECamera.h"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

//
// CLASE: GEScene
//
// DESCRIPCIÓN: Clase que describe una escena
//
class GEScene
{
private:
	GERenderingContext* rc;
	GERenderingContext* shadow_rc;
	GESkybox* skybox;
	std::vector<GEFigure*> figures;
	std::vector<GETexture*> textures;
	GECamera* camera;
	glm::mat4 projection;
	std::unordered_map<int, bool> keyStates;
	float deltaTime;
	std::chrono::steady_clock::time_point lastTime;

public:
	GEScene(GEGraphicsContext* gc, GEDrawingContext* dc, GECommandContext* cc);
	void destroy(GEGraphicsContext* gc);
	void recreate(GEGraphicsContext* gc, GEDrawingContext* dc, GECommandContext* cc);
	void update(GEGraphicsContext* gc, uint32_t index);
	void key_action(int key, bool pressed);
	void aspect_ratio(double aspect);
	void calculateDeltaTime();

private:
	void fillCommandBuffers(GECommandContext* cc);
	GEPipelineConfig* createSkyboxPipelineConfig(VkExtent2D extent);
	GEPipelineConfig* createScenePipelineConfig(VkExtent2D extent);
	GEPipelineConfig* createShadowPipelineConfig(VkExtent2D extent);
	glm::mat4 getLightViewMatrix();
};

