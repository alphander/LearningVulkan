# My project for learning vulkan

As the title says, this is a little project I'm starting for myself to better my understanding of the vulkan graphics api.

## Learning material

### Vulkan Specification
The code I'm writing is coming from thoroughly reading the vulkan specification.
 https://registry.khronos.org/vulkan/specs/1.3/html/
(PDF): https://registry.khronos.org/vulkan/specs/1.3/pdf/vkspec.pdf

### Vulkan Tutorial
This is the very famous vulkan tutorial that everyone uses. I'm using the code examples as a loose guideline. (Note this is for C++) 
https://vulkan-tutorial.com/

### Vulkan in 30 minutes
A very useful article which gives a nice overiew on how vulkan is structured.
https://renderdoc.org/vulkan-in-30-minutes.html

## Downloads

### LunarG Vulkan
In order to use vulkan, I have downloaded LunarG's implementation of vulkan. 
https://www.lunarg.com/vulkan-sdk/

### Winlibs
The compilers and compiled libraries and headers I use, come from an excellent project by Brecht Sanders called winlibs.
https://winlibs.com/

```c
#include <vulkan/vulkan.h>

void DoVulkanRendering()
{
  const char *extensionNames[] = { "VK_KHR_surface", "VK_KHR_win32_surface" };

  VkInstanceCreateInfo instanceCreateInfo = {
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, 
    NULL,                                   

    0,                                      

    NULL,                                   

    0,                                      
    NULL,                                   

    2,                                      
    extensionNames,                         
  };

  VkInstance inst;
  vkCreateInstance(&instanceCreateInfo, NULL, &inst);

  VkPhysicalDevice phys[4]; uint32_t physCount = 4;
  vkEnumeratePhysicalDevices(inst, &physCount, phys);

  VkDeviceCreateInfo deviceCreateInfo = {
    
  };

  VkDevice dev;
  vkCreateDevice(phys[0], &deviceCreateInfo, NULL, &dev);

	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
		
	};
  VkSurfaceKHR surf;
  vkCreateWin32SurfaceKHR(inst, &surfaceCreateInfo, NULL, &surf);

  VkSwapchainCreateInfoKHR swapCreateInfo = {
    
  };
  VkSwapchainKHR swap;
  vkCreateSwapchainKHR(dev, &swapCreateInfo, NULL, &swap);

  VkImage images[4]; uint32_t swapCount;
  vkGetSwapchainImagesKHR(dev, swap, &swapCount, images);

  uint32_t currentSwapImage;
  vkAcquireNextImageKHR(dev, swap, UINT64_MAX, presentCompleteSemaphore, NULL, &currentSwapImage);

  VkImageView backbufferView;
  vkCreateImageView(dev, &backbufferViewCreateInfo, NULL, &backbufferView);

  VkQueue queue;
  vkGetDeviceQueue(dev, 0, 0, &queue);

  VkRenderPassCreateInfo renderpassCreateInfo = {
    
  };

  VkRenderPass renderpass;
  vkCreateRenderPass(dev, &renderpassCreateInfo, NULL, &renderpass);

  VkFramebufferCreateInfo framebufferCreateInfo = {
    
  };

  VkFramebuffer framebuffer;
  vkCreateFramebuffer(dev, &framebufferCreateInfo, NULL, &framebuffer);

  VkDescriptorSetLayoutCreateInfo descSetLayoutCreateInfo = {
    
  };

  VkDescriptorSetLayout descSetLayout;
  vkCreateDescriptorSetLayout(dev, &descSetLayoutCreateInfo, NULL, &descSetLayout);

  VkPipelineCreateInfo pipeLayoutCreateInfo = {
    
  };

  VkPipelineLayout pipeLayout;
  vkCreatePipelineLayout(dev, &pipeLayoutCreateInfo, NULL, &pipeLayout);

  VkShaderModule vertModule, fragModule;
  vkCreateShaderModule(dev, &vertModuleInfoWithSPIRV, NULL, &vertModule);
  vkCreateShaderModule(dev, &fragModuleInfoWithSPIRV, NULL, &fragModule);

  VkGraphicsPipelineCreateInfo pipeCreateInfo = {
    
  };

  VkPipeline pipeline;
  vkCreateGraphicsPipelines(dev, NULL, 1, &pipeCreateInfo, NULL, &pipeline);

  VkDescriptorPoolCreateInfo descPoolCreateInfo = {
    
  };

  VkDescriptorPool descPool;
  vkCreateDescriptorPool(dev, &descPoolCreateInfo, NULL, &descPool);

  VkDescriptorSetAllocateInfo descAllocInfo = {
    
  };

  VkDescriptorSet descSet;
  vkAllocateDescriptorSets(dev, &descAllocInfo, &descSet);

  VkBufferCreateInfo bufferCreateInfo = {
    
  };

  VkMemoryAllocateInfo memAllocInfo = {
    
    
  };
  VkBuffer buffer;
  VkDeviceMemory memory;
  vkCreateBuffer(dev, &bufferCreateInfo, NULL, &buffer);
  vkAllocateMemory(dev, &memAllocInfo, NULL, &memory);
  vkBindBufferMemory(dev, buffer, memory, 0);

  void *data = NULL;
  vkMapMemory(dev, memory, 0, VK_WHOLE_SIZE, 0, &data);
  
  vkUnmapMemory(dev, memory);

  VkWriteDescriptorSet descriptorWrite = {
    
  };

  vkUpdateDescriptorSets(dev, 1, &descriptorWrite, 0, NULL);

  VkCommandPoolCreateInfo commandPoolCreateInfo = {
    
  };

  VkCommandPool commandPool;
  vkCreateCommandPool(dev, &commandPoolCreateInfo, NULL, &commandPool);

  VkCommandBufferAllocateInfo commandAllocInfo = {
    
  };
  VkCommandBuffer cmd;
  vkAllocateCommandBuffers(dev, &commandAllocInfo, &cmd);


  vkBeginCommandBuffer(cmd, &cmdBeginInfo);
  vkCmdBeginRenderPass(cmd, &renderpassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
  
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  
  vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          descSetLayout, 1, &descSet, 0, NULL);
  
  vkCmdSetViewport(cmd, 1, &viewport);
  
  vkCmdDraw(cmd, 3, 1, 0, 0);
  vkCmdEndRenderPass(cmd);
  vkEndCommandBuffer(cmd);

  VkSubmitInfo submitInfo = {
    
  };

  vkQueueSubmit(queue, 1, &submitInfo, NULL);

  
  VkPresentInfoKHR presentInfo = {
    
  };
  vkQueuePresentKHR(queue, &presentInfo);
}

```
