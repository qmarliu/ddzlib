#include "community_cards.h"

CommunityCards::CommunityCards()
{

}

void CommunityCards::add_card(Card card)
{
	cards.push_back(card);
}

void CommunityCards::copy_cards(std::vector<Card> *v)
{
	for (unsigned int i = 0; i < cards.size(); i++)
	{
		v->push_back(cards[i]);
	}
}

void CommunityCards::copy_to_hole_cards(HoleCards &holecards)
{
	for (unsigned int i = 0; i < cards.size(); i++)
	{
		holecards.add_card(cards[i]);
	}
}

void CommunityCards::debug()
{
	Card::dump_cards(cards);
}
