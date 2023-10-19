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



int main()
{
	// ####################################################################################################
	// Buffer
	//
	//
	//
	// ####################################################################################################

	#define BUFFER_SIZE 1024 * sizeof(float)

	VkBuffer buffer;
	{
		VkBufferCreateInfo bufferInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = BUFFER_SIZE,
			.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		};

		result = vkCreateBuffer(device, &bufferInfo, NULL, &buffer);
		vkresult_error(result, "vkCreateBuffer"); // ERROR HANDLING
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

	uint32_t requiredPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
									 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	int selectedMemoryTypeIndex = -1;

	for (int i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if (!(memoryRequirements.memoryTypeBits & (1 << i))) // Required to get valid type
			continue;

		VkMemoryType memoryType = physicalDeviceMemoryProperties.memoryTypes[i]; // This is personalized
		if ((memoryType.propertyFlags & requiredPropertyFlags) == requiredPropertyFlags)
		{
			selectedMemoryTypeIndex = i;
			break;
		}	
	}

	if (selectedMemoryTypeIndex == -1)
		error("Could not find valid memory type for required memory properties!");

	print("-MemoryType: %i\n", selectedMemoryTypeIndex);
	{
		uint32_t memoryPropertyFlagCount;
		VkMemoryPropertyFlags memoryPropertyFlags = physicalDeviceMemoryProperties.memoryTypes[selectedMemoryTypeIndex].propertyFlags;
		debug_memory_property_flag_names(memoryPropertyFlags, &memoryPropertyFlagCount, NULL);
		char* memoryPropertyFlagNames[memoryPropertyFlagCount];
		debug_memory_property_flag_names(memoryPropertyFlags, &memoryPropertyFlagCount, memoryPropertyFlagNames);
		
		for (int i = 0; i < memoryPropertyFlagCount; i++)
			print("  -%s\n", memoryPropertyFlagNames[i]);
	}

	VkDeviceMemory deviceMemory;
	{
		VkMemoryAllocateInfo memoryAllocateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memoryRequirements.size,
			.memoryTypeIndex = selectedMemoryTypeIndex,
		};
		result = vkAllocateMemory(device, &memoryAllocateInfo, NULL, &deviceMemory);
		vkresult_error(result, "vkAllocateMemory"); // ERROR HANDLING
	}

	print("\n");

	result = vkBindBufferMemory(device, buffer, deviceMemory, 0);
	vkresult_error(result, "vkBindBufferMemory"); // ERROR HANDLING

	// ####################################################################################################
	// Shader Module
	//
	//
	//
	// ####################################################################################################

	// ####################################################################################################
	// Descriptor Set
	//
	//
	//
	// ####################################################################################################

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
		vkresult_error(result, "vkCreateDescriptorPool"); // ERROR HANDLING
	}

	#define DESCRIPTOR_SET_COUNT 1

	const VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[] = 
	{
		{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
			.pImmutableSamplers = NULL,
		},
	};

	VkDescriptorSetLayout descriptorSetLayout;
	{
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pBindings = descriptorSetLayoutBindings,
			.bindingCount = 1,
		};

		result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, NULL, &descriptorSetLayout);
		vkresult_error(result, "vkCreateDescriptorSetLayout"); // ERROR HANDLING
	}

	VkDescriptorSet descriptorSet;
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorSetCount = DESCRIPTOR_SET_COUNT,
			.pSetLayouts = &descriptorSetLayout,
			.descriptorPool = descriptorPool,
		};

		result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet);
		vkresult_error(result, "vkAllocateDescriptorSets"); // ERROR HANDLING
	}

	// ####################################################################################################
	// Create VkPipeline
	//
	//
	//
	// ####################################################################################################

	VkPipelineLayout pipelineLayout;
	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pSetLayouts = &descriptorSetLayout,
			.setLayoutCount = 1,
		};

		result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, NULL, &pipelineLayout);
		vkresult_error(result, "vkCreatePipelineLayout"); // ERROR HANDLING
	}

	#define COMPUTE_PIPELINE_COUNT 1
	#define COMPUTE_PIPELINE_ENTRY_FUNCTION_NAME "main"

	VkPipeline pipeline;
	{	
		VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_COMPUTE_BIT,
			.module = shaderModule,
			.pName = COMPUTE_PIPELINE_ENTRY_FUNCTION_NAME,
		};

		VkComputePipelineCreateInfo computePipelineCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
			.stage = pipelineShaderStageCreateInfo,
			.layout = pipelineLayout,
		};

		result = vkCreateComputePipelines(device, NULL, COMPUTE_PIPELINE_COUNT, &computePipelineCreateInfo, NULL, &pipeline);
		vkresult_error(result, "vkCreateComputePipelines"); // ERROR HANDLING
	}

	// ####################################################################################################
	// Command Pool
	//
	//
	//
	// ####################################################################################################

	VkCommandPool commandPool;
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = queueFamilyIndex,
		};

		result = vkCreateCommandPool(device, &commandPoolCreateInfo, NULL, &commandPool);
		vkresult_error(result, "vkCreateCommandPool"); // ERROR HANDLING
	}

	// ####################################################################################################
	// Command Buffer
	//
	//
	//
	// ####################################################################################################

	#define FENCE_COUNT 1

	VkFence fence;
	{
		VkFenceCreateInfo fenceCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		};

		result = vkCreateFence(device, &fenceCreateInfo, NULL, &fence);
		vkresult_error(result, "vkCreateFence");
	}

	#define COMMAND_BUFFER_COUNT 1

	VkCommandBuffer commandBuffer;
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = commandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = COMMAND_BUFFER_COUNT,
		};

		result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
		vkresult_error(result, "vkAllocateCommandBuffers"); // ERROR HANDLING
	}

	// VkWriteDescriptorSet writeDescriptorSet = 
	// {
	// 	.
	// };

	// vkUpdateDescriptorSets(device, );

	// Record command buffer	
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		};

		result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
		vkresult_error(result, "vkBeginCommandBuffer"); // ERROR HANDLING

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

		vkCmdBindDescriptorSets(commandBuffer, 
								VK_PIPELINE_BIND_POINT_COMPUTE, 
								pipelineLayout, 
								0,
								DESCRIPTOR_SET_COUNT,
								&descriptorSet,
								0,
								NULL);

		// vkCmdUpdateBuffer(commandBuffer, buffer, 0, 1024, )
		// vkInit

		vkCmdDispatch(commandBuffer, 1024, 1, 1);
	
		result = vkEndCommandBuffer(commandBuffer);
		vkresult_error(result, "vkEndCommandBuffer"); // ERROR HANDLING
	}

	// Submit command buffer
	{
		VkSubmitInfo submitInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pCommandBuffers = &commandBuffer,
			.commandBufferCount = COMMAND_BUFFER_COUNT,
		};

		result = vkQueueSubmit(queue, 1, &submitInfo, fence);
		vkresult_error(result, "vkQueueSubmit");
	}

	vkWaitForFences(device, FENCE_COUNT, &fence, VK_TRUE, UINT64_MAX);

	// ####################################################################################################
	//	Cleanup
	//
	//
	//
	// ####################################################################################################

	vkDestroyFence(device, fence, NULL);

	vkFreeCommandBuffers(device, commandPool, COMMAND_BUFFER_COUNT, &commandBuffer);
	vkDestroyCommandPool(device, commandPool, NULL);

	vkDestroyPipeline(device, pipeline, NULL);
	vkDestroyPipelineLayout(device, pipelineLayout, NULL);

	vkFreeDescriptorSets(device, descriptorPool, DESCRIPTOR_SET_COUNT, &descriptorSet);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);
	vkDestroyDescriptorPool(device, descriptorPool, NULL);

	vkDestroyShaderModule(device, shaderModule, NULL);

	vkFreeMemory(device, deviceMemory, NULL);
	vkDestroyBuffer(device, buffer, NULL);

	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);	
}
