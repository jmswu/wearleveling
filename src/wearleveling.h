#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
    uint16_t pageCapacityInByte;
    uint16_t dataSizeInByte;
    uint16_t (*readTwoByte) (uint32_t addr);
    uint8_t (*writeTwoByte) (uint32_t addr, uint16_t data);
    uint8_t (*pageErase) (void);
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

typedef wearleveling_state_typeDef* wearleveling_handle_typeDef;

/* new interface, starting from v0.1.x */
wearleveling_handle_typeDef wearleveling_v2_construct(wearleveling_state_typeDef * const pState, wearleveling_params_typeDef * const pParam);
uint8_t wearleveling_v2_save(wearleveling_handle_typeDef handle, uint8_t * const pData);
uint8_t wearleveling_v2_read(wearleveling_handle_typeDef handle, uint8_t * const pData);
uint16_t wearleveling_v2_getEraseWriteCycleMultiplier(wearleveling_handle_typeDef handle);
uint32_t wearleveling_v2_getVersionNumber(void);

//
// New interface should be use, this is for backward 
// compatible only.
//
#define WEARLEVELING_USE_V1_CODE

#ifdef WEARLEVELING_USE_V1_CODE
extern const wearleveling_typeDef wearleveling;

uint32_t wearleveling_getVersionNumber(void);
uint16_t wearleveling_getEraseWriteCycleMultiplier(void);

/* for debug only */
wearleveling_state_typeDef * debug_wearleveling_getInternalState(void);
#endif

#ifdef __cplusplus
}
#endif