#include "test.h"
using json = nlohmann::json; //for convenience
using namespace std;

/***
  @brief a class to read json data.
 ***/
class ReadRobotData
{
    public:
        ReadRobotData() {}
        void readData(const string &dataPath, vector<HoleCards> &allHoleCards, vector<vector<int>> &allLast_cards, vector<vector<int>> &allOut_cards)
        {
            ifstream input(dataPath);
            if (!input)
            {
                printf("can't open the data file\n");
                return;
            }

            json j;
            input >> j; 
            for (const auto &typeElements : j)
            {
                for (const auto &typeElement : typeElements)
                { 
                    HoleCards hc;
                    vector<int> last_cards;
                    vector<int> out_cards;
                    for (const auto &value : typeElement["cards"])
                    {
                        hc.add_card(Card(value.get<int>()));
                    }
                    hc.ghost_face = typeElement["diceFace"].get<int>();
                    hc.table_type = typeElement["table_type"].get<int>();
                    hc.landlord_seat = typeElement["landlord_seat"].get<int>();
                    hc.seatid = typeElement["seatid"].get<int>();
                    hc.pre_play_seat = typeElement["pre_play_seat"].get<int>();
                    for (const auto &value : typeElement["last_cards"])
                    {
                        last_cards.push_back(value.get<int>());
                    }
                    for (const auto &value : typeElement["out_cards"])
                    {
                        out_cards.push_back(value.get<int>());
                    }
                    allHoleCards.push_back(hc);
                    allLast_cards.push_back(last_cards);
                    allOut_cards.push_back(out_cards);
                }
            }
        }
};

class RobotTest : public ::testing::Test
{
    protected:
        virtual void SetUp() { //for test case
            readRobotData.readData("./test_data/input_robot_data.json", allHoleCards, allLast_cards, allOut_cards);
            // readRobotData.readData("./test_data/debug_data.json", allHoleCards, allLast_cards, allOut_cards);
            for(unsigned i = 0; i < allHoleCards.size() ; ++i)
            {
                // out << "######################################" << endl;
                allHoleCards[i].split_cards();
                // out << "######################################" << endl;
            }
        }

        virtual void TearDown() {
            allHoleCards.clear();
            allLast_cards.clear();
            allOut_cards.clear();
        }

        static void SetUpTestCase() { //for test suit
        }

        static void TearDownTestCase() {
        }

        ReadRobotData readRobotData;
        vector<HoleCards> allHoleCards; 
        vector<vector<int>> allLast_cards; 
        vector<vector<int>> allOut_cards; 
};

/*
 *@brief compare expected Split Cards and actual splited Cards
         if not equal output message.
 */
testing::AssertionResult compareRobotPlayCards(const char* expr_1, const char* expr_2, const char* expr_3,
        const vector<int> &out_cards, const vector<int> &outCards, const HoleCards &hc) 
{
    if (out_cards == outCards)  //equal
        return testing::AssertionSuccess();

    //not equal ,message formatting.
    testing::Message msg; 
    msg << "HoleCards: ";
    msg << hc << endl;
    msg << "--" << out_cards;

    msg << "    **"; 
    msg << outCards << endl;
    return testing::AssertionFailure(msg);
}

/*
 * play cards test
 */
TEST_F(RobotTest, HandlerPlayGetCards)
{
    for (unsigned i = 0; i < allHoleCards.size(); ++i)
    {
        // out << "HoleCards["<< i << "]: ";
        // out << allHoleCards[i] << endl;
        // out << "last_cards[" << i <<"]: ";
        // out << allLast_cards[i] << endl;

        /*
        vector<int> seatVec;  //记录其它玩家的坐位号
        vector<int> cardLeft; //记录其它玩家的剩余牌数
        seatVec.push_back(1);
        cardLeft.push_back(12);
        seatVec.push_back(0);
        cardLeft.push_back(1);

        allHoleCards[i].set_other_player_info(seatVec, cardLeft);
        */

        vector<int> outCards;
        allHoleCards[i].get_play_cards(allLast_cards[i], outCards);
        // Card::dump_cards(outCards, "outCards");
        EXPECT_PRED_FORMAT3(compareRobotPlayCards, allOut_cards[i], outCards, allHoleCards[i]);
    }
}