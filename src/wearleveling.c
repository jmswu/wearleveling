
#include <string.h>
#include <stdio.h>
#include "wearleveling.h"

#define WEARLEVELING_LIB_VER_MAJOR      (0U)
#define WEARLEVELING_LIB_VER_MINOR      (1U)
#define WEARLEVELING_LIB_VER_PATCH      (1U)

#define WEARLEVELING_LIB_FORMATED_FLAG  ((uint16_t)0x1234)
#define WEARLEVELING_LIB_DIRTY_FLAG     ((uint8_t)0x55)
#define WEARLEVELING_LIB_EMPTY_FLAG     ((uint8_t)0xFF)

static uint8_t wearleveling_v2_saveDataToAddress(wearleveling_state_typeDef * const pState, const uint32_t addr, uint8_t * const pData);
static uint16_t wearleveling_v2_calculateBucketSize(wearleveling_params_typeDef * const pParam);
static uint16_t wearleveling_v2_calculateNumOfBuckets(wearleveling_params_typeDef * const pParam);
static uint32_t wearleveling_v2_calculateAddressFromBucketIndex(const uint16_t index, const uint16_t bucketSize);
static uint16_t wearleveling_v2_findBucketIndexRead(wearleveling_state_typeDef * const pState);
static uint16_t wearleveling_v2_findBucketIndexWrite(wearleveling_state_typeDef * const pState);
static uint16_t wearleveling_v2_getTwoByte(uint16_t index, uint8_t * const pData);
static uint16_t wearleveling_v2_assembleLastTwoByte(wearleveling_state_typeDef * const pState, uint8_t * const pData);
static uint8_t wearleveling_v2_getLastbyte(wearleveling_state_typeDef * const pState, uint8_t * const pData);
static uint8_t wearleveling_v2_isFull(wearleveling_state_typeDef * const pState);
static uint8_t wearleveling_v2_isFormated(wearleveling_params_typeDef * const pParam);
static uint8_t wearleveling_v2_isEvenNumber(uint16_t number);
static void wearleveling_v2_resetIndex(wearleveling_state_typeDef * const pState);
static void wearleveling_v2_formatPage(wearleveling_params_typeDef * const pParam);
static void wearleveling_v2_updateBuckietIndexReadWrite(wearleveling_state_typeDef * const pState);

//
// V1 interface
//
#ifdef WEARLEVELING_USE_V1_CODE
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

static void wearleveling_init(wearleveling_params_typeDef * const pParams)
{
    wearleveling_v2_construct(&internalState, pParams);
}

static uint8_t wearleveling_save(uint8_t * const pData)
{
    return wearleveling_v2_save(&internalState, pData);
}

static uint8_t wearleveling_read(uint8_t * const pData)
{
    return wearleveling_v2_read(&internalState, pData);
}

uint32_t wearleveling_getVersionNumber(void)
{
    return wearleveling_v2_getVersionNumber();
}

uint16_t wearleveling_getEraseWriteCycleMultiplier(void)
{
    return internalState.numOfBuckets;
}

#endif


//
// V2 code starts here
//

uint32_t wearleveling_v2_getVersionNumber(void)
{
    return ((WEARLEVELING_LIB_VER_MAJOR << 16) | (WEARLEVELING_LIB_VER_MINOR << 8) | (WEARLEVELING_LIB_VER_PATCH));
}

wearleveling_handle_typeDef 
wearleveling_v2_construct(wearleveling_state_typeDef * const pState, wearleveling_params_typeDef * const pParam)
{
    if ((pState == NULL) || (pParam == NULL)) return NULL;

    memset((void *)pState, 0, sizeof(wearleveling_state_typeDef));
    pState->params = *pParam;
    //pState->params.pageCapacityInByte = pState->params.pageCapacityInByte % 2 ? pState->params.pageCapacityInByte - 1 : pState->params.pageCapacityInByte;
    pState->bucketSize = wearleveling_v2_calculateBucketSize(pParam);
    pState->numOfBuckets = wearleveling_v2_calculateNumOfBuckets(pParam);

    if (wearleveling_v2_isFormated(pParam))
    {
        pState->indexBucketWrite = wearleveling_v2_findBucketIndexWrite(pState);
        pState->indexBucketRead = wearleveling_v2_findBucketIndexRead(pState);
    }
    else
    {
        wearleveling_v2_formatPage(pParam);
        wearleveling_v2_resetIndex(pState);
    }

    return (wearleveling_handle_typeDef)pState;
}

