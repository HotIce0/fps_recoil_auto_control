#include <stdio.h>

// #define LOG_DEBUG

#define log_info(fmt, ...) \
    printf("[INFO] %s:%d: %s " fmt "\n", __FILE__, __LINE__, __func__, ## __VA_ARGS__)
#define log_err(fmt, ...) \
    printf("[ERR] %s:%d: %s " fmt "\n", __FILE__, __LINE__, __func__, ## __VA_ARGS__)
#define log_warn(fmt, ...) \
    printf("[WARN] %s:%d: %s " fmt "\n", __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#ifdef LOG_DEBUG
    #define log_debug(fmt, ...) \
        printf("[DEBUG] %s:%d: %s " fmt "\n", __FILE__, __LINE__, __func__, ## __VA_ARGS__)
#else
    #define log_debug(fmt, ...)
#endif