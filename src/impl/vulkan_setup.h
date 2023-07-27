#pragma once

void vulkan_setup(const char* const enabledLayerArray[], 
				  uint32_t enabledLayerCount, 
				  const char* const enabledExtensionArray[], 
				  uint32_t enabledExtensionCount,
				  VkInstance* pInstance,
				  VkPhysicalDevice* pPhysicalDevice,
				  VkDevice* pDevice, 
				  VkQueue* pQueue, 
				  VkCommandPool* pCommandPool);

void vulkan_cleanup(VkInstance* pInstance,
				  	VkDevice* pDevice, 
				  	VkCommandPool* pCommandPool);