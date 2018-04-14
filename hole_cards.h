#ifndef _HOLE_CARDS_H_
#define _HOLE_CARDS_H_

#include <map>
#include <vector>
#include <algorithm>

#include "card.h"
#include "card_statistics.h"

using namespace std;

struct SeatInfo { //收集某个坐位号的玩家信息
    int seatid;   //玩家的坐位号
    int cards_left_num; //剩余牌数
    //如果计算需要，还可以添加出了什么牌等其它信息
};

/*
struct leftCard { //除去机器人手里的牌和其它玩家打的牌，还剩余的牌，暂时只计10以前的牌值
    int leftJCnt;
    int leftQCnt;
    int leftKCnt;
    int leftACnt;
    int left2Cnt;
    int leftSmallKingCnt;
    int leftBigKingCnt;
};*/

class HoleCards
{
public:
	HoleCards();
	
	void add_card(Card c);
	void clear() { cards.clear(); };
	void copy_cards(std::vector<Card> *v);
	void copy_cards(std::vector<int> *v);
	int get_one_little_card(bool includeGhostFace = false);
    int get_one_max_card(bool includeGhostFace = false);
	void remove(std::vector<Card> &v);
	void remove(std::vector<int> &v);
	int size();
	void debug();

    int ghost_face; //鬼牌面值
	std::map<int, Card> cards;

//机器人相关的函数
    void split_cards();
	void robot(std::vector<Card> &v);
    void get_play_cards(const vector<int> &last_cards, vector<int> &out_cards, int ctype = 0, int cface = 0);

    //设置玩家信息的方法
    void set_left_num_of(int seatid, int left_num);
    void set_other_player_info(vector<int> &seatVec, vector<int> &cardLeft);

    //拆牌
    vector<vector<Card>> splitedCards;
    vector<int> splitedTypes;

    //玩家信息
    int landlord_seat;
    int table_type;
    int seatid;
    int pre_play_seat;

  private:

    //机器人相关的代码

    //拆牌相关函数
    void split_rocket(vector<Card> &splits);
    void split_bomb(vector<Card> &splits);
    void split_threeline(vector<Card> &splits);
    void split_three(vector<Card> &splits);
    void split_oneline(vector<Card> &splits);
    void split_twoline(vector<Card> &splits);
    void split_two(vector<Card> &splits);
    void split_one(vector<Card> &splits);

    int calc_max_line_cnt_of(const CardStatistics &my_card_stat);
    bool find_max_len_threeline(vector<Card> &splits);
    bool find_max_len_twoline(vector<Card> &splits);
    bool find_max_len_oneline(vector<Card> &splits);
    bool find_splite_three(vector<Card> &splits);

    void delete_cards_from_splits(vector<Card> &splits, const vector<Card> &anaCards, int splitedtype);

    //robot函数中调用的工具函数
    int check_three_with_other();
    int count_threeline_three();
    int count_type_of(int type);
    int get_bomb_size();
    int get_smallest_type_of(int type);
    int calc_with_cards(vector<Card> &v);


    //get_play_cards的工具方法
    int find_splite_bigger_of(int ctype, int cface);
    int play_bomb(vector<Card>& outCards, int type = 0, int face = 0);

    void split_cards_for_oneType(vector<Card> &curCards, int cface, vector<Card> &outCards);
    void find_splited_bigger_then_one(int cface, int ctype, vector<Card> &outCards);
    void find_splited_oneLine_bigger_then_one(int cface, int lineNum, vector<Card> &outCards);

    void split_cards_for_twoType(int cface, vector<Card> &outCards);
    void find_splited_twoLine_bigger_then_two(int cface, int lineNum, vector<Card> &outCards);
    void find_splited_bigger_then_two(int cface, int ctype, vector<Card> &outCards);

    void split_cards_for_threeType(int cface, vector<Card> &outCards);
    void get_followd_with_cards(int ctype, vector<Card> &outCards, const vector<Card> &last_cards = vector<Card>());

    void split_cards_for_threeLineType(int cface, vector<Card> &outCards);

    void calc_planewithone(vector<Card> &v);
    void calc_planewithtwo(vector<Card> &v);
    void delete_extra_three_cards(vector<Card> &outCards ,const vector<Card>& lastCards);

    void split_cards_for_oneLineType(int cface, vector<Card> &outCards, int lineCnt);
    void find_splited_oneLine_bigger_then_oneLine(int cface, vector<Card> &outCards, int clineCnt, int type = 0);
    void find_splited_twoLine_bigger_then_oneLine(int cface, vector<Card> &outCards, int clineCnt, int type = 0);
    void find_splited_threeLine_bigger_then_oneLine(int cface, vector<Card> &outCards, int clineCnt, int type = 0);

    void split_cards_for_twoLineType(int cface, vector<Card> &outCards, int lineCnt);
    void find_splited_twoLine_bigger_then_twoLine(int cface, vector<Card> &outCards, int clineCnt, int type = 0);
    void find_splited_threeLine_bigger_then_twoLine(int cface, vector<Card> &outCards, int clineCnt, int type = 0);

    void special_condition_check(std::vector<Card> &v, int ret_type, bool isFollow);
    void get_dan_shuang(bool &has_bao_dan, bool &has_bao_shuang);
    vector<SeatInfo> other_player_info; //存在其它玩家的信息,用于出牌分析
};

#endif /* _HOLE_CARDS_H_ */
