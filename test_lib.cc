#include "card.h"
#include "deck.h"
#include "hole_cards.h"
#include "community_cards.h"
#include "card_statistics.h"
#include "card_analysis.h"
#include "card_find.h"

#include "test_data/json.hpp"
// for convenience
using json = nlohmann::json;

#include <fstream>
#include <sstream>
#include <bitset>

using namespace std;

/**
* suit  0  方块  1 梅花   2  红桃    3黑桃
* 	
0x01, 0x11, 0x21, 0x31,		//A 14
0x02, 0x12, 0x22, 0x32,		//2 15
0x03, 0x13, 0x23, 0x33,		//3 3
0x04, 0x14, 0x24, 0x34,		//4 4
0x05, 0x15, 0x25, 0x35,		//5 5
0x06, 0x16, 0x26, 0x36,		//6 6
0x07, 0x17, 0x27, 0x37,		//7 7
0x08, 0x18, 0x28, 0x38,		//8 8
0x09, 0x19, 0x29, 0x39,		//9 9
0x0A, 0x1A, 0x2A, 0x3A,		//10 10
0x0B, 0x1B, 0x2B, 0x3B,		//J 11
0x0C, 0x1C, 0x2C, 0x3C,		//Q 12
0x0D, 0x1D, 0x2D, 0x3D,		//K 13
0x0E,						//小王
0x0F						//大王
* @author luochuanting
*/

static char *card_type_str[] = 
{
	(char*)"CARD_TYPE_ERROR-0", 
	(char*)"CARD_TYPE_ONE-1",
	(char*)"CARD_TYPE_ONELINE-2",
	(char*)"CARD_TYPE_TWO-3",
	(char*)"CARD_TYPE_TWOLINE-4",
	(char*)"CARD_TYPE_THREE-5",
	(char*)"CARD_TYPE_THREELINE-6",
	(char*)"CARD_TYPE_THREEWITHONE-7",
	(char*)"CARD_TYPE_THREEWITHTWO-8",
	(char*)"CARD_TYPE_PLANEWITHONE-9",
	(char*)"CARD_TYPE_PLANEWITHWING-10",
	(char*)"CARD_TYPE_FOURWITHONE-11",
	(char*)"CARD_TYPE_FOURWITHTWO-12",
	(char*)"CARD_TYPE_SOFTBOMB-13",
	(char*)"CARD_TYPE_BOMB-14",
	(char*)"CARD_TYPE_GHOSTBOMB-15",
	(char*)"CARD_TYPE_ROCKET-16"
};

static string face_symbols[] = {
	"0", "o", "e", "3", "4", "5", "6", "7", "8", "9",
	"10", "J", "Q", "K", "A", "2", "XW", "DW"
};

struct AnaResult
{
    std::vector<Card> vc; //需要分析的牌
    int diceFace;    //癞子
    int expertType;  //期望分析出的type
    int expertFace;  //期望分析出的face
    int anaType;     //分析出的type
    int anaFace;     //分析出的face
    bool error_flag; //分析正确还是错误
    vector<int> backUpType; 
    vector<int> backUpFace;
    vector<int> expertbackUpType; 
    vector<int> expertbackUpFace;
    string test_type;
};

struct FindResult
{
    std::vector<Card> vc; //需要对比的牌
    int diceFace;    //癞子
    int compareType;  //对比的type
    int compareFace;  //对比的face
    std::vector<Card> myVc; //可用于查找的牌
    std::vector<vector<Card> > outVc; //分析出的所有牌型
    std::vector<int> outVcFrom;//存储所有找到的牌来源类型
    int expertResult; //期望分析的结果,第一个正确，这个期望就算正确
    bool error_flag; //分析正确还是错误
    string test_type;
    int expertType;  //纯粹为了控件格式，并它的值设置为compareType一样
};

