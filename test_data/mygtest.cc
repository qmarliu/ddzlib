#include <iostream>
#include <vector>

class Manage;
class Test {
    friend class Manage;
    protected:
        Test(const char *caseName) : testCaseName(caseName){}

        virtual void run() = 0;

        bool isSucceed; //记录这个Test有没有执行成功
        
        const char * testCaseName;
};


/***
 * @brief 管理所有Test
 ***/
class Manage {
    private:
        Manage() {}
    public:
        static Manage const*getInstance()
        {
            static Manage singleton;
            return &singleton;
        }

        static void RegisterTest(Test *test)
        {
            allTest.push_back(test);
        }

        void run()
        {
            isSucceed = false;
            for(auto test: allTest)
            {
                test->run();
                if (!test->isSucceed)
                {
                    ++succeedCnt;
                }
                else
                {
                    ++failedCnt;
                    isSucceed = false;
                }
            }

            //输出结果
            std::cout << "======================================" << std::endl;
            std::cout << "succeed: " << succeedCnt << std::endl;
            std::cout << "failure: " << failedCnt << std::endl;
            if (isSucceed)
                std::cout << "--test all passed!!!---" << std::endl;
            else
                std::cout << "--test has failure!!!---" << std::endl;
        }


    private:
        static std::vector<Test *> allTest;

        unsigned succeedCnt;
        unsigned failedCnt;
        bool isSucceed;
};

//初始化静态变量
std::vector<Test *> Manage::allTest;

#define GET_TEST_NAME(testName) \
    Test_##testName

#define MYTEST_(testName) \
    class GET_TEST_NAME(testName) : public Test { \
        public: \
            GET_TEST_NAME(testName)(const char *testcase) : Test(testcase) {}\
            virtual void run();\
    };\
    virtual void GET_TEST_NAME(testName)::run()

#define MYTEST(testName) \
    MYTEST_(testName)

MYTEST(testName)
{

}

int main()
{
    return 0;
}


