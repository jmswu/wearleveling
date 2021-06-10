
#include <string.h>
#include "wearleveling.h"

#define WEARLEVELING_LIB_VER_MAJOR      (0U)
#define WEARLEVELING_LIB_VER_MINOR      (0U)
#define WEARLEVELING_LIB_VER_PATCH      (1U)

static wearleveling_state_typeDef internalState = {0};

static void wearleveling_init(wearleveling_params_typeDef * const pParams);
static uint8_t wearleveling_save(uint8_t * const pData);
static uint8_t wearleveling_read(uint8_t * const pData);

const wearleveling_typeDef wearleveling = 
{
    .init   = wearleveling_init,
    .save   = wearleveling_save,
    .read   = wearleveling_read,
};

uint32_t wearleveling_getVersionNumber(void)
{
    return ((WEARLEVELING_LIB_VER_MAJOR << 16) | (WEARLEVELING_LIB_VER_MINOR << 8) | (WEARLEVELING_LIB_VER_PATCH));
}

static void wearleveling_init(wearleveling_params_typeDef * const pParams)
{
    memset((void *)&internalState, 0, sizeof(wearleveling_state_typeDef));
    internalState.params = *pParams;
    internalState.indexBucketRead = 0;
    internalState.indexBucketWrite = 0;
    internalState.bucketSize = 0;
    internalState.numOfBuckets = 0;
    internalState.indexPage = 0;
}

static uint8_t wearleveling_save(uint8_t * const pData)
{
    (void)pData;
    return 0;
}

static uint8_t wearleveling_read(uint8_t * const pData)
{
    (void)pData;
    return 0;
}



wearleveling_state_typeDef * debug_wearleveling_getInternalState(void)
{
    return &internalState;
}