/*
 @brief 格式化输出分析的结果
 @param output[in] 输出的位置
 @param result[in] 分析的结果
*/
void results_format(ostream &output, const FindResult &result)
{
    output << "************************************************" << endl;

    ostringstream stroutput;
    for (const auto &card : result.myVc) stroutput << card.value << ", ";

    string str = "[" + stroutput.str();
    if(str.size() > 2 )str.pop_back(); 
    if(str.size() > 2 )str.pop_back(); //去掉最后的逗号
    str += "]";
    output << "find \"cards\": " << left << str << endl;

    stroutput.str("");
    for (const auto &card : result.vc) stroutput << card.value << ", ";
    str = "[ " + stroutput.str();
    if(str.size() > 2 )str.pop_back(); 
    if(str.size() > 2 )str.pop_back(); //去掉最后的逗号
    str += " ]";
    const int format_width = 26; //格式控制
    output << "compare \"cards\": " << setw(format_width)
           << left << str << " \"diceFace\": " << result.diceFace << endl;

    output << "\"compareType\": " << setw(format_width + 2)
           << left << card_type_str[result.compareType] << " \"compareFace\": "
           << result.compareFace << endl;

    if (result.error_flag)
        output << "\"flag\": " << setw(format_width + 9) << left << "Error"; 
    else
        output << "\"flag\": " << setw(format_width + 9) << left << "Correct";
    output << right << " \"from\": " << result.test_type << endl;

    unsigned int i = 0;
    for (const auto &cards : result.outVc) 
    {
        output << "outVc[" << i++ << "]: ";

        stroutput.str("");
        for (const auto &card : cards) 
        {
            stroutput << card.value << ", ";
        }
        str = "[ " + stroutput.str();
        if (str.size() > 2) str.pop_back();
        if (str.size() > 2) str.pop_back(); //去掉最后的逗号
        str += " ]";
        output << left << setw(format_width + 7) << str << " \"outFrom\"" << card_type_str[result.outVcFrom[i - 1]] << endl;
    }
    if (result.myVc.size() == 0)
        output << "You got No cards here! set it first, please!!!" << endl;
    else if (result.outVc.size() == 0)
    {
        output << "No big cards, give up!!!" << endl;
    }

    output << "************************************************" << endl << endl;
}

/*
 @brief 格式化输出分析的结果
 @param output[in] 输出的位置
 @param result[in] 分析的结果

*/
void results_format(ostream &output, const AnaResult &result)
{
    output << "************************************************" << endl;

    ostringstream stroutput;
    for (const auto &card : result.vc)
    {
        stroutput << card.value << ", ";
    }
    string str = "[ " + stroutput.str();
    if(str.size() > 2 )str.pop_back();
    if(str.size() > 2 )str.pop_back(); //去掉最后的逗号
    str += " ]";
    const int format_width = 20; //格式控制
    output << "analysis \"cards\": " << setw(format_width)
           << left << str << " \"diceFace\": " << result.diceFace << endl;

    output << "\"anaType\": " << setw(format_width + 7)
           << left << card_type_str[result.anaType] << " \"expertType\": "
           << card_type_str[result.expertType] << endl;

    output << "\"anaFace\": " << setw(format_width + 7)
           << left << face_symbols[result.anaFace] << " \"expertFace\": "
           << face_symbols[result.expertFace] << endl;

    stroutput.str("");
    for (const auto &backType : result.backUpType) stroutput << backType << ", ";
    str = stroutput.str();

    stroutput.str("");
    for (const auto &expertbackType : result.expertbackUpType) stroutput << expertbackType << ", ";
    string str2 = stroutput.str();

    if(str.size() > 0 )
    {
        str.pop_back(); str.pop_back(); //去掉最后的逗号
        str = "[" + str + "]";
        output << "\"ana-backUpType\": " << left << setw(format_width) << str;
        if (str2.size() == 0)
            output << endl;
    }

    if(str2.size() > 0 )
    {
        str2.pop_back(); str2.pop_back(); //去掉最后的逗号
        if (str.size() == 0)
            output << "\"ana-backUpType\": " << left << setw(format_width) << str;
        output << " \"expert-backUpType\": [" << str2 << "]" << endl;
    }

    stroutput.str("");
    for (const auto &backFace : result.backUpFace) stroutput << backFace << ", ";
    str = stroutput.str();

    stroutput.str("");
    for (const auto &expertbackFace : result.expertbackUpFace) stroutput << expertbackFace << ", ";
    str2 = stroutput.str();

    if(str.size() > 0 )
    {
        str.pop_back(); str.pop_back(); //去掉最后的逗号
        str = "[" + str + "]";
        output << "\"ana-backUpFace\": " << left << setw(format_width) << str;
        if (str2.size() == 0)
            output << endl;
    }
    if(str2.size() > 0 )
    {
        str2.pop_back(); str2.pop_back(); //去掉最后的逗号
        if (str.size() == 0)
            output << "\"ana-backUpFace\": " << left << setw(format_width) << str;
        output << " \"expert-backUpFace\": " << "[" << str2 << "]" << endl;
    }
        
    if (result.error_flag)
        output << "\"flag\": " << setw(format_width + 10) << left << "Error"; 
    else
        output << "\"flag\": " << setw(format_width + 10) << left << "Correct";
    output << right << " \"from\": " << result.test_type << endl;
    output << "************************************************" << endl << endl;
}

