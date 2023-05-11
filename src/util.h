#pragma once

char* result_to_name(uint32_t result);
void queue_flags_to_name(uint32_t queueFlags, uint32_t* queueFlagCount, char** flagArray);
char* physical_device_type_to_name(int physicalDeviceType);