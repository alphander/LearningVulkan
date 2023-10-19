#include "compute_engine/vulkan_compute.h"

int main()
{

    vkc_create();

    vkc_dispatch(10, 10, 10);

    vkc_destroy();
}