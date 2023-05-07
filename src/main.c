#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "logging/logging.h"

int main() 
{
	// ##########################################################################################
	//	GLFW Setup
	//
	// ##########################################################################################

	if (!glfwInit()) 
		error("Could not init GLFW");

	if (!glfwVulkanSupported()) 
		error("Vulkan is not supported by GLFW!");

	// ##########################################################################################
	//	Vulkan Setup
	//
	// ##########################################################################################

	VkResult result;

	VkInstance instance;

	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_3;
	applicationInfo.pEngineName = "No Engine";
	applicationInfo.pApplicationName = "LearningVulkan";

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.enabledExtensionCount = 0;
	instanceCreateInfo.ppEnabledExtensionNames = NULL;
	instanceCreateInfo.enabledLayerCount = 0;
	instanceCreateInfo.ppEnabledLayerNames = NULL;


	result = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
	if(result != VK_SUCCESS) error("Couldn't create vulkan instance! %d", result);

	vkDestroyInstance(instance, NULL);

	return 0;
}