static uint8_t wearleveling_v2_saveDataToAddress(wearleveling_state_typeDef * const pState, const uint32_t addr, uint8_t * const pData)
{
    if (pState == NULL) return 0;
    if (pData == NULL) return 0;

    uint16_t tmpTwoBytes;
    uint32_t offset;

    uint16_t numOfCopy = pState->params.dataSizeInByte >> 1;
    for(uint16_t i = 0; i < numOfCopy; i++)
    {
        offset = i * 2;
        tmpTwoBytes = wearleveling_v2_getTwoByte(i, pData);
        if (pState->params.writeTwoByte(addr + offset, tmpTwoBytes) == 0) return 0;
    }

    offset = numOfCopy * 2;
    tmpTwoBytes = wearleveling_v2_assembleLastTwoByte(pState, pData);
    if (pState->params.writeTwoByte(addr + offset, tmpTwoBytes) == 0) return 0;

    return 1;
}

uint16_t wearleveling_v2_getEraseWriteCycleMultiplier(wearleveling_handle_typeDef handle)
{
    return handle == NULL ? 0 : handle->numOfBuckets;
}

uint8_t wearleveling_v2_save(wearleveling_handle_typeDef handle, uint8_t * const pData)
{
    if (handle == NULL) return 0;
    if (pData == NULL) return 0;

    if (wearleveling_v2_isFull(handle))
    {
        wearleveling_v2_formatPage(&handle->params);
        wearleveling_v2_resetIndex(handle);
    }

    const uint32_t ADDRESS = wearleveling_v2_calculateAddressFromBucketIndex(handle->indexBucketWrite, handle->bucketSize);
    wearleveling_v2_updateBuckietIndexReadWrite(handle);
    return wearleveling_v2_saveDataToAddress(handle, ADDRESS, pData);
}

uint8_t wearleveling_v2_read(wearleveling_handle_typeDef handle, uint8_t * const pData)
{
    if ((pData == NULL) || (handle == NULL)) return 0;

    const uint32_t ADDR_TO_READ = wearleveling_v2_calculateAddressFromBucketIndex(handle->indexBucketRead, handle->bucketSize);
    const uint16_t NUM_OF_READ = handle->params.dataSizeInByte >> 1;
    uint16_t tmpTwoByte = 0;

    for(uint16_t i = 0; i < NUM_OF_READ; i++)
    {
        tmpTwoByte = handle->params.readTwoByte(ADDR_TO_READ + (i * 2));
        uint16_t index = i * 2;
        pData[index] = (uint8_t)(tmpTwoByte);
        pData[index + 1] = (uint8_t)(tmpTwoByte >> 8);
    }

    tmpTwoByte = handle->params.readTwoByte(ADDR_TO_READ + (NUM_OF_READ * 2));

    if (wearleveling_v2_isEvenNumber(internalState.params.dataSizeInByte) == 0)
    {
        pData[NUM_OF_READ * 2] = (uint8_t)tmpTwoByte;
    }

    return 1;
}

static uint16_t wearleveling_v2_calculateBucketSize(wearleveling_params_typeDef * const pParam)
{
    if (pParam == NULL) return 0;
    uint16_t size_dataPlusDirtyMark_inBytes = pParam->dataSizeInByte + sizeof(WEARLEVELING_LIB_DIRTY_FLAG);
    return size_dataPlusDirtyMark_inBytes % 2 ? size_dataPlusDirtyMark_inBytes + 1 : size_dataPlusDirtyMark_inBytes;
}

static uint16_t wearleveling_v2_calculateNumOfBuckets(wearleveling_params_typeDef * const pParam)
{
    uint16_t capacityMinusFormatedString = pParam->pageCapacityInByte - sizeof(WEARLEVELING_LIB_FORMATED_FLAG);
    return capacityMinusFormatedString / wearleveling_v2_calculateBucketSize(pParam);
}

static uint32_t wearleveling_v2_calculateAddressFromBucketIndex(const uint16_t index, const uint16_t bucketSize)
{
    const uint32_t FORMATED_FLAG_OFFSET = sizeof(WEARLEVELING_LIB_FORMATED_FLAG);
    return (FORMATED_FLAG_OFFSET + (index * bucketSize));
}

