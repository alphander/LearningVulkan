#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <stdint.h>

void queue_flags_to_name(uint32_t queueFlags, uint32_t* queueFlagCount, char** flagArray)
{
	const int maxFlags = 8;

	if (flagArray == NULL)
	{
		char* tempArray[maxFlags];
		flagArray = tempArray;
	}

	int i = 0;
	if (queueFlags & VK_QUEUE_GRAPHICS_BIT) 
		flagArray[i++] = "VK_QUEUE_GRAPHICS_BIT";
	if (queueFlags & VK_QUEUE_COMPUTE_BIT) 
		flagArray[i++] = "VK_QUEUE_COMPUTE_BIT";
	if (queueFlags & VK_QUEUE_TRANSFER_BIT) 
		flagArray[i++] = "VK_QUEUE_TRANSFER_BIT";
	if (queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) 
		flagArray[i++] = "VK_QUEUE_SPARSE_BINDING_BIT";
	if (queueFlags & VK_QUEUE_PROTECTED_BIT) 
		flagArray[i++] = "VK_QUEUE_PROTECTED_BIT";
	if (queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) 
		flagArray[i++] = "VK_QUEUE_VIDEO_DECODE_BIT_KHR";
	if (queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV) 
		flagArray[i++] = "VK_QUEUE_OPTICAL_FLOW_BIT_NV";

	*queueFlagCount = i;
}

char* physical_device_type_to_name(int physicalDeviceType)
{
	switch(physicalDeviceType)
	{
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			return "VK_PHYSICAL_DEVICE_TYPE_OTHER";
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			return "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			return "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			return "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			return "VK_PHYSICAL_DEVICE_TYPE_CPU";
		default:
			return NULL;
	}
}