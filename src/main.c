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
	if (result != VK_SUCCESS) error("Problem at vkCreateInstance! VkResult: %d\n", result); // ERROR HANDLING

	// ####################################################################################################
	// VkPhysicalDevice

	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);

	VkPhysicalDevice physicalDeviceArray[physicalDeviceCount];
	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDeviceArray);
	if (result != VK_SUCCESS) error("Problem at vkEnumeratePhysicalDevices! VkResult: %d\n", result); // ERROR HANDLING

	// ####################################################################################################
	// VkPhysicalDeviceProperties
	
	VkPhysicalDeviceProperties physicalDevicePropertiesArray[physicalDeviceCount];

	for (int i = 0; i < physicalDeviceCount; i++)
	{
		vkGetPhysicalDeviceProperties(physicalDeviceArray[i], &physicalDevicePropertiesArray[i]);
	}

	// ----------------------------------------------------------------------------------------------------
	// Logging "physicalDevicePropertiesArray"
	#ifdef LOG_ALL_VULKAN_DEBUG

	printf("\nPhysical Device List:\n");
	printf("-------------------------------------------------------------------------\n");
	for (int i = 0; i < physicalDeviceCount; i++)
	{
		VkPhysicalDeviceProperties physicalDeviceProperties = physicalDevicePropertiesArray[i]; 
		printf("  %s:\n", physicalDeviceProperties.deviceName);
		printf("    - Device Type: \"%s\"\n", physical_device_type_to_name(physicalDeviceProperties.deviceType));
		printf("\n");
	}

	#endif

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Find a proper way of selecting a physical device based on its properties.

	VkPhysicalDevice physicalDevice = physicalDeviceArray[0]; // TEMP

	// ####################################################################################################
	// vkGetPhysicalDeviceQueueFamilyProperties

	uint32_t queueFamilyPropertiesCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, NULL);

	VkQueueFamilyProperties queueFamilyPropertiesArray[queueFamilyPropertiesCount];
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyPropertiesArray);

	// ----------------------------------------------------------------------------------------------------
	// Logging "queueFamilyPropertiesArray"'s queueFlags
	#ifdef LOG_ALL_VULKAN_DEBUG
	
	printf("\nQueue Family:\n");
	printf("-------------------------------------------------------------------------\n");
	for (int i = 0; i < queueFamilyPropertiesCount; i++)
	{
		VkQueueFlags queueFlags = queueFamilyPropertiesArray[i].queueFlags;

		uint32_t flagCount = 0;
		queue_flags_to_name(queueFlags, &flagCount, NULL);
		char* queueFlagNameArray[flagCount];
		queue_flags_to_name(queueFlags, &flagCount, queueFlagNameArray);

		printf("\n  Queue %i Flags:\n", i);
		for (int j = 0; j < flagCount; j++)
		{
			printf("    %s\n", queueFlagNameArray[j]);
		}
	}

	#endif

	// ####################################################################################################
	//	Cleanup
	//
	// ####################################################################################################

	vkDestroyInstance(instance, NULL);

	return 0;
}
