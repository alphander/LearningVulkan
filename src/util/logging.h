#pragma once

#define LOGGING_STRINGIFY(x) #x
#define LOGGING_PASS(x) LOGGING_STRINGIFY(x)
#define LOGGING_AT "[" __FILE__ ":" LOGGING_PASS(__LINE__) "] "

#define LOGGING_EMPTY ({})

#ifndef DISABLE_ERROR_LOGGING
#define error(format, ...) ({logging_print(LOGGING_AT); logging_error(format, ##__VA_ARGS__); logging_print("\n");})
#else
#define error(format, ...) LOGGING_EMPTY
#endif

#ifndef DISABLE_FINISH_LOGGING
#define finish(format, ...) ({logging_print(LOGGING_AT); logging_finish(format, ##__VA_ARGS__); logging_print("\n");})
#else
#define finish(format, ...) LOGGING_EMPTY
#endif

#ifndef DISABLE_LOG_LOGGING
#define log(format, ...) ({logging_print(LOGGING_AT); logging_print(format, ##__VA_ARGS__); logging_print("\n");})
#else
#define log(format, ...) LOGGING_EMPTY
#endif

#ifndef DISABLE_PRINT_LOGGING
#define print(format, ...) logging_print(format, ##__VA_ARGS__)
#else
#define print(format, ...) LOGGING_EMPTY
#endif

void logging_error(const char* format, ...);
void logging_finish(const char* format, ...);
void logging_print(const char* format, ...);