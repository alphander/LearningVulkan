#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <logging/logging.h>

typedef struct UtilFile
{
	size_t size;
	char* data;
} UtilFile;

char* result_to_name(uint32_t result)
{
	switch(result)
	{
		case VK_SUCCESS:
			return "VK_SUCCESS";
    	case VK_NOT_READY:
			return "VK_NOT_READY";
    	case VK_TIMEOUT:
			return "VK_TIMEOUT";
    	case VK_EVENT_SET:
			return "VK_EVENT_SET";
    	case VK_EVENT_RESET:
			return "VK_EVENT_RESET";
    	case VK_INCOMPLETE:
			return "VK_INCOMPLETE";
    	case VK_ERROR_OUT_OF_HOST_MEMORY:
			return "VK_ERROR_OUT_OF_HOST_MEMORY";
    	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    	case VK_ERROR_INITIALIZATION_FAILED:
			return "VK_ERROR_INITIALIZATION_FAILED";
    	case VK_ERROR_DEVICE_LOST:
			return "VK_ERROR_DEVICE_LOST";
    	case VK_ERROR_MEMORY_MAP_FAILED:
			return "VK_ERROR_MEMORY_MAP_FAILED";
    	case VK_ERROR_LAYER_NOT_PRESENT:
			return "VK_ERROR_LAYER_NOT_PRESENT";
    	case VK_ERROR_EXTENSION_NOT_PRESENT:
			return "VK_ERROR_EXTENSION_NOT_PRESENT";
    	case VK_ERROR_FEATURE_NOT_PRESENT:
			return "VK_ERROR_FEATURE_NOT_PRESENT";
    	case VK_ERROR_INCOMPATIBLE_DRIVER:
			return "VK_ERROR_INCOMPATIBLE_DRIVER";
    	case VK_ERROR_TOO_MANY_OBJECTS:
			return "VK_ERROR_TOO_MANY_OBJECTS";
    	case VK_ERROR_FORMAT_NOT_SUPPORTED:
			return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    	case VK_ERROR_FRAGMENTED_POOL:
			return "VK_ERROR_FRAGMENTED_POOL";
    	case VK_ERROR_UNKNOWN:
			return "VK_ERROR_UNKNOWN";
    	case VK_ERROR_OUT_OF_POOL_MEMORY:
			return "VK_ERROR_OUT_OF_POOL_MEMORY";
    	case VK_ERROR_INVALID_EXTERNAL_HANDLE:
			return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
    	case VK_ERROR_FRAGMENTATION:
			return "VK_ERROR_FRAGMENTATION";
    	case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
			return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
    	case VK_PIPELINE_COMPILE_REQUIRED:
			return "VK_PIPELINE_COMPILE_REQUIRED";
    	case VK_ERROR_SURFACE_LOST_KHR:
			return "VK_ERROR_SURFACE_LOST_KHR";
    	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
    	case VK_SUBOPTIMAL_KHR:
			return "VK_SUBOPTIMAL_KHR";
    	case VK_ERROR_OUT_OF_DATE_KHR:
			return "VK_ERROR_OUT_OF_DATE_KHR";
    	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
    	case VK_ERROR_VALIDATION_FAILED_EXT:
			return "VK_ERROR_VALIDATION_FAILED_EXT";
    	case VK_ERROR_INVALID_SHADER_NV:
			return "VK_ERROR_INVALID_SHADER_NV";
    	case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
			return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
    	case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
			return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
    	case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
			return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
    	case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
			return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
    	case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
			return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
    	case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
			return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
    	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
			return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
    	case VK_ERROR_NOT_PERMITTED_KHR:
			return "VK_ERROR_NOT_PERMITTED_KHR";
    	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
    	case VK_THREAD_IDLE_KHR:
			return "VK_THREAD_IDLE_KHR";
    	case VK_THREAD_DONE_KHR:
			return "VK_THREAD_DONE_KHR";
    	case VK_OPERATION_DEFERRED_KHR:
			return "VK_OPERATION_DEFERRED_KHR";
    	case VK_OPERATION_NOT_DEFERRED_KHR:
			return "VK_OPERATION_NOT_DEFERRED_KHR";
	#ifdef VK_ENABLE_BETA_EXTENSIONS
    	case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
			return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
	#endif
    	case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
			return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
    	case VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT:
			return "VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT";
	}
}

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
			return "INVALID_PHYSICAL_DEVICE_ENUMERATION";
	}
}

void utilfile_create(UtilFile* utilFile, const char* path)
{
	if (utilFile == NULL) error("util file is null %s!", path);

	FILE* file = fopen(path, "rb");

	if (file == NULL) error("Couldn't read file at %s!", path);

	fseek(file, 0L, SEEK_END);
	size_t fileSize = (size_t)ftell(file);
	rewind(file);

	char* fileBuffer = malloc(fileSize);

	if (fileBuffer == NULL)
	{
		fclose(file);
		error("Couldn't allocate memory for file!");
	}

	fread(fileBuffer, sizeof(char), fileSize, file);
	fclose(file);

	utilFile->size = fileSize;
	utilFile->data = fileBuffer;
}

void utilfile_destroy(UtilFile* file)
{
	free(file->data);
}