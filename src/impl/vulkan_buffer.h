#pragma once

void vulkan_buffer_create(VkBuffer* pBuffer, void* pData, VkDevice device, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties);
void vulkan_buffer_destroy(VkBuffer* pBuffer, VkDevice device);