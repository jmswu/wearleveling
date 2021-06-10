#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "gtest/gtest.h"
#include "wearleveling.h"

const uint16_t PAGE_SIZE = 1024;
static uint8_t page[PAGE_SIZE] = {0};

uint8_t mock_pageErase(void);
uint8_t mock_writeTwoByte(uint32_t addr, uint16_t data);
uint16_t mock_readTwoByte(uint32_t addr);
namespace wearlevelingLibraryTest
{
    class wearlevelingLibraryTest:public::testing::Test
    {
        public:
        private:
            void randomizePageData(void)
            {
                for(unsigned i = 0; i < PAGE_SIZE; i++)
                {
                    page[i] = (uint8_t)rand();
                }
            }
        protected:

            wearlevelingLibraryTest()
            {
                srand(time(NULL));
            }

            virtual ~wearlevelingLibraryTest(){}

            virtual void SetUp()
            {
                randomizePageData();
            }
            virtual void TearDown(){}
    };

    TEST_F(wearlevelingLibraryTest, mock_erase)
    {
        mock_pageErase();
        for(unsigned i = 0; i < PAGE_SIZE; i++)
        {
            ASSERT_EQ(0xFF, page[i]);
        }
    }

    TEST_F(wearlevelingLibraryTest, mock_write_1)
    {
        mock_pageErase();
        ASSERT_EQ(0, mock_writeTwoByte(PAGE_SIZE, 0x00));
        ASSERT_EQ(0xFF, page[0]);
        ASSERT_EQ(0xFF, page[1]);
    }

    TEST_F(wearlevelingLibraryTest, mock_write_2)
    {
        mock_pageErase();
        ASSERT_EQ(1, mock_writeTwoByte(0, 0x1234));
        ASSERT_EQ(0x34, page[0]);
        ASSERT_EQ(0x12, page[1]);
    }

    TEST_F(wearlevelingLibraryTest, mock_write_3)
    {
        mock_pageErase();
        ASSERT_EQ(1, mock_writeTwoByte(0, 0x1234));
        ASSERT_EQ(0, mock_writeTwoByte(1, 0x1234));
        ASSERT_EQ(1, mock_writeTwoByte(2, 0x1234));
        ASSERT_EQ(0, mock_writeTwoByte(3, 0x1234));
        ASSERT_EQ(1, mock_writeTwoByte(4, 0x1234));
        ASSERT_EQ(0, mock_writeTwoByte(PAGE_SIZE, 0x1234));
    }

    TEST_F(wearlevelingLibraryTest, mock_read_1)
    {
        mock_pageErase();
        ASSERT_EQ(0xFFFF,   mock_readTwoByte(0));
        ASSERT_EQ(0,        mock_readTwoByte(1));
        ASSERT_EQ(0xFFFF,   mock_readTwoByte(2));
        ASSERT_EQ(0,        mock_readTwoByte(3));
        ASSERT_EQ(0xFFFF,   mock_readTwoByte(4));
        ASSERT_EQ(0,        mock_readTwoByte(PAGE_SIZE));
    }

    TEST_F(wearlevelingLibraryTest, mock_read_2)
    {
        mock_pageErase();
        ASSERT_EQ(0xFFFF, mock_readTwoByte(0));   
        mock_writeTwoByte(0, 0x1234);
        ASSERT_EQ(0x1234, mock_readTwoByte(0));  
    }

    TEST_F(wearlevelingLibraryTest, init_1)
    {
        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = PAGE_SIZE,
            .dataSizeInByte = 10,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        mock_pageErase();
        wearleveling.init(&params);

        wearleveling_state_typeDef * const pDebugData = debug_wearleveling_getInternalState();
        
        ASSERT_EQ(mock_pageErase, pDebugData->params.pageErase);
        ASSERT_EQ(mock_writeTwoByte, pDebugData->params.writeTwoByte);
        ASSERT_EQ(mock_readTwoByte, pDebugData->params.readTwoByte);
        ASSERT_EQ(PAGE_SIZE, pDebugData->params.pageCapacityInByte);
        ASSERT_EQ(10, pDebugData->params.dataSizeInByte);
        ASSERT_EQ(12, pDebugData->bucketSize);
        ASSERT_EQ(0, pDebugData->indexBucketRead);
        ASSERT_EQ(0, pDebugData->indexBucketWrite);
        ASSERT_EQ(85, pDebugData->numOfBuckets);
    }

