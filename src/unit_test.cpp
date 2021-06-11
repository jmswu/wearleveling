#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "gtest/gtest.h"
#include "wearleveling.h"

const uint16_t PAGE_SIZE = 1024;
static uint8_t page[PAGE_SIZE] = {0};

uint8_t mock_pageErase(void);
uint8_t mock_formatPage(void);
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

    TEST_F(wearlevelingLibraryTest, init_assignment_1)
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
    }

    TEST_F(wearlevelingLibraryTest, init_bucksize_1)
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
        
        ASSERT_EQ(PAGE_SIZE, pDebugData->params.pageCapacityInByte);
        ASSERT_EQ(3, pDebugData->params.dataSizeInByte);
        ASSERT_EQ(4, pDebugData->bucketSize);
    }

    TEST_F(wearlevelingLibraryTest, init_bucksize_2)
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
        
        ASSERT_EQ(PAGE_SIZE, pDebugData->params.pageCapacityInByte);
        ASSERT_EQ(4, pDebugData->params.dataSizeInByte);
        ASSERT_EQ(6, pDebugData->bucketSize);
    }

    TEST_F(wearlevelingLibraryTest, init_bucksize_3)
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
        
        ASSERT_EQ(PAGE_SIZE, pDebugData->params.pageCapacityInByte);
        ASSERT_EQ(35, pDebugData->params.dataSizeInByte);
        ASSERT_EQ(36, pDebugData->bucketSize);
    }

    TEST_F(wearlevelingLibraryTest, init_bucksize_4)
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
        
        ASSERT_EQ(512, pDebugData->params.pageCapacityInByte);
        ASSERT_EQ(35, pDebugData->params.dataSizeInByte);
        ASSERT_EQ(36, pDebugData->bucketSize);
    }

    TEST_F(wearlevelingLibraryTest, init_numOfBuckets_1)
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
        
        ASSERT_EQ(512, pDebugData->params.pageCapacityInByte);
        ASSERT_EQ(35, pDebugData->params.dataSizeInByte);
        ASSERT_EQ(36, pDebugData->bucketSize);
        ASSERT_EQ(14, pDebugData->numOfBuckets);
    }

    TEST_F(wearlevelingLibraryTest, init_numOfBuckets_2)
    {
        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = 512,
            .dataSizeInByte = 101,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        mock_pageErase();
        wearleveling.init(&params);

        wearleveling_state_typeDef * const pDebugData = debug_wearleveling_getInternalState();
        
        ASSERT_EQ(512, pDebugData->params.pageCapacityInByte);
        ASSERT_EQ(101, pDebugData->params.dataSizeInByte);
        ASSERT_EQ(102, pDebugData->bucketSize);
        ASSERT_EQ(5, pDebugData->numOfBuckets);
    }

    TEST_F(wearlevelingLibraryTest, init_numOfBuckets_3)
    {
        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = 512,
            .dataSizeInByte = 2,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        mock_pageErase();
        wearleveling.init(&params);

        wearleveling_state_typeDef * const pDebugData = debug_wearleveling_getInternalState();
        
        ASSERT_EQ(512, pDebugData->params.pageCapacityInByte);
        ASSERT_EQ(2, pDebugData->params.dataSizeInByte);
        ASSERT_EQ(4, pDebugData->bucketSize);
        ASSERT_EQ(127, pDebugData->numOfBuckets);
    }

    TEST_F(wearlevelingLibraryTest, init_numOfBuckets_4)
    {
        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = 512,
            .dataSizeInByte = 15,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        mock_pageErase();
        wearleveling.init(&params);

        wearleveling_state_typeDef * const pDebugData = debug_wearleveling_getInternalState();
        
        ASSERT_EQ(512, pDebugData->params.pageCapacityInByte);
        ASSERT_EQ(15, pDebugData->params.dataSizeInByte);
        ASSERT_EQ(16, pDebugData->bucketSize);
        ASSERT_EQ(31, pDebugData->numOfBuckets);
    }

    TEST_F(wearlevelingLibraryTest, init_formatPage_1)
    {
        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = 1024,
            .dataSizeInByte = 15,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        mock_pageErase();
        wearleveling.init(&params);
        ASSERT_EQ(0x34, page[0]);
        ASSERT_EQ(0x12, page[1]);
        ASSERT_EQ(0xFF, page[2]);
    }

    TEST_F(wearlevelingLibraryTest, init_formatPage_2)
    {
        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = 1024,
            .dataSizeInByte = 15,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        mock_pageErase();
        mock_formatPage();
        wearleveling.init(&params);
        ASSERT_EQ(0x34, page[0]);
        ASSERT_EQ(0x12, page[1]);
        ASSERT_EQ(0x01, page[2]);   /* 0x01 is introfuce by mock_formatPage(), it indicats the wearleveling internal format is not called */
    }

    TEST_F(wearlevelingLibraryTest, save_1)
    {
        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = 1024,
            .dataSizeInByte = 1,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        uint8_t dummy_data [] = {0x11};

        mock_pageErase();
        wearleveling.init(&params);
        wearleveling.save(dummy_data);

        ASSERT_EQ(0x34, page[0]);
        ASSERT_EQ(0x12, page[1]); 

        ASSERT_EQ(0x11, page[2]);
        ASSERT_EQ(0x55, page[3]); 
    }

    TEST_F(wearlevelingLibraryTest, save_2)
    {
        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = 1024,
            .dataSizeInByte = 2,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        uint8_t dummy_data [] = {0x11, 0x22};

        mock_pageErase();
        wearleveling.init(&params);
        wearleveling.save(dummy_data);

        ASSERT_EQ(0x34, page[0]);
        ASSERT_EQ(0x12, page[1]);

        ASSERT_EQ(0x11, page[2]);
        ASSERT_EQ(0x22, page[3]); 
        ASSERT_EQ(0x55, page[4]);
        ASSERT_EQ(0xFF, page[5]); 
    }

    TEST_F(wearlevelingLibraryTest, save_3)
    {
        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = 1024,
            .dataSizeInByte = 3,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        uint8_t dummy_data [] = {0x11, 0x22, 0x33};

        mock_pageErase();
        wearleveling.init(&params);
        wearleveling.save(dummy_data);

        ASSERT_EQ(0x12, page[1]);
        ASSERT_EQ(0x34, page[0]);

        ASSERT_EQ(0x11, page[2]);
        ASSERT_EQ(0x22, page[3]); 
        ASSERT_EQ(0x33, page[4]);
        ASSERT_EQ(0x55, page[5]); 
    }

    TEST_F(wearlevelingLibraryTest, save_4)
    {
        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = 1024,
            .dataSizeInByte = 10,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        uint8_t dummy_data [] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA};

        mock_pageErase();
        wearleveling.init(&params);
        wearleveling.save(dummy_data);

        ASSERT_EQ(0x12, page[1]);
        ASSERT_EQ(0x34, page[0]);

        ASSERT_EQ(0x11, page[2]);
        ASSERT_EQ(0x22, page[3]); 
        ASSERT_EQ(0x33, page[4]);
        ASSERT_EQ(0x44, page[5]);
        ASSERT_EQ(0x55, page[6]); 
        ASSERT_EQ(0x66, page[7]); 
        ASSERT_EQ(0x77, page[8]); 
        ASSERT_EQ(0x88, page[9]); 
        ASSERT_EQ(0x99, page[10]); 
        ASSERT_EQ(0xAA, page[11]); 
    }

    TEST_F(wearlevelingLibraryTest, save_5_two_entry)
    {
        wearleveling_state_typeDef * const pDebugData = debug_wearleveling_getInternalState();

        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = 1024,
            .dataSizeInByte = 3,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        uint8_t dummy_data [] = {0x11, 0x22, 0x33};

        mock_pageErase();
        wearleveling.init(&params);

        ASSERT_EQ(0, pDebugData->indexBucketWrite);
        wearleveling.save(dummy_data);
        ASSERT_EQ(1, pDebugData->indexBucketWrite);
        wearleveling.save(dummy_data);

        ASSERT_EQ(2, pDebugData->indexBucketWrite);

        ASSERT_EQ(0x12, page[1]);
        ASSERT_EQ(0x34, page[0]);

        /* first save */
        ASSERT_EQ(0x11, page[2]);
        ASSERT_EQ(0x22, page[3]); 
        ASSERT_EQ(0x33, page[4]);
        ASSERT_EQ(0x55, page[5]); 

        /* second save */
        ASSERT_EQ(0x11, page[6]);
        ASSERT_EQ(0x22, page[7]); 
        ASSERT_EQ(0x33, page[8]);
        ASSERT_EQ(0x55, page[9]); 
    }

    TEST_F(wearlevelingLibraryTest, save_6_mutiple_entry)
    {
        wearleveling_state_typeDef * const pDebugData = debug_wearleveling_getInternalState();

        wearleveling_params_typeDef params = 
        {
            .pageCapacityInByte = 512,
            .dataSizeInByte = 64,
            .readTwoByte = mock_readTwoByte,
            .writeTwoByte = mock_writeTwoByte,
            .pageErase = mock_pageErase,
        };

        uint8_t dummy_data1 [] = {
                0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
                0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
                0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
                0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
            };

        uint8_t dummy_data2 [] = {
                0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
                0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
                0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
                0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
            };

        mock_pageErase();
        wearleveling.init(&params);

        ASSERT_EQ(0, pDebugData->indexBucketWrite);
        wearleveling.save(dummy_data1);
        ASSERT_EQ(1, pDebugData->indexBucketWrite);
        wearleveling.save(dummy_data1);
        ASSERT_EQ(2, pDebugData->indexBucketWrite);
        wearleveling.save(dummy_data1);
        ASSERT_EQ(3, pDebugData->indexBucketWrite);
        wearleveling.save(dummy_data1);
        ASSERT_EQ(4, pDebugData->indexBucketWrite);
        wearleveling.save(dummy_data1);
        ASSERT_EQ(5, pDebugData->indexBucketWrite);
        wearleveling.save(dummy_data1);
        ASSERT_EQ(6, pDebugData->indexBucketWrite);
        wearleveling.save(dummy_data1);
        ASSERT_EQ(7, pDebugData->indexBucketWrite);

        ASSERT_EQ(0x12, page[1]);
        ASSERT_EQ(0x34, page[0]);
        ASSERT_EQ(0x00, page[2]);
        ASSERT_EQ(0x11, page[3]);

        ASSERT_EQ(0x00, page[66 * 1 + 2]);
        ASSERT_EQ(0x11, page[66 * 1 + 3]);

        ASSERT_EQ(0x00, page[66 * 2 + 2]);
        ASSERT_EQ(0x11, page[66 * 2 + 3]);

        ASSERT_EQ(0x00, page[66 * 3 + 2]);
        ASSERT_EQ(0x11, page[66 * 3 + 3]);

        ASSERT_EQ(0x00, page[66 * 4 + 2]);
        ASSERT_EQ(0x11, page[66 * 4 + 3]);

        ASSERT_EQ(0x00, page[66 * 5 + 2]);
        ASSERT_EQ(0x11, page[66 * 5 + 3]);

        ASSERT_EQ(0x00, page[66 * 6 + 2]);
        ASSERT_EQ(0x11, page[66 * 6 + 3]);

        wearleveling.save(dummy_data2);
        ASSERT_EQ(1, pDebugData->indexBucketWrite);

        ASSERT_EQ(0x12, page[1]);
        ASSERT_EQ(0x34, page[0]);
        ASSERT_EQ(0x55, page[2]);
        ASSERT_EQ(0x55, page[3]);
    }
}


uint8_t mock_pageErase(void)
{
    memset((void *)page, 0xFF, PAGE_SIZE);
    return 1;
}

uint8_t mock_formatPage(void)
{
    page[0] = 0x34;
    page[1] = 0x12;
    page[2] = 0x01;
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
