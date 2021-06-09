#include "gtest/gtest.h"
#include "wearleveling.h"

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
        ASSERT_EQ(0, wearleveling_returnOne());
    }
}