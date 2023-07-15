#include <vulkan/vulkan.h>
#include "util/logging.h"
#include "util/util.h"

// ################################################################################
// Static functions

static void select_physical_device(uint32_t physicalDeviceCount, 
								   VkPhysicalDevice physicalDeviceArray[],
								   VkPhysicalDevice* pPhysicalDevice, 
								   VkPhysicalDeviceProperties* pPhysicalDeviceProperties, 
								   uint32_t* pQueueFamilyIndex)
{
	// ################################################################################
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

	VkPhysicalDevice physicalDevice = NULL;
	VkPhysicalDeviceProperties physicalDeviceProperties;
	uint32_t queueFamilyIndex = 0;

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

	*pPhysicalDevice = physicalDevice;
	*pPhysicalDeviceProperties = physicalDeviceProperties;
	*pQueueFamilyIndex = queueFamilyIndex;
}


static void print_physical_device_stats(VkPhysicalDeviceProperties* pPhysicalDeviceProperties)
{
	print("Max bound descriptor sets: %i\n", pPhysicalDeviceProperties->limits.maxBoundDescriptorSets);
}

// ################################################################################
// Public functions

void setup_vulkan(const char* const enabledLayerArray[], 
				  uint32_t enabledLayerCount, 
				  const char* const enabledExtensionArray[], 
				  uint32_t enabledExtensionCount,
				  VkInstance* pInstance,
				  VkPhysicalDevice* pPhysicalDevice,
				  VkDevice* pDevice, 
				  VkQueue* pQueue, 
				  VkCommandPool* pCommandPool)
{
	VkResult result; // Reusable

	// ################################################################################
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
			.enabledLayerCount = enabledLayerCount,
			.ppEnabledLayerNames = enabledLayerArray,
			.enabledExtensionCount = enabledExtensionCount,
			.ppEnabledExtensionNames = enabledExtensionArray,
		};

		result = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
		if (result != VK_SUCCESS) error("Problem at vkCreateInstance! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
	}

	// ################################################################################
	// VkPhysicalDevice

	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);

	VkPhysicalDevice physicalDeviceArray[physicalDeviceCount];
	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDeviceArray);
	if (result != VK_SUCCESS) error("Problem at vkEnumeratePhysicalDevices! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING

	// ################################################################################
	// VkPhysicalDevice selection

	VkPhysicalDevice physicalDevice = NULL;
	VkPhysicalDeviceProperties physicalDeviceProperties;
	uint32_t queueFamilyIndex = 0;

	// Select physical device and select queue family
	select_physical_device(physicalDeviceCount, physicalDeviceArray, &physicalDevice, &physicalDeviceProperties, &queueFamilyIndex);
	print_physical_device_stats(&physicalDeviceProperties);

	// ################################################################################
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

	// ################################################################################
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

	*pInstance = instance,
	*pPhysicalDevice = physicalDevice,
    *pDevice = device; 
	*pQueue = queue;
	*pCommandPool = commandPool;
}

void cleanup_vulkan(VkInstance* pInstance,
				  	VkDevice* pDevice, 
				  	VkCommandPool* pCommandPool)
{
	vkDestroyCommandPool(*pDevice, *pCommandPool, NULL);
	vkDestroyDevice(*pDevice, NULL);
	vkDestroyInstance(*pInstance, NULL);
}