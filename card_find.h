#ifndef _CARD_FIND_H_
#define _CARD_FIND_H_

#include <vector>
#include <string>

#include "card.h"
#include "card_type.h"
#include "card_analysis.h"
#include "card_statistics.h"


using namespace std;

class CardFind
{
public:
	vector<vector<Card> > results;
	vector<int> results_type; //查找的牌的类型

    int ghost_face;//鬼牌面值

    int compare_type; //要找的牌型，或能大过这个牌型的牌型
    int compare_face; //要找牌型的值能取的最小值-1
    CardStatistics my_card_stat;
    CardStatistics card_stat;
	
    CardFind(int g_face = 0, bool robotFlag = false);
    CardFind(const vector<Card> &compare_cards, const vector<Card> &my_cards, int ctype = 0, int cface = 0, int gface = 0, bool robotFlag = false);

    void clear();
    void init(const vector<Card> &compare_cards, const vector<Card> &my_cards, int ctype = 0, int cface = 0, int gface = 0);
    int find(const vector<Card> &compare_cards, const vector<Card> &my_cards, int ctype = 0, int cface = 0, int gface = 0);

    void debug();
    int tip(const vector<int> &last, const vector<int> &cur, int ctype = 0, int cface = 0, int gface = 0);
    int tip(const vector<Card> &cards0, const vector<Card> &cards1, int ctype = 0, int cface = 0, int gface = 0);
    void set_ghost_face(const int g_face = 0);

    //以前的，无癞子的查找和测试代码,现在已经没有用到了。
    int find(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
	
	void find_one(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
	void find_two(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
	void find_three(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
	void find_one_ine(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
	void find_two_ine(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
	void find_three_line(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
	void find_three_with_one(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
	void find_three_with_two(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
	void find_plane_with_one(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
	void find_plane_with_wing(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
	void find_four_with_one(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
	void find_four_with_two(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
	void find_bomb(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
	void find_rocket(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat);
    // int tip(vector<int> &last, vector<int> &cur);
	static int find_straight(vector<int> &input, vector<int> &output);
	static int get_straight(CardStatistics &card_stat, vector<Card> &output);
	static int get_max(unsigned int a, unsigned int b, unsigned int c);
	static void get_longest_straight(vector<Card> &input, int type, vector<Card> &output);
	static void test(int input0[], int len0, int input1[], int len1);
	static void test(int input[], int len);
    //end 以前的代码 

    //机器人相关变量，在机器人打牌时用得上
    bool robot_flag; //如果是机器人调用，要记录一些额外信息,如果不是，则下面的变量将会没有赋值
    vector<int> from_two; //如果查找单牌，记得查到单牌是从一对拆出来的results中的下标
    vector<int> from_three;//如果查找单或一对，记得查到单牌或一对是从三张拆出来的results中的下标
    // vector<int> from_bomb; //记录是否是炸弹
    void set_robot_flag(bool robotFlag);
  private:
    inline bool has_ghost() const { return ghost_face != 0; } 

    void find_type_bigger_then_compare_type(bool (CardFind::*)(const vector<Card> &v), int ghost_max = 4);

    void find_all_bomb_bigger_then_other_type();
    void find_softbomb_bigger_then_other_type();
    void find_bomb_bigger_then_other_type();
    void find_ghostbomb_bigger_then_other_type();
    void find_rocket_bigger_then_other_type();


    bool find_one(const vector<Card> &diceCards = vector<Card>());
    bool find_two(const vector<Card> &diceCards = vector<Card>());
    bool find_three(const vector<Card> &diceCards = vector<Card>());
    bool find_oneline(const vector<Card> &diceCards = vector<Card>());
    bool find_twoline(const vector<Card> &diceCards = vector<Card>());
    bool find_threeline(const vector<Card> &diceCards = vector<Card>());
    bool find_planewithone(const vector<Card> &diceCards = vector<Card>());
    bool find_planewithtwo(const vector<Card> &diceCards = vector<Card>());
    bool find_fourwithone(const vector<Card> &diceCards = vector<Card>());
    bool find_fourwithtwo(const vector<Card> &diceCards = vector<Card>());

    bool find_anaCards_type_with_ghost(int diff, const vector<Card> &cards, const vector<Card> &diceCards = vector<Card>(), unsigned fixCardSize = 0);
    void copy_card1_without_king(const vector<Card> &card1, vector<Card> &cards);

    void robot_data_analysis(const vector<Card> &ana_cards);
};


#endif /* _CARD_FIND_H_ */
