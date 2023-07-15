// !! Headers in required order !!

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <vulkan/vulkan.h>

// #define DISABLE_ERROR_LOGGING
// #define DISABLE_FINISH_LOGGING
// #define DISABLE_LOG_LOGGING
// #define DISABLE_PRINT_LOGGING

#include "util/logging.h"
#include "util/util.h"
#include "util/profiling.h"

#include "impl/vulkan_setup.h"

const char* const enabledLayerArray[] = 
{
    "VK_LAYER_KHRONOS_validation",
};
const uint32_t enabledLayerCount = (uint32_t) (sizeof(enabledLayerArray) / sizeof(char*));

const char* const enabledExtensionArray[] = 
{

};
const uint32_t enabledExtensionCount = (uint32_t) (sizeof(enabledExtensionArray) / sizeof(char*));

static void create_descriptor_set(VkDescriptorSet* descriptorSet,
								  VkDevice device,
								  VkDescriptorPool descriptorPool,
								  VkDescriptorSetLayout descriptorSetLayout)
{
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &descriptorSetLayout,
	};

	vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet);
}

int main()
{
	// ################################################################################
	// Setup vulkan

	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue queue;
	VkCommandPool commandPool;
	
	setup_vulkan(enabledLayerArray, enabledLayerCount, enabledExtensionArray, enabledExtensionCount, &device, &queue, &commandPool);


	VkResult result;

	// ################################################################################
	// Create VkShaderModule

	VkShaderModule shaderModule;
	{
		const char* filePath = "obj/computeshader.spv";

		UtilFile utilFile;

		utilfile_create(&utilFile, filePath);

		VkShaderModuleCreateInfo shaderModuleCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = utilFile.size,
			.pCode = (uint32_t*)utilFile.data,
		};

		result = vkCreateShaderModule(device, &shaderModuleCreateInfo, NULL, &shaderModule);
		if (result != VK_SUCCESS) error("Problem at vkCreateShaderModule! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING

		utilfile_destroy(&utilFile);
	}

	// ################################################################################
	// Create VkBuffer

	VkBuffer buffer;
	{
		VkBufferCreateInfo bufferInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = 10000 * sizeof(float),
			.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		};

		vkCreateBuffer(device, &bufferInfo, NULL, &buffer);	
	}

	// ################################################################################
	// Create VkDescriptorPool

	VkDescriptorPool descriptorPool;
	{
		VkDescriptorPoolSize descriptorPoolSize = 
		{
			.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.descriptorCount = 1,
		};

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = 1,
			.pPoolSizes = &descriptorPoolSize,
			.poolSizeCount = 1,
		};

		vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, NULL, &descriptorPool);
	}

	VkDescriptorSetLayout descriptorSetLayout;
	{
		const VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[] = 
		{
			{
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
				.pImmutableSamplers = NULL,
			}
		};

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pBindings = descriptorSetLayoutBindings,
			.bindingCount = 1,
		};

		vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, NULL, &descriptorSetLayout);
	}

	VkDescriptorSet descriptorSet;
	create_descriptor_set(&descriptorSet, device, descriptorPool, descriptorSetLayout);

	VkPipelineLayout pipelineLayout;
	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 1,
			.pSetLayouts = &descriptorSetLayout,
		};

		result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, NULL, &pipelineLayout);
		if (result != VK_SUCCESS) error("Problem at vkCreatePipelineLayout! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
	}

	VkPipeline pipeline;
	{	
		VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_COMPUTE_BIT,
			.module = shaderModule,
			.pName = "main",
		};

		VkComputePipelineCreateInfo computePipelineCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
			.stage = pipelineShaderStageCreateInfo,
			.layout = pipelineLayout,
		};

		result = vkCreateComputePipelines(device, NULL, 1, &computePipelineCreateInfo, NULL, &pipeline);
		if (result != VK_SUCCESS) error("Problem at vkCreateComputePipelines! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
	}

	// ################################################################################
	// Command Buffer

	VkCommandBuffer commandBuffer;
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = commandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};

		result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
		if (result != VK_SUCCESS) error("Problem at vkAllocateCommandBuffers! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
	}

	// Record command buffer	
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		};

		result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
		if (result != VK_SUCCESS) error("Problem at vkBeginCommandBuffer! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

		// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ); TODO

		vkCmdDispatch(commandBuffer, 32, 32, 0);
	
		result = vkEndCommandBuffer(commandBuffer);
		if (result != VK_SUCCESS) error("Problem at vkEndCommandBuffer! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
	}

	// Submit command buffer
	{
		VkSubmitInfo submitInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pCommandBuffers = &commandBuffer,
			.commandBufferCount = 1,
		};

		vkQueueSubmit(queue, 1, &submitInfo, NULL);
	}

	// ################################################################################
	//	Cleanup
	//
	// ################################################################################

	vkFreeDescriptorSets(device, descriptorPool, 1, &descriptorSet);
	vkDestroyShaderModule(device, shaderModule, NULL);
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

	cleanup_vulkan(&instance, &device, &commandPool);
}
