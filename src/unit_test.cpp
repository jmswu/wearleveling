#include <stdint.h>
#include "gtest/gtest.h"
#include "wearleveling.h"

const uint16_t PAGE_SIZE = 1024;
static uint8_t page1[PAGE_SIZE];
static uint8_t page2[PAGE_SIZE];

uint8_t mock_pageErase(uint16_t pageNum);
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

    TEST_F(wearlevelingLibraryTest, dummyTest)
    {
        ASSERT_EQ(0x00000001, wearleveling_getVersionNumber());
    }
}


uint8_t mock_pageErase(uint16_t pageNum)
{

}

uint8_t mock_writeTwoByte(uint32_t addr, uint16_t data)
{

}

uint16_t mock_readTwoByte(uint32_t addr)
{
    
}