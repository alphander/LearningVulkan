#include <vulkan/vulkan.h>

#include "util/logging.h"
#include "util/util.h"

#define TEMP_BUFFER_SIZE 8192 // Temporary

void vulkan_buffer_create(VkBuffer* pBuffer, void* pData, VkDevice device, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties)
{
    VkResult result;

    VkBuffer buffer;

    VkBufferCreateInfo bufferInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = TEMP_BUFFER_SIZE * sizeof(float),
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    result = vkCreateBuffer(device, &bufferInfo, NULL, &buffer);	
    if (result != VK_SUCCESS) error("Problem at vkCreateBuffer! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING

    VkMemoryAllocateInfo memoryAllocateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = TEMP_BUFFER_SIZE * sizeof(int),
        // .memoryTypeIndex = physicalDeviceMemoryProperties.;
    };

    VkDeviceMemory deviceMemory;

    result = vkAllocateMemory(device, &memoryAllocateInfo, NULL, &deviceMemory);
    if (result != VK_SUCCESS) error("Problem at vkAllocateMemory! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING

    result = vkBindBufferMemory(device, buffer, deviceMemory, 0);
    if (result != VK_SUCCESS) error("Problem at vkBindBufferMemory! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING

    result = vkMapMemory(device, deviceMemory, 0, TEMP_BUFFER_SIZE * sizeof(int), 0, &pData);
    if (result != VK_SUCCESS) error("Problem at vkMapMemory! VkResult: %s\n", result_to_name(result)); // ERROR HANDLING
	
    *pBuffer = buffer;
}

void vulkan_buffer_destroy(VkBuffer* pBuffer, VkDevice device)
{
    vkDestroyBuffer(device, *pBuffer, NULL);
}