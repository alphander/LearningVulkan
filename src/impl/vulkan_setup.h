#pragma once

void setup_vulkan(char* const enabledLayerArray[], 
				  uint32_t enabledLayerCount, 
				  char* const enabledExtensionArray[], 
				  uint32_t enabledExtensionCount,
				  VkDevice* pDevice, 
				  VkQueue* pQueue, 
				  VkCommandPool* pCommandPool);

void cleanup_vulkan(VkInstance* pInstance,
				  	VkDevice* pDevice, 
				  	VkCommandPool* pCommandPool);