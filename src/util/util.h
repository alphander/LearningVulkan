#pragma once

typedef struct UtilFile
{
	size_t size;
	char* data;
} UtilFile;

void utilfile_create(UtilFile* utilFile, const char* path);
void utilfile_destroy(UtilFile* utilFile);

void debug_physical_device(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);
const char* debug_result_name(VkResult result);
const char* debug_physical_device_type_name(VkPhysicalDeviceType physicalDeviceType);
void debug_queue_flags_names(VkQueueFlags queueFlags, uint32_t* queueFlagCount, char* flagArray[]);
void debug_memory_property_flag_names(VkMemoryPropertyFlagBits memoryTypeFlags, uint32_t* memoryTypeFlagCount, char* memoryTypeArray[]);
void debug_physical_device_memory_properties(VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties);
void debug_memory_requirements(VkMemoryRequirements memoryRequirements);