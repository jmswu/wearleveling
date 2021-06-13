
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

static uint8_t wearleveling_saveDataToAddress(const uint32_t addr, uint8_t * const pData);

static uint16_t wearleveling_calculateBucketSize(void);
static uint16_t wearleveling_calculateNumOfBuckets(void);
static uint32_t wearleveling_calculateAddressFromBucketIndex(uint16_t index);
static uint16_t wearleveling_findBucketIndexRead(void);
static uint16_t wearleveling_findBucketIndexWrite(void);
static uint16_t wearleveling_getTwoByte(uint16_t index, uint8_t * const pData);
static uint16_t wearleveling_assembleLastTwoByte(uint8_t * const pData);
static uint8_t wearleveling_getLastbyte(uint8_t * const pData);
// static uint8_t wearleveling_isEmpty(void);
static uint8_t wearleveling_isFull(void);
static uint8_t wearleveling_isFormated(void);
static uint8_t wearleveling_isEvenNumber(uint16_t number);
static void wearleveling_resetIndex(void);
static void wearleveling_formatPage(void);
static void wearleveling_updateBuckietIndexReadWrite(void);

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
        internalState.indexBucketWrite = wearleveling_findBucketIndexWrite();
        internalState.indexBucketRead = wearleveling_findBucketIndexRead();
    }
    else
    {
        wearleveling_formatPage();
        wearleveling_resetIndex();
    }
}

static uint8_t wearleveling_saveDataToAddress(const uint32_t addr, uint8_t * const pData)
{
    if (pData == NULL) return 0;

    uint16_t tmpTwoBytes;
    uint32_t offset;

    uint16_t numOfCopy = internalState.params.dataSizeInByte >> 1;
    for(uint16_t i = 0; i < numOfCopy; i++)
    {
        offset = i * 2;
        tmpTwoBytes = wearleveling_getTwoByte(i, pData);
        if (internalState.params.writeTwoByte(addr + offset, tmpTwoBytes) == 0) return 0;
    }

    offset = numOfCopy * 2;
    tmpTwoBytes = wearleveling_assembleLastTwoByte(pData);
    if (internalState.params.writeTwoByte(addr + offset, tmpTwoBytes) == 0) return 0;

    return 1;
}

static uint8_t wearleveling_save(uint8_t * const pData)
{
    if (pData == NULL) return 0;

    if (wearleveling_isFull())
    {
        wearleveling_formatPage();
        wearleveling_resetIndex();
    }

    const uint32_t ADDRESS = wearleveling_calculateAddressFromBucketIndex(internalState.indexBucketWrite);
    wearleveling_updateBuckietIndexReadWrite();
    return wearleveling_saveDataToAddress(ADDRESS, pData);
}

static uint8_t wearleveling_read(uint8_t * const pData)
{
    if (pData == NULL) return 0;

    const uint32_t ADDR_TO_READ = wearleveling_calculateAddressFromBucketIndex(internalState.indexBucketRead);
    const uint16_t NUM_OF_READ = internalState.params.dataSizeInByte >> 1;
    uint16_t tmpTwoByte = 0;

    printf("addr to read: %02x, numOfRead: %u\n", ADDR_TO_READ, NUM_OF_READ);

    for(uint16_t i = 0; i < NUM_OF_READ; i++)
    {
        tmpTwoByte = internalState.params.readTwoByte(ADDR_TO_READ + (i * 2));
        uint16_t index = i * 2;
        pData[index] = (uint8_t)(tmpTwoByte);
        pData[index + 1] = (uint8_t)(tmpTwoByte >> 8);
        printf("twoByte: %02X[%u], %02X[%u]\n", pData[index], index , pData[index + 1], index + 1);
    }

    tmpTwoByte = internalState.params.readTwoByte(ADDR_TO_READ + (NUM_OF_READ * 2));

    if (wearleveling_isEvenNumber(internalState.params.dataSizeInByte) == 0)
    {
        pData[NUM_OF_READ * 2] = (uint8_t)tmpTwoByte;
        printf("lastByte: %02X[%u]\n", pData[NUM_OF_READ * 2], NUM_OF_READ * 2);
    }

    return 1;
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

static uint16_t wearleveling_findBucketIndexRead(void)
{
    uint16_t writeIndex = internalState.indexBucketWrite;
    return writeIndex == 0 ? 0 : writeIndex - 1;
}

static uint16_t wearleveling_findBucketIndexWrite(void)
{
    for(uint16_t i = 0; i < internalState.numOfBuckets; i++)
    {
        const uint32_t ADDRESS_OF_NEXT_BUCKET = wearleveling_calculateAddressFromBucketIndex(i + 1);
        const uint16_t LAST_TWO_BYTES = internalState.params.readTwoByte(ADDRESS_OF_NEXT_BUCKET - 2);

        uint8_t dirtyFlag = 0;
        if (wearleveling_isEvenNumber(internalState.params.dataSizeInByte))
        {
            dirtyFlag = (uint8_t)(LAST_TWO_BYTES);
        }
        else
        {
            dirtyFlag = (uint8_t)(LAST_TWO_BYTES >> 8);
        }

        if ((dirtyFlag != WEARLEVELING_LIB_DIRTY_FLAG) && (dirtyFlag == WEARLEVELING_LIB_EMPTY_FLAG))
        {
            return i;
        }
    }

    return 0;
}

static uint16_t wearleveling_getTwoByte(uint16_t index, uint8_t * const pData)
{
    uint16_t tmpTwobyte = 0;
    uint16_t offset = 0;

    offset = index * 2;

    uint16_t higher_byte = *(pData + 1 + offset);
    uint16_t lower_byte = *(pData + offset);
    tmpTwobyte = (higher_byte<< 8) + lower_byte;

    return tmpTwobyte;
}

static uint16_t wearleveling_assembleLastTwoByte(uint8_t * const pData)
{
    if (pData == NULL) return 0;
    uint16_t twoByte = 0;
    if (wearleveling_isEvenNumber(internalState.params.dataSizeInByte))
    {
        twoByte = (WEARLEVELING_LIB_EMPTY_FLAG << 8) + WEARLEVELING_LIB_DIRTY_FLAG;
    }
    else
    {
        twoByte = (WEARLEVELING_LIB_DIRTY_FLAG << 8) + wearleveling_getLastbyte(pData);
    }
    return twoByte;
}

static uint8_t wearleveling_getLastbyte(uint8_t * const pData)
{
    if (pData == NULL) return 0;
    return *(pData + internalState.params.dataSizeInByte - 1);
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

static uint8_t wearleveling_isFull(void)
{
    return internalState.indexBucketWrite >= internalState.numOfBuckets ? 1 : 0;
}

static uint8_t wearleveling_isFormated(void)
{
    uint16_t formatedFlag = internalState.params.readTwoByte(0x00);
    return formatedFlag == WEARLEVELING_LIB_FORMATED_FLAG ? 1 : 0;
}

static uint8_t wearleveling_isEvenNumber(uint16_t number)
{
    return ((number % 2) == 0) ? 1 : 0;
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

static void wearleveling_updateBuckietIndexReadWrite(void)
{
    internalState.indexBucketWrite++;
    internalState.indexBucketRead = wearleveling_findBucketIndexRead();
}


wearleveling_state_typeDef * debug_wearleveling_getInternalState(void)
{
    return &internalState;
}