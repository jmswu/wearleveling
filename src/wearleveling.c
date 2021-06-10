
#include <string.h>
#include <stdio.h>
#include "wearleveling.h"

#define WEARLEVELING_LIB_VER_MAJOR      (0U)
#define WEARLEVELING_LIB_VER_MINOR      (0U)
#define WEARLEVELING_LIB_VER_PATCH      (1U)

#define WEARLEVELING_LIB_FORMATED_FLAG  ((uint16_t)0x1234)
#define WEARLEVELING_LIB_DIRTY_FLAG     ((uint8_t)0x55)
#define WEARLEVELING_LIB_EMPTY_FLAG     ((uint8_t)0xFF)

static wearleveling_state_typeDef internalState = {0};

static void wearleveling_init(wearleveling_params_typeDef * const pParams);
static uint8_t wearleveling_save(uint8_t * const pData);
static uint8_t wearleveling_read(uint8_t * const pData);

static uint16_t wearleveling_calculateBucketSize(void);
static uint16_t wearleveling_calculateNumOfBuckets(void);
static uint32_t wearleveling_calculateAddressFromBucketIndex(uint16_t index);
static uint16_t wearleveling_findIndexRead(void);
static uint16_t wearleveling_findIndexWrite(void);
// static uint8_t wearleveling_isEmpty(void);
// static uint8_t wearleveling_isFull(void);
static uint8_t wearleveling_isFormated(void);
static void wearleveling_resetIndex(void);
static void wearleveling_formatPage(void);

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
    if (pParams == NULL) return;

    memset((void *)&internalState, 0, sizeof(wearleveling_state_typeDef));
    internalState.params = *pParams;
    internalState.bucketSize = wearleveling_calculateBucketSize();
    internalState.numOfBuckets = wearleveling_calculateNumOfBuckets();

    if (wearleveling_isFormated())
    {
        internalState.indexBucketWrite = wearleveling_findIndexWrite();
        internalState.indexBucketRead = wearleveling_findIndexRead();
    }
    else
    {
        wearleveling_formatPage();
        wearleveling_resetIndex();
    }
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

static uint16_t wearleveling_calculateBucketSize(void)
{
    uint16_t size_dataPlusDirtyMark_inBytes = internalState.params.dataSizeInByte + sizeof(WEARLEVELING_LIB_DIRTY_FLAG);
    return size_dataPlusDirtyMark_inBytes % 2 ? size_dataPlusDirtyMark_inBytes + 1 : size_dataPlusDirtyMark_inBytes;
}

static uint16_t wearleveling_calculateNumOfBuckets(void)
{
    uint16_t capacityMinusFormatedString = internalState.params.pageCapacityInByte - sizeof(WEARLEVELING_LIB_FORMATED_FLAG);
    return capacityMinusFormatedString / internalState.bucketSize;
}

static uint32_t wearleveling_calculateAddressFromBucketIndex(uint16_t index)
{
    const uint32_t FORMATED_FLAG_OFFSET = sizeof(WEARLEVELING_LIB_FORMATED_FLAG);
    return (FORMATED_FLAG_OFFSET + (index * internalState.bucketSize));
}

static uint16_t wearleveling_findIndexRead(void)
{
    uint16_t indexWrite = wearleveling_findIndexWrite();
    return indexWrite == 0 ? 0 : indexWrite - 1;
}

static uint16_t wearleveling_findIndexWrite(void)
{
    for(uint16_t i = 0; i < internalState.numOfBuckets; i++)
    {
        const uint32_t ADDRESS = wearleveling_calculateAddressFromBucketIndex(i);
        const uint8_t DIRTY_FLAG = (uint8_t)(internalState.params.readTwoByte(ADDRESS) & 0xFF);
        if (DIRTY_FLAG == WEARLEVELING_LIB_EMPTY_FLAG)
            return i;
    }

    return 0;
}

// static uint8_t wearleveling_isEmpty(void)
// {
//     if ((internalState.indexBucketRead == internalState.indexBucketWrite) && (internalState.indexBucketRead == 0))
//     {
//         return 1;
//     }
//     else
//     {
//         return 0;
//     }
// }

// static uint8_t wearleveling_isFull(void)
// {
//     return internalState.indexBucketWrite == internalState.numOfBuckets ? 1 : 0;
// }

static uint8_t wearleveling_isFormated(void)
{
    uint16_t formatedFlag = internalState.params.readTwoByte(0x00);
    return formatedFlag == WEARLEVELING_LIB_FORMATED_FLAG ? 1 : 0;
}

static void wearleveling_resetIndex(void)
{
    internalState.indexBucketRead = 0;
    internalState.indexBucketWrite = 0;
}

static void wearleveling_formatPage(void)
{
    internalState.params.pageErase();
    internalState.params.writeTwoByte(0x00, WEARLEVELING_LIB_FORMATED_FLAG);
}


wearleveling_state_typeDef * debug_wearleveling_getInternalState(void)
{
    return &internalState;
}