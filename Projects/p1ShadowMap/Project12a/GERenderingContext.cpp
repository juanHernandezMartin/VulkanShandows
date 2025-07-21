#include "GERenderingContext.h"

#include "resource.h"
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////                              Métodos públicos                                   /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCIÓN: GERenderingContext::GERenderingContext()
//
// PROPÓSITO: Crea un contexto de renderizado (pipelines, renderpass y framebuffer) 
//
GERenderingContext::GERenderingContext(GEGraphicsContext* gc, GEDrawingContext* dc, GEPipelineConfig* config)
{
	imageCount = dc->getImageCount();
	format = dc->getFormat();
	extent = dc->getExtent();
	pipelineIndex = 0;
	graphicsPipeline.resize(0);
	descriptorSetLayout.resize(0);
	pipelineLayout.resize(0);
	createRenderPass(gc);
	createGraphicsPipeline(gc, config);
	createDepthBuffers(gc);
	createFramebuffers(gc, dc);
}

//
// FUNCIÓN: GERenderingContext::GERenderingContext()
//
// PROPÓSITO: Crea un contexto de renderizado (pipelines, renderpass y framebuffer) 
//
GERenderingContext::GERenderingContext(GEGraphicsContext* gc, uint32_t count, VkExtent2D ext, GEPipelineConfig* config)
{
	imageCount = count;
	format = gc->findDepthFormat(); //VK_FORMAT_D16_UNORM; 
	extent = ext;
	pipelineIndex = 0;
	graphicsPipeline.resize(0);
	descriptorSetLayout.resize(0);
	pipelineLayout.resize(0);
	createShadowRenderPass(gc);
	createGraphicsPipeline(gc, config);
	createShadowDepthBuffers(gc);
	createDepthSamplers(gc);
	createShadowFramebuffers(gc);
}

//
// FUNCIÓN: GERenderingContext::destroy(GEGraphicsContext* gc)
//
// PROPÓSITO: Crea el estado de Vulkan
//
void GERenderingContext::destroy(GEGraphicsContext* gc)
{
	for (uint32_t i = 0; i < imageCount; i++)
	{
		vkDestroyFramebuffer(gc->device, framebuffers[i], nullptr);
		depthBuffers[i]->destroy(gc);
	}
	for (uint32_t i = 0; i < graphicsPipeline.size(); i++)
	{
		vkDestroyPipeline(gc->device, graphicsPipeline[i], nullptr);
		vkDestroyPipelineLayout(gc->device, pipelineLayout[i], nullptr);
		vkDestroyDescriptorSetLayout(gc->device, descriptorSetLayout[i], nullptr);
	}
	vkDestroyRenderPass(gc->device, renderPass, nullptr);
}

//
// FUNCIÓN: GERenderingContext::createGraphicsPipeline()
//
// PROPÓSITO: Añade un nuevo Pipeline de renderizado
//
void GERenderingContext::addGraphicsPipeline(GEGraphicsContext* gc, GEPipelineConfig* config)
{
	pipelineIndex = (uint32_t) graphicsPipeline.size();
	createGraphicsPipeline(gc, config);
}

//
// FUNCIÓN: GERenderingContext::setActivePipeline(uint32_t index)
//
// PROPÓSITO: Asigna el pipeline activo
//
void GERenderingContext::setActivePipeline(uint32_t index)
{
	if (index < graphicsPipeline.size()) pipelineIndex = index;
}

//
// FUNCIÓN: GERenderingContext::getActiveDescriptorSetLayout()
//
// PROPÓSITO: Obtiene la plantilla de descriptor set del pipeline activo
//
VkDescriptorSetLayout GERenderingContext::getActiveDescriptorSetLayout()
{
	return descriptorSetLayout[pipelineIndex];
}