    TEST_F(wearlevelingLibraryTest, init_2)
    {
        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = PAGE_SIZE,
            .dataSizeInByte = 3,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        mock_pageErase();
        wearleveling.init(&params);

        wearleveling_state_typeDef * const pDebugData = debug_wearleveling_getInternalState();
        
        ASSERT_EQ(mock_pageErase, pDebugData->params.pageErase);
        ASSERT_EQ(mock_writeTwoByte, pDebugData->params.writeTwoByte);
        ASSERT_EQ(mock_readTwoByte, pDebugData->params.readTwoByte);
        ASSERT_EQ(PAGE_SIZE, pDebugData->params.pageCapacityInByte);
        ASSERT_EQ(3, pDebugData->params.dataSizeInByte);
        ASSERT_EQ(4, pDebugData->bucketSize);
        ASSERT_EQ(0, pDebugData->indexBucketRead);
        ASSERT_EQ(0, pDebugData->indexBucketWrite);
        ASSERT_EQ(255, pDebugData->numOfBuckets);
    }

    TEST_F(wearlevelingLibraryTest, init_3)
    {
        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = PAGE_SIZE,
            .dataSizeInByte = 4,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        mock_pageErase();
        wearleveling.init(&params);

        wearleveling_state_typeDef * const pDebugData = debug_wearleveling_getInternalState();
        
        ASSERT_EQ(mock_pageErase, pDebugData->params.pageErase);
        ASSERT_EQ(mock_writeTwoByte, pDebugData->params.writeTwoByte);
        ASSERT_EQ(mock_readTwoByte, pDebugData->params.readTwoByte);
        ASSERT_EQ(PAGE_SIZE, pDebugData->params.pageCapacityInByte);
        ASSERT_EQ(4, pDebugData->params.dataSizeInByte);
        ASSERT_EQ(6, pDebugData->bucketSize);
        ASSERT_EQ(0, pDebugData->indexBucketRead);
        ASSERT_EQ(0, pDebugData->indexBucketWrite);
        ASSERT_EQ(170, pDebugData->numOfBuckets);
    }

    TEST_F(wearlevelingLibraryTest, init_4)
    {
        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = PAGE_SIZE,
            .dataSizeInByte = 35,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        mock_pageErase();
        wearleveling.init(&params);

        wearleveling_state_typeDef * const pDebugData = debug_wearleveling_getInternalState();
        
        ASSERT_EQ(mock_pageErase, pDebugData->params.pageErase);
        ASSERT_EQ(mock_writeTwoByte, pDebugData->params.writeTwoByte);
        ASSERT_EQ(mock_readTwoByte, pDebugData->params.readTwoByte);
        ASSERT_EQ(PAGE_SIZE, pDebugData->params.pageCapacityInByte);
        ASSERT_EQ(35, pDebugData->params.dataSizeInByte);
        ASSERT_EQ(36, pDebugData->bucketSize);
        ASSERT_EQ(0, pDebugData->indexBucketRead);
        ASSERT_EQ(0, pDebugData->indexBucketWrite);
        ASSERT_EQ(28, pDebugData->numOfBuckets);
    }

    TEST_F(wearlevelingLibraryTest, init_5)
    {
        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = 512,
            .dataSizeInByte = 35,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        mock_pageErase();
        wearleveling.init(&params);

        wearleveling_state_typeDef * const pDebugData = debug_wearleveling_getInternalState();
        
        ASSERT_EQ(mock_pageErase, pDebugData->params.pageErase);
        ASSERT_EQ(mock_writeTwoByte, pDebugData->params.writeTwoByte);
        ASSERT_EQ(mock_readTwoByte, pDebugData->params.readTwoByte);
        ASSERT_EQ(512, pDebugData->params.pageCapacityInByte);
        ASSERT_EQ(35, pDebugData->params.dataSizeInByte);
        ASSERT_EQ(36, pDebugData->bucketSize);
        ASSERT_EQ(0, pDebugData->indexBucketRead);
        ASSERT_EQ(0, pDebugData->indexBucketWrite);
        ASSERT_EQ(14, pDebugData->numOfBuckets);
    }
}


uint8_t mock_pageErase(void)
{
    memset((void *)page, 0xFF, PAGE_SIZE);
    return 1;
}

uint8_t mock_writeTwoByte(uint32_t addr, uint16_t data)
{
    if (addr >= PAGE_SIZE) return 0;
    if (addr % 2) return 0;
    
    page[addr + 0] = (uint8_t)(data & 0xFF);
    page[addr + 1] = (uint8_t)((data >> 8) & 0xFF);

    return 1;
}

uint16_t mock_readTwoByte(uint32_t addr)
{
    if (addr >= PAGE_SIZE) return 0;
    if (addr % 2) return 0;

    uint16_t retval = page[addr] + (page[addr + 1] << 8);

    return retval;
}