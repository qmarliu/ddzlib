#include <gtest/gtest.h>
#include <iostream>
#include <deque>
#include <queue>
#include "./test_data/splitHoleCardsAndRobotPlayTest.hpp" 
#include "./test_data/robotFollowPlayCardTest.hpp"

using namespace std;

#if 0
class FooEnvironment : public testing::Environment
{
	/***
	  要实现全局事件，必须写一个类，继承testing::Environment类，实现里面的SetUp和TearDown方法。
	  1. SetUp()方法在所有案例执行前执行
	  2. TearDown()方法在所有案例执行后执行
	  当然，这样还不够，我们还需要告诉gtest添加这个全局事件，我们需要在main函数中通过testing::AddGlobalTestEnvironment方法将事件挂进来，也就是说，我们可以写很多个这样的类，然后将他们的事件都挂上去。
	  testing::AddGlobalTestEnvironment(new FooEnvironment);
	 ***/
	public:

        virtual void SetUp()
        {
            std::cout << "Foo FooEnvironment SetUP" << std::endl;
        }


        virtual void TearDown()
        {
            std::cout << "Foo FooEnvironment TearDown" << std::endl;
        }
};
#endif

int main(int argc, char *argv[])
{
    //testing::AddGlobalTestEnvironment(new FooEnvironment());
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
