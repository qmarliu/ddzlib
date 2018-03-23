#ifndef _COMMUNITY_CARDS_H_
#define _COMMUNITY_CARDS_H_

#include <vector>
#include <algorithm>

#include "card.h"
#include "hole_cards.h"

using namespace std;

class CommunityCards
{
public:
	CommunityCards();
	
	void add_card(Card c);
	void clear() { cards.clear(); };
	
	void copy_cards(std::vector<Card> *v);
	void copy_to_hole_cards(HoleCards &holecards);
	void debug();

	std::vector<Card> cards;
};

#endif /* _COMMUNITY_CARDS_H_ */
