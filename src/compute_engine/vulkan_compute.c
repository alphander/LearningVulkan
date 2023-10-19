#include "vulkan_compute.h"
#include "util/util.h"
#include "util/logging.h"

// Macro preserves "error"	 line number
#define vkresult_error(result, functionName)\
{\
	if (result != VK_SUCCESS)\
		error("Problem at %s! VkResult: %s\n", functionName, debug_result_name(result));\
}

const char* enabledLayerArray[] = 
{
    "VK_LAYER_KHRONOS_validation",
};
const uint32_t enabledLayerCount = (uint32_t) (sizeof(enabledLayerArray) / sizeof(char*));

const char* enabledExtensionArray[] = 
{

};
const uint32_t enabledExtensionCount = (uint32_t) (sizeof(enabledExtensionArray) / sizeof(char*));

// ####################################################################################################
// Instance
//
//
//
// ####################################################################################################


static VkInstance instance_create()
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
        .enabledExtensionCount = enabledExtensionCount,
        .ppEnabledExtensionNames = enabledExtensionArray,
        .enabledLayerCount = enabledLayerCount,
        .ppEnabledLayerNames = enabledLayerArray,
    };

    VkInstance instance;

    VkResult result = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
    vkresult_error(result, "vkCreateInstance"); // ERROR HANDLING

    return instance;
}

static void instance_destroy(VkInstance instance)
{
    vkDestroyInstance(instance, NULL);
}

// ####################################################################################################
// PhysicalDevice
//
//
//
// ####################################################################################################

static VkPhysicalDevice physical_device_select(VkInstance instance)
{
    uint32_t physicalDeviceCount;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
    VkPhysicalDevice physicalDeviceArray[physicalDeviceCount];
    VkResult result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDeviceArray);
    vkresult_error(result, "vkEnumeratePhysicalDevices");

    // VkPhysicalDeviceProperties physicalDevicePropertiesArray[physicalDeviceCount];
    VkPhysicalDeviceFeatures physicalDeviceFeaturesArray[physicalDeviceCount];

    for (int i = 0; i < physicalDeviceCount; i++)
    {
        // vkGetPhysicalDeviceProperties(physicalDeviceArray[i], &physicalDevicePropertiesArray[i]);
        vkGetPhysicalDeviceFeatures(physicalDeviceArray[i], &physicalDeviceFeaturesArray[i]);
    }

    uint32_t selectedIndex = 0;
    for (int i = 0; i < physicalDeviceCount; i++)
    {
        VkPhysicalDevice physicalDevice = physicalDeviceArray[i];
        // VkPhysicalDeviceProperties physicalDeviceProperties = physicalDevicePropertiesArray[i];

        uint32_t queueFamilyPropertiesCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, NULL);
        VkQueueFamilyProperties queueFamilyPropertiesArray[queueFamilyPropertiesCount];
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyPropertiesArray);

        for (int j = 0; j < queueFamilyPropertiesCount; j++)
        {
            VkQueueFamilyProperties queueFamilyProperties = queueFamilyPropertiesArray[j];
            VkQueueFlags queueFlags = queueFamilyProperties.queueFlags;

            if (!(queueFlags & VK_QUEUE_COMPUTE_BIT)) continue;
        }
        selectedIndex = i;
    }
    return physicalDeviceArray[selectedIndex];
}

static VkPhysicalDeviceMemoryProperties physical_device_get_memory_properties(VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

    return physicalDeviceMemoryProperties;
}

static uint32_t physical_device_get_queue_index(VkPhysicalDevice physicalDevice)
{
    uint32_t index = 0;

    uint32_t queueFamilyPropertiesCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, NULL);
    VkQueueFamilyProperties queueFamilyPropertiesArray[queueFamilyPropertiesCount];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyPropertiesArray);

    for (int j = 0; j < queueFamilyPropertiesCount; j++)
    {
        VkQueueFamilyProperties queueFamilyProperties = queueFamilyPropertiesArray[j];
        VkQueueFlags queueFlags = queueFamilyProperties.queueFlags;

        if (!(queueFlags & VK_QUEUE_COMPUTE_BIT)) continue;

        index = j;
    }
    return index;
}

// ####################################################################################################
// Device and Queue
//
//
//
// ####################################################################################################

static VkDevice device_create(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
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

    VkDevice device;
    VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device);
    vkresult_error(result, "vkCreateDevice"); // ERROR HANDLING

    return device;
}

static void device_destroy(VkDevice device)
{
    vkDestroyDevice(device, NULL);
}

static VkQueue device_get_queue(VkDevice device, uint32_t queueFamilyIndex)
{
    VkQueue queue;
	vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);

    return queue;
}

// ####################################################################################################
// Shader Module
//
//
//
// ####################################################################################################

static VkShaderModule shader_create(const char* filePath, VkDevice device)
{
    UtilFile utilFile;
    utilfile_create(&utilFile, filePath);

    VkShaderModuleCreateInfo shaderModuleCreateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = utilFile.size,
        .pCode = (uint32_t*)utilFile.data,
    };

    VkShaderModule shaderModule;
    VkResult result = vkCreateShaderModule(device, &shaderModuleCreateInfo, NULL, &shaderModule);
    vkresult_error(result, "vkCreateShaderModule"); // ERROR HANDLING

    utilfile_destroy(&utilFile);

    return shaderModule;
}

static void shader_destroy(VkShaderModule shaderModule, VkDevice device)
{
    vkDestroyShaderModule(device, shaderModule, NULL);
}

static VkBuffer buffer_create(size_t bufferSize, VkDevice device)
{
    VkBuffer buffer;

    VkBufferCreateInfo bufferInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = bufferSize,
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VkResult result = vkCreateBuffer(device, &bufferInfo, NULL, &buffer);
    vkresult_error(result, "vkCreateBuffer"); // ERROR HANDLING
}

static void buffer_destroy(VkBuffer buffer, VkDevice device)
{
    vkDestroyBuffer(device, buffer, NULL);
}

static VkInstance instance;
static VkPhysicalDevice physicalDevice;
static VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
static uint32_t queueFamilyIndex;

static VkDevice device;
static VkQueue queue;
static VkShaderModule shaderModule;

void vkc_create()
{
    instance = instance_create();
    physicalDevice = physical_device_select(instance);
    physicalDeviceMemoryProperties = physical_device_get_memory_properties(physicalDevice);
    queueFamilyIndex = physical_device_get_queue_index(physicalDevice);

    debug_physical_device(physicalDevice, queueFamilyIndex);

    device = device_create(physicalDevice, queueFamilyIndex);
    queue = device_get_queue(device, queueFamilyIndex);

    shaderModule = shader_create("obj/computeshader.spv", device);
}

void vkc_dispatch(uint32_t xWidth, uint32_t yHeight, uint32_t zLength)
{
    // VkCommandBuffer commandBuffer;
    // VkCommandBufferAllocateInfo commandBufferAllocateInfo = 
    // {
    //     .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    //     .commandPool = commandPool,
    //     .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    //     .commandBufferCount = COMMAND_BUFFER_COUNT,
    // };
    // vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);


}

void vkc_destroy()
{
    shader_destroy(shaderModule, device);
    device_destroy(device);
    instance_destroy(instance);
}