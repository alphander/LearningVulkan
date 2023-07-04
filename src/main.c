#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <vulkan/vulkan.h>

#include "logging/logging.h"
#include "util.h"
#include "profiling.h"

#define LOG_ALL_VULKAN_DEBUG


int main()
{
	// ####################################################################################################
	//	Vulkan Setup
	//
	// ####################################################################################################

	const char* enabledLayerArray[] = 
	{
		"VK_LAYER_KHRONOS_validation",
	};
	const uint32_t enabledLayerCount = (uint32_t) (sizeof(enabledLayerArray) / sizeof(char*));

	VkResult result;

	// ####################################################################################################
	// VkInstance
	VkInstance instance;

	{
		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.apiVersion = VK_API_VERSION_1_3;
		applicationInfo.pEngineName = "No Engine";
		applicationInfo.pApplicationName = "LearningVulkan";

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledExtensionCount = 0;
		instanceCreateInfo.ppEnabledExtensionNames = NULL;
		instanceCreateInfo.enabledLayerCount = enabledLayerCount;
		instanceCreateInfo.ppEnabledLayerNames = enabledLayerArray;

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

	// Populate VkPhysicalDeviceProperties array and VkPhysicalDeviceFeatures array
	{
		for (int i = 0; i < physicalDeviceCount; i++)
		{
			vkGetPhysicalDeviceProperties(physicalDeviceArray[i], &physicalDevicePropertiesArray[i]);
			vkGetPhysicalDeviceFeatures(physicalDeviceArray[i], &physicalDeviceFeaturesArray[i]);
		}

		printf("\nPhysical Device List:\n");
		printf("-------------------------------------------------------------------------\n");
		for (int i = 0; i < physicalDeviceCount; i++)
		{
			VkPhysicalDeviceProperties physicalDeviceProperties = physicalDevicePropertiesArray[i]; 
			VkPhysicalDeviceFeatures physicalDeviceFeatures = physicalDeviceFeaturesArray[i];
			printf("  %s:\n", physicalDeviceProperties.deviceName);
			printf("    - Device Type: \"%s\"\n", physical_device_type_to_name(physicalDeviceProperties.deviceType));
			printf("\n");
		}
	}

	// ####################################################################################################
	// VkPhysicalDevice selection

	VkPhysicalDevice physicalDevice = NULL;
	uint32_t queueFamilyIndex = 0;

	printf("\nQueue Family:\n");
	printf("-------------------------------------------------------------------------");
	for (int i = 0; i < physicalDeviceCount; i++)
	{
		printf("\n -Queue Families: %s\n", physicalDevicePropertiesArray[i].deviceName);
		VkPhysicalDeviceProperties currentPhysicalDeviceProperties = physicalDevicePropertiesArray[i];

		uint32_t currentQueueFamilyPropertiesCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceArray[i], &currentQueueFamilyPropertiesCount, NULL);
		VkQueueFamilyProperties currentQueueFamilyPropertiesArray[physicalDeviceCount];
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceArray[i], &currentQueueFamilyPropertiesCount, currentQueueFamilyPropertiesArray);

		for (int j = 0; j < currentQueueFamilyPropertiesCount; j++)
		{
			VkQueueFamilyProperties currentQueueFamilyProperties = currentQueueFamilyPropertiesArray[j];
			VkQueueFlags queueFlags = currentQueueFamilyProperties.queueFlags;

			printf("   -Queue Family Index %i:", j);

			if (currentQueueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT &&
				currentPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				printf(" VALID\n");
				physicalDevice = physicalDeviceArray[i];
				queueFamilyIndex = j;
			}
			else printf(" NOT VALID\n");

			uint32_t flagCount;
			queue_flags_to_name(queueFlags, &flagCount, NULL);
			char* queueFlagNameArray[flagCount];
			queue_flags_to_name(queueFlags, &flagCount, queueFlagNameArray);


			for (int k = 0; k < flagCount; k++)
			{
				printf("      %s\n", queueFlagNameArray[k]);
			}
			printf("\n");
		}
	}

	if (physicalDevice == NULL) error("No valid queues!");

	// ####################################################################################################
	// vkDevice

	VkDevice device;

	// Create VkDevice
	{
		VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
		deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfo.queueCount = 1;
		deviceQueueCreateInfo.pQueuePriorities = (float[1]) {1.0f};
		deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
	
		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
		deviceCreateInfo.queueCreateInfoCount = 1;
	
		result = vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device);
		if (result != VK_SUCCESS) error("Problem at vkCreateDevice! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
	}

	VkQueue queue;
	vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);


	// ####################################################################################################
	//	Cleanup
	//
	// ####################################################################################################

	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);
}
