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
#include "impl/vulkan_buffer.h"

const char* const enabledLayerArray[] = 
{
    "VK_LAYER_KHRONOS_validation",
};
const uint32_t enabledLayerCount = (uint32_t) (sizeof(enabledLayerArray) / sizeof(char*));

const char* const enabledExtensionArray[] = 
{

};
const uint32_t enabledExtensionCount = (uint32_t) (sizeof(enabledExtensionArray) / sizeof(char*));


int main()
{
	// ################################################################################
	// Setup vulkan

	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue queue;
	VkCommandPool commandPool;
	
	vulkan_setup(enabledLayerArray, enabledLayerCount, enabledExtensionArray, enabledExtensionCount,
				 &instance, &physicalDevice, &device, &queue, &commandPool);

	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

	for (int i = 0; i < VK_MAX_MEMORY_TYPES; i++)
	{
		VkMemoryType memoryType = physicalDeviceMemoryProperties.memoryTypes[i];
		uint32_t memoryTypeFlagCount;
		memory_type_to_name(memoryType.propertyFlags, &memoryTypeFlagCount, NULL);
		char* memoryTypeArray[memoryTypeFlagCount];
		memory_type_to_name(memoryType.propertyFlags, &memoryTypeFlagCount, memoryTypeArray);

		print("%i\n", i);

		for (int j = 0; j < memoryTypeFlagCount; j++)
		{
			print("    %s\n", memoryTypeArray[j]);
		}
	}

	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

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

	// VkBuffer buffer;
	// vulkan_buffer_create(&buffer, device);

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
			.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		};

		result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, NULL, &descriptorPool);
		if (result != VK_SUCCESS) error("Problem at vkCreateDescriptorPool! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
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

		result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, NULL, &descriptorSetLayout);
		if (result != VK_SUCCESS) error("Problem at vkCreateDescriptorSetLayout! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING

	}

	VkDescriptorSet descriptorSet;
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = descriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &descriptorSetLayout,
		};

		result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet);
		if (result != VK_SUCCESS) error("Problem at vkAllocateDescriptorSets! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
	}

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

	// ################################################################################
	// Create VkPipeline

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
	// Create VkCommandBuffer

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

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

		vkCmdDispatch(commandBuffer, 1024, 0, 0);
	
		result = vkEndCommandBuffer(commandBuffer);
		if (result != VK_SUCCESS) error("Problem at vkEndCommandBuffer! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
	}

	// ################################################################################
	// Create VkFence

	VkFence fence;
	{
		VkFenceCreateInfo fenceCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		};

		result = vkCreateFence(device, &fenceCreateInfo, NULL, &fence);
		if (result != VK_SUCCESS) error("Problem at vkCreateFence! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
	}

	// Submit command buffer

	{
		VkSubmitInfo submitInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pCommandBuffers = &commandBuffer,
			.commandBufferCount = 1,
		};

		result = vkQueueSubmit(queue, 1, &submitInfo, fence);
		if (result != VK_SUCCESS) error("Problem at vkQueueSubmit! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING

		result = vkWaitForFences(device, 1, &fence, VK_TRUE, 10000000);
		if (result != VK_SUCCESS) error("Problem at vkWaitForFences! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
	}

	// ################################################################################
	//	Cleanup
	//
	// ################################################################################

	vkDestroyFence(device, fence, NULL);
	
	// vulkan_buffer_destroy(&buffer, device);

	vkDestroyPipeline(device, pipeline, NULL);
	vkDestroyPipelineLayout(device, pipelineLayout, NULL);
	vkFreeDescriptorSets(device, descriptorPool, 1, &descriptorSet);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);
	vkDestroyDescriptorPool(device, descriptorPool, NULL);
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	vkDestroyShaderModule(device, shaderModule, NULL);

	vulkan_cleanup(&instance, &device, &commandPool);
}
