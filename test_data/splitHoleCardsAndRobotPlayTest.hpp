#include "test.h"
using json = nlohmann::json; //for convenience
using namespace std;

/***
  @brief a class to read json data.
 ***/
class ReadSplitAndSelfOutCards
{
    public:
        ReadSplitAndSelfOutCards() {}
        void readData(string dataPath, vector<vector<vector<Card>>> &allSplitedCards, vector<HoleCards> &allHoleCards, vector<vector<Card>> &allSelfOut)
        {
            ifstream input(dataPath);
            if (!input)
            {
                printf("can't open the data file test_data/input_data.json\n");
                return;
            }

            json j;
            input >> j; 
            for (const auto &typeElements : j)
            {
                for (const auto &typeElement : typeElements)
                { 
                    vector<vector<Card>> splitedCards;
                    HoleCards cards;
                    vector<Card> selfOut;
                    for (const auto &value : typeElement["cards"])
                    {
                        cards.add_card(Card(value.get<int>()));
                    }
                    for (const auto &splitCards : typeElement["splits"])
                    {//expect splited cards
                        vector<Card> spCards;
                        for (const auto &splitCard : splitCards)
                        {
                            spCards.push_back(Card(splitCard.get<int>()));
                        }
                        splitedCards.push_back(spCards);
                    }
                    cards.ghost_face = typeElement["diceFace"].get<int>();
                    for (const auto &value : typeElement["selfOut"])
                    { 
                        selfOut.push_back(Card(value.get<int>()));
                    }
                    allSplitedCards.push_back(splitedCards);
                    allHoleCards.push_back(cards);
                    allSelfOut.push_back(selfOut);
                }
            }
        }

        void debug(const vector<vector<vector<Card>>> &allSplitedCards, const vector<HoleCards> &allHoleCards,const vector<vector<Card>> &allSelfOut)
        {
            for (unsigned i=0; i<allSplitedCards.size(); ++i)
            {
                out << "************ " << i <<  " ************" << endl;
                for (const auto & card : allHoleCards[i].cards)
                {
                    out << card.first << " ";
                }
                out << endl;
                out << "diceFace: " << allHoleCards[i].ghost_face << endl;
                out << "splitCards: " << endl;
                for (const auto & card : allSplitedCards[i])
                {
                    out << "-";
                    for (const auto & c : card)
                    {
                        out << c.value << " ";
                    }
                    out << endl;
                }
                out << endl;
                cout << "selfOut: ";
                for (const auto & card : allSelfOut[i])
                {
                    out << card.value << " ";
                }
                out << endl;
            }
        }
};

class SplitCardsAndSelfOutTest : public ::testing::Test
{
    protected:
        virtual void SetUp() { //for test case
        }

        virtual void TearDown() {
        }

        static void SetUpTestCase() { //for test suit
            readSplitAndSelfOutCards.readData("./test_data/input_split_data.json", allSplitedCards, allHoleCards, allSelfOut);
            for(unsigned i = 0; i < allHoleCards.size() ; ++i)
            {
                // out << "######################################" << endl;
                allHoleCards[i].split_cards();
                // out << "######################################" << endl;
            }
            // readSplitAndSelfOutCards.debug(allSplitedCards, allHoleCards, allSelfOut);
        }

        static void TearDownTestCase() {
            allSplitedCards.clear();
            allHoleCards.clear();
            allSelfOut.clear();
        }

        static ReadSplitAndSelfOutCards readSplitAndSelfOutCards;

        static vector<vector<vector<Card>>> allSplitedCards; 
        static vector<HoleCards> allHoleCards; 
        static vector<vector<Card>> allSelfOut;//robot played cards. 
};

//initialize static members
vector<vector<Card>> SplitCardsAndSelfOutTest::allSelfOut;
vector<HoleCards> SplitCardsAndSelfOutTest::allHoleCards;
vector<vector<vector<Card>>> SplitCardsAndSelfOutTest::allSplitedCards;
ReadSplitAndSelfOutCards SplitCardsAndSelfOutTest::readSplitAndSelfOutCards;

/*
 *@brief compare expected Split Cards and actual splited Cards
         if not equal output message.
 */
testing::AssertionResult compareSplitedCards(const char* expr_1, const char* expr_2, const char* expr_3, const char* expr_4,
        const vector<vector<Card>> &expSpliteCards, const vector<vector<Card>> &splitedCards, const vector<int> &splitedTypes, const HoleCards &hc)
{
    if (expSpliteCards == splitedCards && splitedCards.size() == splitedTypes.size())  //equal
        return testing::AssertionSuccess();

    //not equal ,message formatting.
    testing::Message msg; 
    const int formatWidth = 50;
    // msg << "expert: " << setw(formatWidth) << left << expr_1 << "split: " << expr_2 << " type:" << expr_3 << endl;
    msg << hc << endl;

    const unsigned max_i = (expSpliteCards.size() > splitedCards.size()) ? 
        expSpliteCards.size() : splitedCards.size();
    for (unsigned i = 0; i < max_i; ++i)
    {
        stringstream expstr;
        if (i < expSpliteCards.size())
            expstr << expSpliteCards[i]; 
        else
            expstr << "[ No expected Cards here ]";

        msg << "--" << setw(formatWidth+6) << left << expstr.str();

        msg << "**"; 
        if ( i < splitedCards.size() )
            msg << splitedCards[i] << "{" << card_type_str[splitedTypes[i]] << "}";
        else
            msg << "[ No splited cards here ]";

        msg << endl;
    }
    return testing::AssertionFailure(msg);
}

/*
 * split cards test
 */
TEST_F(SplitCardsAndSelfOutTest, HandlerSplitCards)
{
    for(unsigned i = 0; i < allHoleCards.size() ; ++i)
    {
        // out << "HoleCards["<< i << "]: ";
        // out << allHoleCards[i] << endl;
        EXPECT_PRED_FORMAT4(compareSplitedCards, allSplitedCards[i], allHoleCards[i].splitedCards, allHoleCards[i].splitedTypes, allHoleCards[i]);
    }
}
/*
 *@brief compare expected  robot play Cards and actual robot play Cards
         if not equal output message.
*/
testing::AssertionResult compareRobotPlayCards(const char* expr_1, const char* expr_2, const char* expr_3,
        const vector<Card> &outCards, const vector<Card> &selfOut, const HoleCards &hc)
{
    if (selfOut == outCards)  //equal
        return testing::AssertionSuccess();

    //not equal ,message formatting.
    testing::Message msg; 
    msg << hc << endl;
    const int formatWidth = 20;

    msg << "selfOut: " << selfOut;
    msg << setw(formatWidth -1) << right << "out: " << outCards << endl;
    return testing::AssertionFailure(msg);
}

/*
 * play cards test
 */
TEST_F(SplitCardsAndSelfOutTest, HandlerSelfOutCards)
{
    for(unsigned i = 0; i < allHoleCards.size() ; ++i)
    {
        // out << "HoleCards["<< i << "]: ";
        // out << allHoleCards[i] << endl;
        vector<Card> outCards;
        allHoleCards[i].robot(outCards);
        EXPECT_PRED_FORMAT3(compareRobotPlayCards, outCards, allSelfOut[i], allHoleCards[i]);
    }
}