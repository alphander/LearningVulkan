#pragma once

typedef struct UtilFile
{
	size_t size;
	char* data;
} UtilFile;

void utilfile_create(UtilFile* utilFile, const char* path);
void utilfile_destroy(UtilFile* utilFile);

char* result_to_name(VkResult result);
void queue_flags_to_name(VkQueueFlags queueFlags, uint32_t* queueFlagCount, char** flagArray);
char* physical_device_type_to_name(VkPhysicalDeviceType physicalDeviceType);