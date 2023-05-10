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
	VkPhysicalDeviceFeatures physicalDeviceFeaturesArray[physicalDeviceCount];

	for (int i = 0; i < physicalDeviceCount; i++)
	{
		vkGetPhysicalDeviceProperties(physicalDeviceArray[i], &physicalDevicePropertiesArray[i]);
		vkGetPhysicalDeviceFeatures(physicalDeviceArray[i], &physicalDeviceFeaturesArray[i]);
	}

	// ----------------------------------------------------------------------------------------------------
	// Logging "physicalDevicePropertiesArray"
	#ifdef LOG_ALL_VULKAN_DEBUG
	{
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
	#endif

	VkPhysicalDevice physicalDevice;

	for (int i = 0; i < physicalDeviceCount; i++)
	{
		VkPhysicalDeviceProperties currentPhysicalDeviceProperties = physicalDevicePropertiesArray[i];
		if (currentPhysicalDeviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) 
			continue;

		uint32_t currentQueueFamilyPropertiesCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &currentQueueFamilyPropertiesCount, NULL);

		VkQueueFamilyProperties currentQueueFamilyPropertiesArray[currentQueueFamilyPropertiesCount];
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &currentQueueFamilyPropertiesCount, currentQueueFamilyPropertiesArray);

		bool complete = false;
		for (int j = 0; j < currentQueueFamilyPropertiesCount; j++)
		{
			if (currentQueueFamilyPropertiesArray[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				physicalDevice = physicalDeviceArray[i];
				complete = true;
				break;
			}
		}
		if (complete) break;
	}

	// ####################################################################################################
	// vkDevice

	VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = (float[1]) {1.0f};
	deviceQueueCreateInfo.queueFamilyIndex = 0;

	VkDevice device;

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;



	vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device);

	// ####################################################################################################
	//	Cleanup
	//
	// ####################################################################################################

	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);
}
