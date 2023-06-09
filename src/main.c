// !! Headers in required order !!

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <vulkan/vulkan.h> 
#include <GLFW/glfw3.h>

// #define DISABLE_ERROR_LOGGING
// #define DISABLE_FINISH_LOGGING
// #define DISABLE_LOG_LOGGING
// #define DISABLE_PRINT_LOGGING

#include "logging/logging.h"
#include "util.h"
#include "profiling.h"

int main()
{
	const char* enabledLayerArray[] = 
	{
		"VK_LAYER_KHRONOS_validation",
	};
	const uint32_t enabledLayerCount = (uint32_t) (sizeof(enabledLayerArray) / sizeof(char*));

	const char* const enabledExtensionArray[] = 
	{

	};
	const uint32_t enabledExtensionCount = (uint32_t) (sizeof(enabledExtensionArray) / sizeof(char*));

	if (!glfwInit()) error("OOPS");


	VkResult result; // Reusable

	// ####################################################################################################
	// VkInstance

	VkInstance instance;

	// Create VkInstance
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
		if (result != VK_SUCCESS) error("Problem at vkCreateInstance! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
	}

	// ####################################################################################################
	// VkPhysicalDevice

	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);

	VkPhysicalDevice physicalDeviceArray[physicalDeviceCount];
	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDeviceArray);
	if (result != VK_SUCCESS) error("Problem at vkEnumeratePhysicalDevices! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING

	// ####################################################################################################
	// VkPhysicalDeviceProperties
	
	VkPhysicalDeviceProperties physicalDevicePropertiesArray[physicalDeviceCount];
	VkPhysicalDeviceFeatures physicalDeviceFeaturesArray[physicalDeviceCount];

	// Populate VkPhysicalDeviceProperties array and populate VkPhysicalDeviceFeatures array
	{
		for (int i = 0; i < physicalDeviceCount; i++)
		{
			vkGetPhysicalDeviceProperties(physicalDeviceArray[i], &physicalDevicePropertiesArray[i]);
			vkGetPhysicalDeviceFeatures(physicalDeviceArray[i], &physicalDeviceFeaturesArray[i]);
		}

		// Print VkPhysicalDeviceProperties and VkPhysicalDeviceFeatures arrays
		print("\nPhysical Device List:\n");
		print("-------------------------------------------------------------------------\n");
		for (int i = 0; i < physicalDeviceCount; i++)
		{
			VkPhysicalDeviceProperties physicalDeviceProperties = physicalDevicePropertiesArray[i]; 
			VkPhysicalDeviceFeatures physicalDeviceFeatures = physicalDeviceFeaturesArray[i];
			print("  %s:\n", physicalDeviceProperties.deviceName);
			print("    - Device Type: \"%s\"\n\n", physical_device_type_to_name(physicalDeviceProperties.deviceType));
		}
	}

	// ####################################################################################################
	// VkPhysicalDevice selection

	VkPhysicalDevice physicalDevice = NULL;
	VkPhysicalDeviceProperties physicalDeviceProperties;
	uint32_t queueFamilyIndex = 0;

	// Select physical device and select queue family
	{
		print("\nQueue Family:\n");
		print("-------------------------------------------------------------------------");
		for (int i = 0; i < physicalDeviceCount; i++)
		{
			print("\n -Queue Families: %s\n", physicalDevicePropertiesArray[i].deviceName);
			VkPhysicalDeviceProperties currentPhysicalDeviceProperties = physicalDevicePropertiesArray[i];

			uint32_t currentQueueFamilyPropertiesCount;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceArray[i], &currentQueueFamilyPropertiesCount, NULL);
			VkQueueFamilyProperties currentQueueFamilyPropertiesArray[physicalDeviceCount];
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceArray[i], &currentQueueFamilyPropertiesCount, currentQueueFamilyPropertiesArray);

			for (int j = 0; j < currentQueueFamilyPropertiesCount; j++)
			{
				VkQueueFamilyProperties currentQueueFamilyProperties = currentQueueFamilyPropertiesArray[j];
				VkQueueFlags queueFlags = currentQueueFamilyProperties.queueFlags;

				print("   -Queue Family Index %i:", j);

				if (currentQueueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT //&&
					/*currentPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU*/)
				{
					print(" VALID\n");
					physicalDevice = physicalDeviceArray[i];
					physicalDeviceProperties = physicalDevicePropertiesArray[i];
					queueFamilyIndex = j;
				}
				else
				{
					print(" NOT VALID\n");
				}

				uint32_t flagCount;
				queue_flags_to_name(queueFlags, &flagCount, NULL);
				char* queueFlagNameArray[flagCount];
				queue_flags_to_name(queueFlags, &flagCount, queueFlagNameArray);


				for (int k = 0; k < flagCount; k++)
				{
					print("      %s\n", queueFlagNameArray[k]);
				}
				print("\n");
			}
		}

		if (physicalDevice == NULL) error("No valid queues!"); // ERROR HANDLING

		print("Chose Queue Family: %d\n", queueFamilyIndex);
	}

	print("Max bound descriptor sets: %i\n", physicalDeviceProperties.limits.maxBoundDescriptorSets);

	// ####################################################################################################
	// vkDevice and VkQueue

	VkDevice device;

	// Create VkDevice
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
		if (result != VK_SUCCESS) error("Problem at vkCreateDevice! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
	}

	VkQueue queue;
	vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);

	// ####################################################################################################
	// Command Pool

	VkCommandPool commandPool;

	// Create VkCommandPool
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = queueFamilyIndex,
		};

		result = vkCreateCommandPool(device, &commandPoolCreateInfo, NULL, &commandPool);
		if (result != VK_SUCCESS) error("Problem at vkCreateCommandPool! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
	}

	// ####################################################################################################
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

	// ####################################################################################################
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

	// ####################################################################################################
	// Create VkDescriptorPool

	VkDescriptorPool descriptorPool;
	{
		VkDescriptorPoolSize descriptorPoolSize = 
		{
			.type = VK_
		};

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = 1,
			.
		};

		vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, NULL, &descriptorPool);
	}

	VkDescriptorSet descriptorSet;
	{

		const VkDescriptorSetLayoutBinding descriptorSetLayoutBinding[] = 
		{
			{
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
				.pImmutableSamplers = NULL,
			}
		};


		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorSetCount = 1,
			.pSetLayouts = descriptorSetLayoutBinding,
		};

		vkAllocateDescriptorSets(device, NULL, &descriptorSet);
	}

	VkPipelineLayout pipelineLayout;
	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
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

	// ####################################################################################################
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

	// ####################################################################################################
	//	Cleanup
	//
	// ####################################################################################################

	vkFreeDescriptorSets(device, descriptorPool, 1, &descriptorSet);
	vkDestroyShaderModule(device, shaderModule, NULL);
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	vkDestroyCommandPool(device, commandPool, NULL);
	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);
	
}
