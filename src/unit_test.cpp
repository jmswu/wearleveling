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

        virtual void SetUp(){
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