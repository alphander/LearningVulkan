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

// Macro preserves "error"	 line number
#define vkresult_error(result, functionName)\
{\
	if (result != VK_SUCCESS)\
		error("Problem at %s! VkResult: %s\n", functionName, debug_result_name(result));\
}

int main()
{
	const char* enabledLayerArray[] = 
	{
		"VK_LAYER_KHRONOS_validation",
	};
	const uint32_t enabledLayerCount = (uint32_t) (sizeof(enabledLayerArray) / sizeof(char*));

	const char* enabledExtensionArray[] = 
	{

	};
	const uint32_t enabledExtensionCount = (uint32_t) (sizeof(enabledExtensionArray) / sizeof(char*));


	VkResult result; // Reusable

	// ####################################################################################################
	// Instance
	//
	//
	//
	// ####################################################################################################

	VkInstance instance;
	{
		VkApplicationInfo applicationInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			.engineVersion = VK_MAKE_VERSION(1, 0, 0),
			.apiVersion = VK_API_VERSION_1_3,
			.pEngineName = "No Engine",
			.pApplicationName = "LearningVulkan",
		};

		VkInstanceCreateInfo instanceCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &applicationInfo,
			.enabledExtensionCount = enabledExtensionCount,
			.ppEnabledExtensionNames = enabledExtensionArray,
			.enabledLayerCount = enabledLayerCount,
			.ppEnabledLayerNames = enabledLayerArray,
		};

		result = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
		vkresult_error(result, "vkCreateInstance"); // ERROR HANDLING
	}

	// ####################################################################################################
	// PhysicalDevice
	//
	//
	//
	// ####################################################################################################

	VkPhysicalDevice physicalDevice;
	uint32_t queueFamilyIndex;
	{
		uint32_t physicalDeviceCount;
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
		VkPhysicalDevice physicalDeviceArray[physicalDeviceCount];
		result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDeviceArray);
		vkresult_error(result, "vkEnumeratePhysicalDevices");

		VkPhysicalDeviceProperties physicalDevicePropertiesArray[physicalDeviceCount];
		VkPhysicalDeviceFeatures physicalDeviceFeaturesArray[physicalDeviceCount];

		for (int i = 0; i < physicalDeviceCount; i++)
		{
			vkGetPhysicalDeviceProperties(physicalDeviceArray[i], &physicalDevicePropertiesArray[i]);
			vkGetPhysicalDeviceFeatures(physicalDeviceArray[i], &physicalDeviceFeaturesArray[i]);
		}

		uint32_t selectedIndex = 0;
		uint32_t index = 0;
		for (int i = 0; i < physicalDeviceCount; i++)
		{
			VkPhysicalDevice physicalDevice = physicalDeviceArray[i];
			VkPhysicalDeviceProperties physicalDeviceProperties = physicalDevicePropertiesArray[i];

			uint32_t queueFamilyPropertiesCount;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, NULL);
			VkQueueFamilyProperties queueFamilyPropertiesArray[physicalDeviceCount];
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyPropertiesArray);

			for (int j = 0; j < queueFamilyPropertiesCount; j++)
			{
				VkQueueFamilyProperties queueFamilyProperties = queueFamilyPropertiesArray[j];
				VkQueueFlags queueFlags = queueFamilyProperties.queueFlags;

				if (!(queueFlags & VK_QUEUE_COMPUTE_BIT)) continue;

				index = j;
			}
			selectedIndex = i;
		}
		physicalDevice = physicalDeviceArray[selectedIndex];
		queueFamilyIndex = index;
	}

	debug_physical_device(physicalDevice, queueFamilyIndex);

	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);
	// Query for memory type for vkAllocateMemory
	// debug_physical_device_memory_properties(physicalDeviceMemoryProperties);

	// ####################################################################################################
	// Device and Queue
	//
	//
	//
	// ####################################################################################################

	VkDevice device;
	{
		VkDeviceQueueCreateInfo deviceQueueCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueCount = 1,
			.pQueuePriorities = (float[1]) {1.0f},
			.queueFamilyIndex = queueFamilyIndex,
		};

		VkDeviceCreateInfo deviceCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pQueueCreateInfos = &deviceQueueCreateInfo,
			.queueCreateInfoCount = 1,
		};

		result = vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device);
		vkresult_error(result, "vkCreateDevice"); // ERROR HANDLING
	}

	VkQueue queue;
	vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);

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

	#define SHADER_FILE_PATH "obj/computeshader.spv"

	VkShaderModule shaderModule;
	{
		const char* filePath = SHADER_FILE_PATH;

		UtilFile utilFile;

		utilfile_create(&utilFile, filePath);

		VkShaderModuleCreateInfo shaderModuleCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = utilFile.size,
			.pCode = (uint32_t*)utilFile.data,
		};

		result = vkCreateShaderModule(device, &shaderModuleCreateInfo, NULL, &shaderModule);
		vkresult_error(result, "vkCreateShaderModule"); // ERROR HANDLING

		utilfile_destroy(&utilFile);
	}

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

	VkWriteDescriptorSet writeDescriptorSet = 
	{
		.descriptorCount = 1,
	};

	vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, NULL);

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
