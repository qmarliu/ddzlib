#ifndef _HOLE_CARDS_H_
#define _HOLE_CARDS_H_

#include <map>
#include <vector>
#include <algorithm>

#include "card.h"

using namespace std;

struct SeatInfo {
    int seatid;
    int cards_left_num;
};

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
	
	int robot(std::vector<int> &v, int landlord_seat, int lord_type, int my_seat);
	
	void remove(std::vector<Card> &v);
	
	void remove(std::vector<int> &v);
	
	int size();
	
	void debug();

	std::map<int, Card> cards;

    vector<SeatInfo> other_player_info; //存在其它玩家的信息,用于出牌分析
    void set_left_num_of(int seatid, int left_num);
    void get_play_cards(vector<int> &last_cards, vector<int> &out_cards, int landlord_seat, int lord_type, int my_seat, int pre_play_seat);
    void set_other_player_info(vector<int> &seatVec, vector<int> &cardLeft);

    int ghost_face; //鬼牌面值
  private:
    void get_dan_shuang(bool &has_bao_dan, bool &has_bao_shuang, int landlord_seat, int lord_type, int my_seat);
};

#endif /* _HOLE_CARDS_H_ */
