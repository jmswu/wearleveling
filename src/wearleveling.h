#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint32_t wearleveling_getVersionNumber(void);

typedef struct
{
    uint16_t pageCapacityInByte;
    uint16_t dataSizeInByte;
    uint16_t (*readTwoByte) (uint32_t addr);
    uint8_t (*writeTwoByte) (uint32_t addr, uint16_t data);
    uint8_t (*pageErase) (uint16_t pageNum);
}wearleveling_params_typeDef;

typedef struct
{
    wearleveling_params_typeDef params;
    uint16_t indexBucketRead;
    uint16_t indexBucketWrite;
    uint16_t bucketSize;
    uint16_t numOfBuckets;
}wearleveling_state_typeDef;

typedef struct 
{
    void (*init) (wearleveling_params_typeDef * const pParams);
    uint8_t (*save) (uint8_t * const pData);
    uint8_t (*read) (uint8_t * const pData);
}wearleveling_typeDef;

extern const wearleveling_typeDef wearleveling;

/* for debug only */
wearleveling_state_typeDef * debug_wearleveling_getInternalState(void);

#ifdef __cplusplus
}
#endif