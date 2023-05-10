#pragma once

#include <stdio.h>
#include <time.h>

#ifndef NDEBUG
#define PROFILER_SAMPLE() profiler_sample()
#else
#define PROFILER_SAMPLE()
#endif

void profiler_sample(char* function_name)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    printf("%s\n", function_name);
    printf("nano seconds: %ld\n", ts.tv_nsec);

    
}