//
// FUNCIÓN: GERenderingContext::getActivePipelineLayout()
//
// PROPÓSITO: Obtiene la plantilla de pipeline del pipeline activo
//
VkPipelineLayout GERenderingContext::getActivePipelineLayout()
{
	return pipelineLayout[pipelineIndex];
}

//
// FUNCIÓN: GERenderingContext::insertBeginCommands(std::vector<VkCommandBuffer> commandBuffers)
//
// PROPÓSITO: Añade la inicialización del renderpass a los buffers de comandos
//
void GERenderingContext::insertBeginCommands(std::vector<VkCommandBuffer> commandBuffers, bool shadow)
{
	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		std::vector<VkClearValue> clearValues;
		
		if (shadow) 
		{
			clearValues.resize(1);
			clearValues[0].depthStencil = { 1.0f, 0 };
		}
		else
		{
			clearValues.resize(2);
			clearValues[0].color = { 1.0f, 1.0f, 1.0f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = framebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = extent;
		renderPassInfo.clearValueCount = (uint32_t) clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
}

//
// FUNCIÓN: GERenderingContext::insertPipelineCommands(std::vector<VkCommandBuffer> commandBuffers)
//
// PROPÓSITO: Añade la selección de pipeline a los buffers de comandos
//
void GERenderingContext::insertPipelineCommands(std::vector<VkCommandBuffer> commandBuffers)
{
	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline[pipelineIndex]);
	}
}

//
// FUNCIÓN: GERenderingContext::insertEndCommands(std::vector<VkCommandBuffer> commandBuffers)
//
// PROPÓSITO: Añade la finalización del renderpass a los buffers de comandos
//
void GERenderingContext::insertEndCommands(std::vector<VkCommandBuffer> commandBuffers)
{
	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		vkCmdEndRenderPass(commandBuffers[i]);
	}
}

//
// FUNCIÓN: GERenderingContext::getDepthView(uint32_t index)
//
// PROPÓSITO: Obtiene la vista del buffer de profundidad
//
VkImageView GERenderingContext::getDepthView(uint32_t index)
{
	return depthBuffers[index]->imageView;
}

//
// FUNCIÓN: GERenderingContext::getDepthSampler(uint32_t index)
//
// PROPÓSITO: Obtiene el sampler del buffer de profundidad
//
VkSampler GERenderingContext::getDepthSampler(uint32_t index)
{
	return depthSamplers[index];
}

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////                    Métodos de creación de los componentes                       /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCIÓN: GERenderingContext::createRenderPass()
//
// PROPÓSITO: Crea los pasos de renderizado
//
void GERenderingContext::createRenderPass(GEGraphicsContext* gc)
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = gc->findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	/*****/
    // Subpass dependencies for layout transitions
	std::vector<VkSubpassDependency> dependencies(2);

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	dependencies[0].dependencyFlags = 0;

	dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].dstSubpass = 0;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].srcAccessMask = 0;
	dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	dependencies[1].dependencyFlags = 0;

	/****/

	VkAttachmentDescription attachment[] = { colorAttachment, depthAttachment };

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 2;
	renderPassInfo.pAttachments = attachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
