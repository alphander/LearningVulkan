#include <vulkan/vulkan.h>
#include <stdio.h>

VkInstance instance;
VkPhysicalDevice physicalDevice;
VkDevice device;
VkQueue queue;
VkCommandPool commandPool;
VkCommandBuffer commandBuffer;
VkBuffer buffer;
VkDeviceMemory bufferMemory;
VkShaderModule computeShaderModule;
VkPipelineLayout pipelineLayout;
VkPipeline computePipeline;

// Compute shader code
const char* computeShaderCode = 
    "#version 450\n"
    "layout(local_size_x = 1) in;\n"
    "layout(std430, binding = 0) buffer Data {\n"
    "    float data[];\n"
    "};\n"
    "void main() {\n"
    "    uint id = gl_GlobalInvocationID.x;\n"
    "    data[id] = data[id] * 2.0f;\n"
    "}\n";

void create_instance() 
{
    // TODO: Initialize Vulkan instance
}

void create_device() 
{
    // TODO: Select physical device and create logical device
}

void create_buffer() 
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = 256; // Size of buffer in bytes
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    VkResult result = vkCreateBuffer(device, &bufferInfo, NULL, &buffer);
    // TODO: Check for errors

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = 0; // TODO: Select a suitable memory type

    result = vkAllocateMemory(device, &allocInfo, NULL, &bufferMemory);
    // TODO: Check for errors

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void create_compute_pipeline() 
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = strlen(computeShaderCode);
    createInfo.pCode = (uint32_t*)computeShaderCode;

    VkResult result = vkCreateShaderModule(device, &createInfo, NULL, &computeShaderModule);
    // TODO: Check for errors

    VkPipelineLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 0; // TODO: Specify descriptor set layouts if needed

    result = vkCreatePipelineLayout(device, &layoutInfo, NULL, &pipelineLayout);
    // TODO: Check for errors

    VkComputePipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipelineInfo.stage.module = computeShaderModule;
    pipelineInfo.stage.pName = "main";
    pipelineInfo.layout = pipelineLayout;

    result = vkCreateComputePipelines(device,VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &computePipeline);
    // TODO: Check for errors
}

void create_command_buffer() 
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;
    VkResult result = vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
    // TODO: Check for errors

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);

    VkBufferMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.buffer = buffer;
    barrier.size = VK_WHOLE_SIZE;
    barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, NULL, 1, &barrier, 0, NULL);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 0, NULL, 0, NULL);

    vkCmdDispatch(commandBuffer, 256, 1, 1);

    vkEndCommandBuffer(commandBuffer);
}

void submit_command_buffer() 
{
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    VkResult result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    // TODO: Check for errors

    vkQueueWaitIdle(queue);
}

int main() 
{
    create_instance();
    create_device();
    create_buffer();
    create_compute_pipeline();
    create_command_buffer();
    submit_command_buffer();

    // TODO: Read back results from buffer and print them
    return 0;
}