/*
 @brief 输出分析测试的结果
 @param output[in] 输出的位置
 @param results[in] 分析的结果
 @param flag[in] 第一位0x01表示要输出错误的， 第二位0x02表示输要出正确的
 @param output_type 显示位对应的类型, 每位对应一个牌型。 
    CARD_TYPE_ERROR = 0,			// 错误类型
	CARD_TYPE_ONE = 1,			    // 单牌
	CARD_TYPE_ONELINE = 2,		    // 单连牌
	CARD_TYPE_TWO = 3,			    // 对子
	CARD_TYPE_TWOLINE = 4,		    // 连对		3个点数连续的
	CARD_TYPE_THREE = 5,			// 三张		
	CARD_TYPE_THREELINE = 6,		// 三顺		2个以上连续点数
	CARD_TYPE_THREEWITHONE = 7,	    // 三带一  带1个单牌	
	CARD_TYPE_THREEWITHTWO = 8,  	// 三带二  带1个对子
	CARD_TYPE_PLANEWITHONE = 9, 	// 飞机带羿	和三带一差不多
	CARD_TYPE_PLANEWITHWING = 10,	// 飞机带翅	和三带二差不多
	CARD_TYPE_FOURWITHONE = 11, 	// 4个带一 带2个单牌
	CARD_TYPE_FOURWITHTWO = 12, 	// 4个带二 带2个对子
	CARD_TYPE_SOFTBOMB = 13,			// 4个	软炸弹，有癞子
	CARD_TYPE_BOMB = 14,			// 4个		硬炸弹
	CARD_TYPE_GHOSTBOMB = 15,			// 4个	纯癞子炸弹
	CARD_TYPE_ROCKET = 16,		    // 2个鬼	火箭
*/
template <typename T>
void print_results(ostream &output, const vector<T> &results, int flag = 1,
                   const bitset<17> output_type = bitset<17>(~0u))
{
    for (const auto &result : results)
    {
        bool error_flag = result.error_flag;

        if (((flag & 0x01) && error_flag) ||
            ((flag & 0x02) && !error_flag))
        {
            if (output_type.test(result.expertType)) results_format(output, result);
            else if (error_flag) results_format(output, result); //设置了错误输出位，有错误的都输出，不管有没有指定显示。
        }
    }
}