//	renderPassInfo.dependencyCount = 1;
//	renderPassInfo.pDependencies = &dependency;
	renderPassInfo.dependencyCount = (uint32_t) dependencies.size();
	renderPassInfo.pDependencies = dependencies.data();

	if (vkCreateRenderPass(gc->device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

//
// FUNCIÓN: GERenderingContext::createShadowRenderPass()
//
// PROPÓSITO: Crea los pasos de renderizado para generar el shadowmap
//
void GERenderingContext::createShadowRenderPass(GEGraphicsContext* gc)
{
	VkAttachmentDescription shadowDepthAttachment = {};
	shadowDepthAttachment.format = format;
	shadowDepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	shadowDepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							// Clear depth at beginning of the render pass
	shadowDepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;						// We will read from depth, so it's important to store the depth attachment results
	shadowDepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	shadowDepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	shadowDepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;					// We don't care about initial layout of the attachment
	shadowDepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;// Attachment will be transitioned to shader read at render pass end

	VkAttachmentReference shadowDepthReference = {};
	shadowDepthReference.attachment = 0;
	shadowDepthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;			// Attachment will be used as depth/stencil during render pass

	VkSubpassDescription shadowSubpass = {};
	shadowSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	shadowSubpass.colorAttachmentCount = 0;									// No color attachments
	shadowSubpass.pColorAttachments = nullptr;
	shadowSubpass.pDepthStencilAttachment = &shadowDepthReference;			// Reference to our depth attachment

	// Use subpass dependencies for layout transitions
	std::vector<VkSubpassDependency> shadowDependencies (2);
	shadowDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	shadowDependencies[0].dstSubpass = 0;
	shadowDependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	shadowDependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	shadowDependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	shadowDependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	shadowDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	shadowDependencies[1].srcSubpass = 0;
	shadowDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	shadowDependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	shadowDependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	shadowDependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	shadowDependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	shadowDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo shadowRenderPassCreateInfo = {};
	shadowRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	shadowRenderPassCreateInfo.attachmentCount = 1;
	shadowRenderPassCreateInfo.pAttachments = &shadowDepthAttachment;
	shadowRenderPassCreateInfo.subpassCount = 1;
	shadowRenderPassCreateInfo.pSubpasses = &shadowSubpass;
//	shadowRenderPassCreateInfo.dependencyCount = 0;
	shadowRenderPassCreateInfo.dependencyCount = (uint32_t) shadowDependencies.size();
    shadowRenderPassCreateInfo.pDependencies = shadowDependencies.data();

	if (vkCreateRenderPass(gc->device, &shadowRenderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shadow render pass!");
	}
}

//
// FUNCIÓN: GERenderingContext::createGraphicsPipeline()
//
// PROPÓSITO: Crea el Pipeline de renderizado
//
void GERenderingContext::createGraphicsPipeline(GEGraphicsContext* gc, GEPipelineConfig* config)
{
	viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) config->extent.width;
	viewport.height = (float) config->extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = config->extent;

	VkShaderModule vertShaderModule, fragShaderModule;
	VkPipelineShaderStageCreateInfo vertShaderStageInfo, fragShaderStageInfo;
	VkPipelineVertexInputStateCreateInfo vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssembly;
	VkPipelineViewportStateCreateInfo viewportState;
	VkPipelineRasterizationStateCreateInfo rasterizer;
	VkPipelineMultisampleStateCreateInfo multisampling;
	VkPipelineDepthStencilStateCreateInfo depthStencil;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlending;

	createPipelineLayout(gc, config);
	createVertexShaderStageCreateInfo(gc, config->vertex_shader, &vertShaderModule, &vertShaderStageInfo);
	createFragmentShaderStageCreateInfo(gc, config->fragment_shader, &fragShaderModule, &fragShaderStageInfo);
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
	createPipelineVertexInputStateCreateInfo(config, &vertexInputInfo);
	createPipelineInputAssemblyStateCreateInfo(&inputAssembly);
	createPipelineViewportStateCreateInfo(&viewportState);
	createPipelineRasterizationStateCreateInfo(config, &rasterizer);
	createPipelineMultisampleStateCreateInfo(&multisampling);
	createPipelineDepthStencilStateCreateInfo(config, &depthStencil);
	createPipelineColorBlendStateCreateInfo(&colorBlendAttachment, &colorBlending);

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.layout = pipelineLayout[pipelineIndex];
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	VkPipeline pipeline;
	if (vkCreateGraphicsPipelines(gc->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	graphicsPipeline.push_back(pipeline);

	vkDestroyShaderModule(gc->device, fragShaderModule, nullptr);
	vkDestroyShaderModule(gc->device, vertShaderModule, nullptr);
}

//
// FUNCIÓN: GERenderingContext::createDepthBuffers()
//
// PROPÓSITO: Crea los buffers de profundidad
//
void GERenderingContext::createDepthBuffers(GEGraphicsContext* gc)
{
	depthBuffers.resize(imageCount);
	for (size_t i = 0; i < imageCount; i++)
	{
		depthBuffers[i] = new GEDepthBuffer(gc, extent, gc->findDepthFormat(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}
}

//
// FUNCIÓN: GERenderingContext::createDepthBuffers()
//
// PROPÓSITO: Crea los buffers de profundidad
//
void GERenderingContext::createShadowDepthBuffers(GEGraphicsContext* gc)
{
	depthBuffers.resize(imageCount);
	for (size_t i = 0; i < imageCount; i++)
	{
		depthBuffers[i] = new GEDepthBuffer(gc, extent, format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	}
}

//
// FUNCIÓN: GERenderingContext::createDepthBuffers()
//
// PROPÓSITO: Crea los samplers asociados a los buffers de profundidad
//
void GERenderingContext::createDepthSamplers(GEGraphicsContext* gc)
{
	depthSamplers.resize(imageCount);
	for (size_t i = 0; i < imageCount; i++)
	{
		// Create sampler to sample from to depth attachment
		// Used to sample in the fragment shader for shadowed rendering
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 1.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		//
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

		if (vkCreateSampler(gc->device, &samplerInfo, nullptr, &depthSamplers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shadowmap sampler!");
		}
	}
}

//
// FUNCIÓN: GERenderingContext::createFramebuffers()
//
// PROPÓSITO: Crea un Framebuffer para cada imagen del swapchain
//
void GERenderingContext::createFramebuffers(GEGraphicsContext* gc, 	GEDrawingContext* dc)
{
	framebuffers.resize(imageCount);

	for (size_t i = 0; i < imageCount; i++)
	{
		VkImageView attachments[] = { dc->imageViews[i], depthBuffers[i]->imageView };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 2;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(gc->device, &framebufferInfo, nullptr, &framebuffers[i])
			!= VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

//
// FUNCIÓN: GERenderingContext::createShadowFramebuffers()
//
// PROPÓSITO: Crea un Framebuffer para cada imagen del swapchain
//
void GERenderingContext::createShadowFramebuffers(GEGraphicsContext* gc)
{
	framebuffers.resize(imageCount);

	for (size_t i = 0; i < imageCount; i++)
	{
		VkImageView attachments[] = { depthBuffers[i]->imageView };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(gc->device, &framebufferInfo, nullptr, &framebuffers[i])
			!= VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////              Métodos de definición del pipeline de renderizado                  /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCIÓN: CAVulkanState::createPipelineLayout()
//
// PROPÓSITO: Crea el esquema de los conjuntos de descriptores
//
void GERenderingContext::createPipelineLayout(GEGraphicsContext* gc, GEPipelineConfig* config)
{
	uint32_t bindingCount = (uint32_t)config->descriptorTypes.size();
	std::vector<VkDescriptorSetLayoutBinding> bindings(bindingCount);
	for (uint32_t i = 0; i < bindingCount; i++)
	{
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = i;
		binding.descriptorCount = 1;
		binding.descriptorType = config->descriptorTypes[i];
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = config->descriptorStages[i];

		bindings[i] = binding;
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = bindingCount;
	layoutInfo.pBindings = bindings.data();

	VkDescriptorSetLayout dsetLayout;
	if (vkCreateDescriptorSetLayout(gc->device, &layoutInfo, nullptr, &dsetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
	descriptorSetLayout.push_back(dsetLayout);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &dsetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	VkPipelineLayout pipLayout;
	if (vkCreatePipelineLayout(gc->device, &pipelineLayoutInfo, nullptr, &pipLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	pipelineLayout.push_back(pipLayout);
}

//
// FUNCIÓN: GERenderingContext::createVertexShaderStageCreateInfo()
//
// PROPÓSITO: Crea la información sobre el Vertex Shader
//
void GERenderingContext::createVertexShaderStageCreateInfo(GEGraphicsContext* gc, int resource, VkShaderModule* vertShaderModule, VkPipelineShaderStageCreateInfo* vertShaderStageInfo)
{
	std::vector<char> vertShaderCode = getFileFromResource(resource);

	*vertShaderModule = createShaderModule(gc, vertShaderCode);

	*vertShaderStageInfo = {};
	vertShaderStageInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo->stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo->module = *vertShaderModule;
	vertShaderStageInfo->pName = "main";
}

//
// FUNCIÓN: GERenderingContext::createFragmentShaderStageCreateInfo()
//
// PROPÓSITO: Crea la información sobre el Vertex Shader
//
void GERenderingContext::createFragmentShaderStageCreateInfo(GEGraphicsContext* gc, int resource, VkShaderModule* fragShaderModule, VkPipelineShaderStageCreateInfo* fragShaderStageInfo)
{
	std::vector<char> fragShaderCode = getFileFromResource(resource);

	*fragShaderModule = createShaderModule(gc, fragShaderCode);

	*fragShaderStageInfo = {};
	fragShaderStageInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo->stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo->module = *fragShaderModule;
	fragShaderStageInfo->pName = "main";
}

//
// FUNCIÓN: GERenderingContext::createPipelineVertexInputStateCreateInfo()
//
// PROPÓSITO: Crea la descripción de los atributos de los vértices
//
void GERenderingContext::createPipelineVertexInputStateCreateInfo(GEPipelineConfig* config, VkPipelineVertexInputStateCreateInfo* vertexInputInfo)
{
	std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

	int attrCount = (int) config->attrOffsets.size();
	int bindingCount = (attrCount > 0 ? 1 : 0);

	bindingDescriptions.resize(bindingCount);
	for (int i = 0; i < bindingCount; i++)
	{
		bindingDescriptions[i] = {};
		bindingDescriptions[i].binding = i;
		bindingDescriptions[i].stride = config->attrStride;
		bindingDescriptions[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	attributeDescriptions.resize(attrCount);
	for (int i = 0; i < attrCount; i++)
	{
		attributeDescriptions[i] = {};
		attributeDescriptions[i].binding = 0;
		attributeDescriptions[i].location = i;
		attributeDescriptions[i].format = config->attrFormats[i];
		attributeDescriptions[i].offset = config->attrOffsets[i];
	}

	*vertexInputInfo = {};
	vertexInputInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo->vertexBindingDescriptionCount = bindingCount;
	vertexInputInfo->vertexAttributeDescriptionCount = attrCount;
	vertexInputInfo->pVertexBindingDescriptions = bindingDescriptions.data();
	vertexInputInfo->pVertexAttributeDescriptions = attributeDescriptions.data();
}

//
// FUNCIÓN: GERenderingContext::createPipelineVertexInputStateCreateInfo()
//
// PROPÓSITO: Crea la información para el ensamblado de primitivas
//
void GERenderingContext::createPipelineInputAssemblyStateCreateInfo(VkPipelineInputAssemblyStateCreateInfo* inputAssembly)
{
	*inputAssembly = {};
	inputAssembly->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly->primitiveRestartEnable = VK_FALSE;
}

//
// FUNCIÓN: CAVulkanState::createPipelineViewportStateCreateInfo()
//
// PROPÓSITO: Crea la información del viewport
//
void GERenderingContext::createPipelineViewportStateCreateInfo(VkPipelineViewportStateCreateInfo* viewportState)
{
	*viewportState = {};
	viewportState->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState->viewportCount = 1;
	viewportState->pViewports = &viewport;
	viewportState->scissorCount = 1;
	viewportState->pScissors = &scissor;
}

//
// FUNCIÓN: GERenderingContext::createPipelineRasterizationStateCreateInfo()
//
// PROPÓSITO: Crea la información de la etapa de rasterización
//
void GERenderingContext::createPipelineRasterizationStateCreateInfo(GEPipelineConfig* config, VkPipelineRasterizationStateCreateInfo* rasterizer)
{
	*rasterizer = {};
	rasterizer->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer->depthClampEnable = VK_FALSE;
	rasterizer->rasterizerDiscardEnable = VK_FALSE;
	rasterizer->polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer->lineWidth = 1.0f;
	rasterizer->cullMode = config->cullMode;
	rasterizer->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer->depthBiasEnable = VK_FALSE;
}

//
// FUNCIÓN: GERenderingContext::createPipelineRasterizationStateCreateInfo()
//
// PROPÓSITO: Crea la información de la etapa de sampleado
//
void GERenderingContext::createPipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo* multisampling)
{
	*multisampling = {};
	multisampling->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling->sampleShadingEnable = VK_FALSE;
	multisampling->rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
}

//
// FUNCIÓN: GERenderingContext::createPipelineRasterizationStateCreateInfo()
//
// PROPÓSITO: Crea la información sobre los tests de profundidad y de plantilla
//
void GERenderingContext::createPipelineDepthStencilStateCreateInfo(GEPipelineConfig* config, VkPipelineDepthStencilStateCreateInfo* depthStencil)
{
	*depthStencil = {};
	depthStencil->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil->pNext = nullptr;
	depthStencil->flags = 0;
	depthStencil->depthTestEnable = config->depthTestEnable;
	depthStencil->depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencil->depthWriteEnable = VK_TRUE;
	depthStencil->depthBoundsTestEnable = VK_FALSE;
	depthStencil->stencilTestEnable = VK_FALSE;
}

//
// FUNCIÓN: GERenderingContext::createPipelineColorBlendStateCreateInfo()
//
// PROPÓSITO: Crea la información sobre la etapa de mezcla de colores
//
void GERenderingContext::createPipelineColorBlendStateCreateInfo(VkPipelineColorBlendAttachmentState* colorBlendAttachment, VkPipelineColorBlendStateCreateInfo* colorBlending)
{
	*colorBlendAttachment = {};
	colorBlendAttachment->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment->blendEnable = VK_FALSE;

	*colorBlending = {};
	colorBlending->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending->logicOpEnable = VK_FALSE;
	colorBlending->logicOp = VK_LOGIC_OP_COPY;
	colorBlending->attachmentCount = 1;
	colorBlending->pAttachments = colorBlendAttachment;
	colorBlending->blendConstants[0] = 0.0f;
	colorBlending->blendConstants[1] = 0.0f;
	colorBlending->blendConstants[2] = 0.0f;
	colorBlending->blendConstants[3] = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////                              Métodos auxiliares                                 /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCIÓN: GERenderingContext::createShaderModule()
//
// PROPÓSITO: Crea un shader a partir de su código en SPIR-V
//
VkShaderModule GERenderingContext::createShaderModule(GEGraphicsContext* gc, const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(gc->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

//
// FUNCIÓN: GERenderingContext::getFileFromResource(int resource)
//
// PROPÓSITO: Extrae el contenido de un fichero incluido como recurso de la aplicación
//
std::vector<char> GERenderingContext::getFileFromResource(int resource)
{
	HRSRC shaderHandle = FindResource(NULL, MAKEINTRESOURCE(resource), RT_HTML);
	HGLOBAL shaderGlobal = LoadResource(NULL, shaderHandle);
	LPCTSTR shaderPtr = static_cast<LPCTSTR>(LockResource(shaderGlobal));
	DWORD shaderSize = SizeofResource(NULL, shaderHandle);

	std::vector<char> shader(shaderSize);
	memcpy(shader.data(), shaderPtr, shaderSize);
	UnlockResource(shaderGlobal);
	FreeResource(shaderGlobal);
	return shader;
}