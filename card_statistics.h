#ifndef _CARD_STATISTICS_H_
#define _CARD_STATISTICS_H_

#include <vector>
#include <string>

using namespace std;

class CardStatistics
{
public:
	vector<Card> card1;
	vector<Card> card2;
	vector<Card> card3;
	vector<Card> card4;
	
	vector<Card> line1;
	vector<Card> line2;
	vector<Card> line3;
	
	unsigned int len;

    CardStatistics() {}
    CardStatistics(const std::vector<Card> &stCards, int set_ghost_face = 0);

    void clear();
    int statistics(const std::vector<Card> &cards = std::vector<Card>(), int set_ghost_face = 0);
	
	void debug();

    int ghost_face; //保存鬼牌面值
    vector<Card> ghost_cards; //保存牌值中的鬼牌,鬼牌不会出现在line和card成员变量中

    bool has_ghost() const { return ghost_cards.size() != 0; }
};

#endif /* _CARD_STATISTICS_H_ */
