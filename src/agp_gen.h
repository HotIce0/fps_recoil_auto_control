#ifndef __AGP_GEN_H__
#define __AGP_GEN_H__
#include <stdint.h>

// can dynamic change
#define DEFAULT_COEFFICIENT 2.5
#define DEFAULT_SENSITIVE   1.5

typedef enum AGPCollectIndex {
    // AGP_COLLECT_IDX_NONE,
    AGP_COLLECT_IDX_AK47,
    AGP_COLLECT_IDX_M4A1,
    AGP_COLLECT_IDX_MAX,
} AGPCollectIndex;

typedef struct AGPData {
    int32_t x;
    int32_t y;
} AGPData;

typedef struct AGPContext AGPContext;

const char *agp_collect_str(uint32_t index);
int agp_coefficient_change(AGPContext *context, uint8_t is_add);
int agp_sensitive_change(AGPContext *context, uint8_t is_add);

int agp_restart(AGPContext *context);
int agp_get_data(AGPContext *context, AGPData *data);
int agp_set_collect(AGPContext *context, uint32_t index);
void agp_close(AGPContext *context);
int agp_open(AGPContext **context);

#endif /* __AGP_GEN_H__ */