#pragma once

#define LOGGING_STRINGIFY(x) #x
#define LOGGING_PASS(x) LOGGING_STRINGIFY(x)

#define AT "[" __FILE__ ":" LOGGING_PASS(__LINE__) "] "

#define error(format, ...) ({logging_print(AT); logging_error(format, ##__VA_ARGS__); logging_print("\n");})
#define finish(format, ...) ({logging_print(AT); logging_finish(format, ##__VA_ARGS__); logging_print("\n");})
#define print(format, ...) ({logging_print(AT); logging_print(format, ##__VA_ARGS__); logging_print("\n");})

void logging_error(const char* format, ...);
void logging_finish(const char* format, ...);
void logging_print(const char* format, ...);