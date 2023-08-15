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

#define vkresult_error(result, functionName) {if (result != VK_SUCCESS) error("Problem at %s! VkResult: %s\n", functionName, result_to_name(result));}

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


	VkResult result; // Reusable

	// ####################################################################################################
	// VkInstance
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
	// VkPhysicalDevice
	//
	//
	//
	// ####################################################################################################

	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);

	VkPhysicalDevice physicalDeviceArray[physicalDeviceCount];
	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDeviceArray);
	vkresult_error(result, "vkEnumeratePhysicalDevices"); // ERROR HANDLING

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
	//
	//
	//
	// ####################################################################################################

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
	// Create VkShaderModule
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
	// Create VkBuffer
	//
	//
	//
	// ####################################################################################################

	#define BUFFER_SIZE 10000 * sizeof(float)

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
	// memoryRequirements.

	VkDeviceMemory deviceMemory;
	{
		VkMemoryAllocateInfo memoryAllocateInfo =
		{
			.allocationSize = memoryRequirements.size,
		};
		result = vkAllocateMemory(device, &memoryAllocateInfo, NULL, &deviceMemory);
		vkresult_error(result, "vkAllocateMemory"); // ERROR HANDLING
	}

	// ####################################################################################################
	// Create VkDescriptorPool
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
		};

		result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, NULL, &descriptorPool);
		vkresult_error(result, "vkCreateDescriptorPool"); // ERROR HANDLING
	}

	#define DESCRIPTOR_SET_COUNT 1

	VkDescriptorSet descriptorSet;
	{
		const VkDescriptorSetLayoutBinding descriptorSetLayoutBinding[] = 
		{
			{
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
				.pImmutableSamplers = NULL,
			}
		};

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorSetCount = DESCRIPTOR_SET_COUNT,
			.pSetLayouts = descriptorSetLayoutBinding,
		};

		result = vkAllocateDescriptorSets(device, NULL, &descriptorSet);
		vkresult_error(result, "vkAllocateDescriptorSets"); // ERROR HANDLING
	}

	VkPipelineLayout pipelineLayout;
	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
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
	// Command Buffer
	//
	//
	//
	// ####################################################################################################

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

	// Record command buffer	
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		};

		result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
		vkresult_error(result, "vkBeginCommandBuffer"); // ERROR HANDLING

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

		// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ); TODO
	
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

		result = vkQueueSubmit(queue, 1, &submitInfo, NULL);
		vkresult_error(result, "vkQueueSubmit");
	}



	// ####################################################################################################
	//	Cleanup
	//
	//
	//
	// ####################################################################################################

	vkFreeDescriptorSets(device, descriptorPool, DESCRIPTOR_SET_COUNT, &descriptorSet);
	vkDestroyShaderModule(device, shaderModule, NULL);
	vkFreeCommandBuffers(device, commandPool, COMMAND_BUFFER_COUNT, &commandBuffer);
	vkDestroyCommandPool(device, commandPool, NULL);
	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);	
}
