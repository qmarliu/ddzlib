#ifndef _CARD_ANALYSIS_H_
#define _CARD_ANALYSIS_H_

#include <vector>
#include <string>

#include "card_statistics.h"

using namespace std;

class CardAnalysis
{
public:
	unsigned int len;
	int type;
	int face;

    vector<int> back_up_type; //有鬼牌的时候，某个特定的牌型可能组成不同的type。
    vector<int> back_up_face;

    int ghost_face;//鬼牌面值
    int ghost_num;//鬼牌数量
    CardStatistics card_stat;

    CardAnalysis();
    CardAnalysis(const std::vector<Card> &anaCards, int set_ghost_face = 0);
    CardAnalysis(const CardStatistics &anacard_stat);
	
	void clear();
	int analysis(const CardStatistics &anacard_stat);
    int analysis(const std::vector<Card> &anaCards, int set_ghost_face);
    int get_card_face_of_type(int expect_type);

	bool operator <  (const CardAnalysis &card_ana) const;

    static bool isGreater(const std::vector<Card> &anaCards, int ghost_face, int ctype, int cface);
	static bool check_type_face(const vector<int> &ana_card, int ctype, int cface, int gface);
	static bool check_type_face(const vector<Card> &ana_card, int ctype, int cface, int gface);

    //分析无癞子时的用的函数
	bool check_is_line(const CardStatistics &card_stat, int line_type);
	bool check_arr_is_line(const std::vector<Card> &line, int line_type);
	bool check_arr_is_line(const std::vector<Card> &line, int line_type, unsigned int begin, unsigned int end);
	
	bool compare(CardAnalysis &card_analysis);
    void debug();
    
    static void format(const CardStatistics &stat, vector<int> &cur);
	static void format(const CardStatistics &stat, vector<Card> &cur);
	static int isGreater(vector<int> &last, vector<int> &cur, int *card_type);
	static int isGreater(vector<Card> &last, vector<Card> &cur, int *card_type);
    static int get_card_type(vector<int> &input);
	static int get_card_type(vector<Card> &input);
    static void test(int input[], int len);
	static void test(int input0[], int len0, int input1[], int len1);
    //end 无癞子函数

private:
    int do_analysis();
    int ghost_analysis();
    int ghost_check_two();
    int ghost_check_three();
    int ghost_check_threewithone();
    int ghost_check_threewithtwo();
    int ghost_check_line();
    int ghost_check_doubleline();
    int ghost_check_tripleline();
    int ghost_check_bomb();
    int ghost_check_fourwithone();
    int ghost_check_fourwithtwo();
    int ghost_check_plane();
    bool ghost_has_plane(vector<int> &triple_card_face, int three_need, int &left_num) const;
    bool ghost_has_pane_with_two(vector<int> &triple_card_face, int &left_num) const;

    void ghost_set_face_and_type(int set_type,int set_face);

    bool has_bigger_than_ace() const;
    int ghost_calc_gap() const;
};


#endif /* _CARD_STATISTICS_H_ */
