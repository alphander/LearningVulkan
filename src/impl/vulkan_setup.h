#pragma once

void setup_vulkan(const char* const enabledLayerArray[], 
				  uint32_t enabledLayerCount, 
				  const char* const enabledExtensionArray[], 
				  uint32_t enabledExtensionCount,
				  VkInstance* pInstance,
				  VkPhysicalDevice* pPhysicalDevice,
				  VkDevice* pDevice, 
				  VkQueue* pQueue, 
				  VkCommandPool* pCommandPool);

void cleanup_vulkan(VkInstance* pInstance,
				  	VkDevice* pDevice, 
				  	VkCommandPool* pCommandPool);