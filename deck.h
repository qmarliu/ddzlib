#ifndef _DECK_H_
#define _DECK_H_

#include <vector>
#include <time.h>

#include "card.h"
#include "hole_cards.h"
#include "community_cards.h"


using namespace std;

class Deck
{
public:
    Deck() {ghost_face  = 0;}
	void fill();
	void empty();
	int count() const;
	
	bool push(Card card);
	bool pop(Card &card);
	bool shuffle(int seed);
	
	void get_hole_cards(HoleCards &holecards);
	void get_community_cards(CommunityCards &communitycards);

    void set_ghost_face(int face);
    int get_ghost_face();
    bool has_ghost();
	
	void debug();
	
public:
	vector<Card> cards;

private:

    void  choose_ghost_face();
    int         ghost_face; //鬼牌面值
};

#endif /* _DECK_H_ */