static uint16_t wearleveling_v2_findBucketIndexRead(wearleveling_state_typeDef * const pState)
{
    if (pState == NULL) return 0;

    const uint16_t writeIndex = pState->indexBucketWrite;
    return writeIndex == 0 ? 0 : writeIndex - 1;
}

static uint16_t wearleveling_v2_findBucketIndexWrite(wearleveling_state_typeDef * const pState)
{
    if (pState == NULL) return 0;

    for(uint16_t i = 0; i < pState->numOfBuckets; i++)
    {
        const uint32_t ADDRESS_OF_NEXT_BUCKET = wearleveling_v2_calculateAddressFromBucketIndex(i + 1, pState->bucketSize);
        const uint16_t LAST_TWO_BYTES = pState->params.readTwoByte(ADDRESS_OF_NEXT_BUCKET - 2);

        const uint8_t dirtyFlag = wearleveling_v2_isEvenNumber(pState->params.dataSizeInByte) ? (uint8_t)(LAST_TWO_BYTES) : (uint8_t)(LAST_TWO_BYTES >> 8);

        if ((dirtyFlag != WEARLEVELING_LIB_DIRTY_FLAG) && (dirtyFlag == WEARLEVELING_LIB_EMPTY_FLAG))
        {
            return i;
        }
    }

    return 0;
}

static uint16_t wearleveling_v2_getTwoByte(uint16_t index, uint8_t * const pData)
{
    uint16_t tmpTwobyte = 0;
    uint16_t offset = 0;

    offset = index * 2;

    uint16_t higher_byte = *(pData + 1 + offset);
    uint16_t lower_byte = *(pData + offset);
    tmpTwobyte = (higher_byte<< 8) + lower_byte;

    return tmpTwobyte;
}

static uint16_t wearleveling_v2_assembleLastTwoByte(wearleveling_state_typeDef * const pState, uint8_t * const pData)
{
    if (pState == NULL) return 0;
    if (pData == NULL) return 0;

    const uint16_t twoByte = wearleveling_v2_isEvenNumber(pState->params.dataSizeInByte) ? 
        (WEARLEVELING_LIB_EMPTY_FLAG << 8) + WEARLEVELING_LIB_DIRTY_FLAG :
        (WEARLEVELING_LIB_DIRTY_FLAG << 8) + wearleveling_v2_getLastbyte(pState, pData);

    return twoByte;
}

static uint8_t wearleveling_v2_getLastbyte(wearleveling_state_typeDef * const pState, uint8_t * const pData)
{
    if (pState == NULL) return 0;
    if (pData == NULL) return 0;
    return *(pData + pState->params.dataSizeInByte - 1);
}

static uint8_t wearleveling_v2_isFull(wearleveling_state_typeDef * const pState)
{
    if (pState == NULL) return 0;
    return pState->indexBucketWrite >= pState->numOfBuckets ? 1 : 0;
}

static uint8_t wearleveling_v2_isFormated(wearleveling_params_typeDef * const pParam)
{
    if (pParam == NULL) return 0;
    uint16_t formatedFlag = pParam->readTwoByte(0x00);
    return formatedFlag == WEARLEVELING_LIB_FORMATED_FLAG ? 1 : 0;
}

static uint8_t wearleveling_v2_isEvenNumber(uint16_t number)
{
    return ((number % 2) == 0) ? 1 : 0;
}

static void wearleveling_v2_resetIndex(wearleveling_state_typeDef * const pState)
{
    if (pState == NULL) return;
    pState->indexBucketRead = 0;
    pState->indexBucketWrite = 0;
}

static void wearleveling_v2_formatPage(wearleveling_params_typeDef * const pParam)
{
    if (pParam == NULL) return;
    pParam->pageErase();
    pParam->writeTwoByte(0x00, WEARLEVELING_LIB_FORMATED_FLAG);
}

static void wearleveling_v2_updateBuckietIndexReadWrite(wearleveling_state_typeDef * const pState)
{
    if (pState == NULL) return;
    pState->indexBucketWrite++;
    pState->indexBucketRead = wearleveling_v2_findBucketIndexRead(pState);
}

wearleveling_state_typeDef * debug_wearleveling_getInternalState(void)
{
    return &internalState;
}