int test_find()
{
    //读取测试文件中的牌型
    ifstream input("test_data/input_find_data.json");
    // ifstream input("test_data/debug_data.json");
    if (!input)
    {
        printf("can't open the data file test_data/input_data.json\n");
        exit(1);
    }
    json j;
    input >> j; //存到json对象中

    ofstream output("test_data/output_data.json");
    output << j.dump(4);

    vector<FindResult> findResults;
    // CardStatistics card_stat;
    // CardAnalysis card_ana;
    CardFind card_find;
    for (auto it = j.begin(); it != j.end(); ++it)
    {
        const auto &typeElements = it.value();
        for (const auto &typeElement : typeElements)
        { //每个类型的每一组数据
            FindResult findResult;
            findResult.test_type = it.key();
            const auto &cards = typeElement["cards"];
            for (const auto &value : cards)
            {
                findResult.vc.push_back(Card(value.get<int>())); //获取设置的牌型
            }
            const auto &myCards = typeElement["myCards"];
            for (const auto &value : myCards)
            {
                findResult.myVc.push_back(Card(value.get<int>())); //获取设置的牌型
            }

            findResult.compareType = typeElement["compareType"].get<int>(); 
            findResult.expertType = typeElement["compareType"].get<int>(); 
            findResult.compareFace = typeElement["compareFace"].get<int>();
            findResult.diceFace = typeElement["diceFace"].get<int>();
            card_find.set_ghost_face(findResult.diceFace);
            card_find.set_robot_flag(true);

            findResult.error_flag = false; //先假设是正确的
            findResult.expertResult = typeElement["expertResult"].get<int>(); 
            if (card_find.tip(findResult.vc, findResult.myVc, findResult.compareType, findResult.compareFace, findResult.diceFace) == 0 && card_find.results.size() > 0)
            {
                if (findResult.expertResult == 0) findResult.error_flag = true; //至少找到一个

                unsigned cnt = 0;
                for (const auto &results : card_find.results)
                { //存储所有找到的牌
                    vector<Card> out;
                    for(const auto &result : results)
                    {
                        out.push_back(result);
                    }
                    findResult.outVc.push_back(out);

                    //存储所有找到的牌来源类型
                    if (findResult.compareType == CARD_TYPE_ONE)
                    {
                        if (std::binary_search(card_find.from_two.begin(), card_find.from_two.end(), cnt))
                        {
                            findResult.outVcFrom.push_back(CARD_TYPE_TWO);
                        }
                        else if( std::binary_search(card_find.from_three.begin(), card_find.from_three.end(), cnt))
                        {
                            findResult.outVcFrom.push_back(CARD_TYPE_THREE);
                        }
                        else
                            findResult.outVcFrom.push_back(findResult.compareType);
                    }
                    else if (findResult.compareType == CARD_TYPE_TWO)
                    {
                        if( std::binary_search(card_find.from_three.begin(), card_find.from_three.end(), cnt))
                        {
                            findResult.outVcFrom.push_back(CARD_TYPE_THREE);
                        }
                        else
                            findResult.outVcFrom.push_back(findResult.compareType);
                    }
                    else
                    {
                        findResult.outVcFrom.push_back(findResult.compareType);
                    }
                    ++cnt;
                }
            }
            else
                if (findResult.expertResult == 1) findResult.error_flag = true; //没找到

            findResults.push_back(findResult);
        }
    }

    ofstream out("data.txt");
    print_results(out, findResults); //检测所有牌型，但只输出错误信息

    ofstream set_out("set_out_data.txt");
    bitset<17> display_type; //设置显示输出的类型
    display_type.set(CARD_TYPE_ERROR);// 错误类型
    display_type.set(CARD_TYPE_ONE);// 单牌
    display_type.set(CARD_TYPE_ONELINE);// 单连牌
    display_type.set(CARD_TYPE_TWO);// 对子
    display_type.set(CARD_TYPE_TWOLINE);// 连对	3个点数连续的
    display_type.set(CARD_TYPE_THREE);// 三张	
    display_type.set(CARD_TYPE_THREELINE);// 三顺 2个以上连续点数
    display_type.set(CARD_TYPE_THREEWITHONE);// 三带一  带1个单牌	
    display_type.set(CARD_TYPE_THREEWITHTWO);// 三带二  带1个对子
    display_type.set(CARD_TYPE_PLANEWITHONE);// 飞机带羿	和三带一差不多
    display_type.set(CARD_TYPE_PLANEWITHWING);// 飞机带翅	和三带二差不多
    display_type.set(CARD_TYPE_FOURWITHONE);// 4个带一 带2个单牌
    display_type.set(CARD_TYPE_FOURWITHTWO);// 4个带二 带2个对子
    display_type.set(CARD_TYPE_SOFTBOMB);// 4个	软炸弹，有癞子
    display_type.set(CARD_TYPE_BOMB);// 4个		硬炸弹
    display_type.set(CARD_TYPE_GHOSTBOMB);// 4个	纯癞子炸弹
    display_type.set(CARD_TYPE_ROCKET);// 2个鬼	火箭
    print_results(set_out, findResults, 0x03, display_type);

    return 0;

}

