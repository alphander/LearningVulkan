#pragma once

#include <vulkan/vulkan.h>

void vkc_create();
void vkc_append_shader();
void vkc_dispatch(uint32_t xWidth, uint32_t yHeight, uint32_t zLength);
void vkc_destroy();