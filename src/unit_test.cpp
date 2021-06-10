#include <stdint.h>
#include "gtest/gtest.h"
#include "wearleveling.h"

const uint16_t PAGE_SIZE = 1024;
static uint8_t page[PAGE_SIZE];

uint8_t mock_pageErase(void);
uint8_t mock_writeTwoByte(uint32_t addr, uint16_t data);
uint16_t mock_readTwoByte(uint32_t addr);
namespace wearlevelingLibraryTest
{
    class wearlevelingLibraryTest:public::testing::Test
    {
        public:
        private:
        protected:

        wearlevelingLibraryTest(){}
        virtual ~wearlevelingLibraryTest(){}

        virtual void SetUp(){}
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
}


uint8_t mock_pageErase(void)
{
    memset((void *)page, 0xFF, PAGE_SIZE);
    return 1;
}

uint8_t mock_writeTwoByte(uint32_t addr, uint16_t data)
{
    if (addr >= PAGE_SIZE) return 0;
    
    page[addr + 0] = (uint8_t)(data & 0xFF);
    page[addr + 1] = (uint8_t)((data >> 8) & 0xFF);

    return 1;
}

uint16_t mock_readTwoByte(uint32_t addr)
{
    if (addr >= PAGE_SIZE) return 0;

    uint16_t retval = page[addr] + (page[addr + 1] << 8);

    return retval;
}