int test_analysis()
{
//读取测试文件中的牌型
    ifstream input("test_data/input_ana_data.json");
    if (!input)
    {
        printf("can't open the data file test_data/input_data.json\n");
        exit(1);
    }
    json j;
    input >> j; //存到json对象中

    ofstream output("test_data/output_data.json");
    output << j.dump(4);

    vector<AnaResult> anaResults;
    CardAnalysis card_ana;
    for (auto it = j.begin(); it != j.end(); ++it)
    {
        const auto &typeElements = it.value();
        for (const auto &typeElement : typeElements)
        { //每个类型的每一组数据
            AnaResult anaResult;
            anaResult.test_type = it.key();
            const auto &cards = typeElement["cards"];
            for (const auto &value : cards)
            {
                anaResult.vc.push_back(Card(value.get<int>())); //获取设置的牌型
            }
            anaResult.expertType = typeElement["expertType"].get<int>(); 
            anaResult.expertFace = typeElement["expertFace"].get<int>();
            anaResult.diceFace = typeElement["diceFace"].get<int>();
            card_ana.analysis(anaResult.vc, anaResult.diceFace);
            anaResult.anaType = card_ana.type;
            anaResult.anaFace = card_ana.face;
            anaResult.error_flag = false; //先假设是正确的
            if ( (card_ana.type != anaResult.expertType) || (card_ana.face != anaResult.expertFace))
                anaResult.error_flag = true;

            const auto &backUpTypes = typeElement["backUpType"];
            for (const auto &backUpType : backUpTypes)
                anaResult.expertbackUpType.push_back(backUpType.get<int>());

            const auto &backUpFaces = typeElement["backUpFace"];
            for (const auto &backUpFace : backUpFaces)
                anaResult.expertbackUpFace.push_back(backUpFace.get<int>());

            for (const auto &backUpFace : card_ana.back_up_face)
                anaResult.backUpFace.push_back(backUpFace);

            for (const auto &backUpType : card_ana.back_up_type)
                anaResult.backUpType.push_back(backUpType);

            if (anaResult.backUpType.size() == anaResult.expertbackUpType.size() &&
                anaResult.backUpFace.size() == anaResult.expertbackUpFace.size())
            {
                for (unsigned i = 0; i < anaResult.backUpType.size(); ++i)
                {
                    if ((anaResult.backUpType[i] != anaResult.expertbackUpType[i]) ||
                        (anaResult.backUpFace[i] != anaResult.expertbackUpFace[i]))
                    {
                        anaResult.error_flag = true;
                    }
                }
            }
            else
                anaResult.error_flag = true;
            anaResults.push_back(anaResult);
        }
    }

    ofstream out("data.txt");
    print_results(out, anaResults); //检测所有牌型，但只输出错误信息

    ofstream set_out("set_out_data.txt");
    bitset<17> display_type; //设置显示输出的类型
    display_type.set(CARD_TYPE_ERROR);// 错误类型
    display_type.set(CARD_TYPE_ONE);// 单牌
    display_type.set(CARD_TYPE_ONELINE);// 单连牌
    display_type.set(CARD_TYPE_TWO);// 对子
    display_type.set(CARD_TYPE_TWOLINE);// 连对	3个点数连续的
    display_type.set(CARD_TYPE_THREE);// 三张	
    display_type.set(CARD_TYPE_THREELINE);// 三顺 2个以上连续点数
    display_type.set(CARD_TYPE_THREEWITHONE);// 三带一  带1个单牌	
    display_type.set(CARD_TYPE_THREEWITHTWO);// 三带二  带1个对子
    display_type.set(CARD_TYPE_PLANEWITHONE);// 飞机带羿	和三带一差不多
    display_type.set(CARD_TYPE_PLANEWITHWING);// 飞机带翅	和三带二差不多
    display_type.set(CARD_TYPE_FOURWITHONE);// 4个带一 带2个单牌
    display_type.set(CARD_TYPE_FOURWITHTWO);// 4个带二 带2个对子
    display_type.set(CARD_TYPE_SOFTBOMB);// 4个	软炸弹，有癞子
    display_type.set(CARD_TYPE_BOMB);// 4个		硬炸弹
    display_type.set(CARD_TYPE_GHOSTBOMB);// 4个	纯癞子炸弹
    // display_type.set(CARD_TYPE_ROCKET);// 2个鬼	火箭
    print_results(set_out, anaResults, 0x03, display_type);
    return 0;
}

int main(int argc, char *argv[])
{
    // test_analysis();
    test_find();
}
