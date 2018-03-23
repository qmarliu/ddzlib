#include "card.h"
#include "card_statistics.h"


CardStatistics::CardStatistics(const std::vector<Card> &stCards, int set_ghost_face)
{
	statistics(stCards, set_ghost_face);
}

void CardStatistics::clear()
{
	card1.clear();
	card2.clear();
	card3.clear();
	card4.clear();
	
	line1.clear();
	line2.clear();
	line3.clear();

    ghost_face = 0;
    ghost_cards.clear();
}

int CardStatistics::statistics(const std::vector<Card> &stCards, int set_ghost_face)
{
    clear();

    std::vector<Card> cards = stCards;
    len = cards.size();
    if (len == 0)
        return 0;
    ghost_face = set_ghost_face;

    Card::sort_by_ascending(cards);
    // Card::sort_by_descending(cards);
    int j = 0;
    unsigned int i = 0;
    Card temp = cards[0];
    for (i = 1; i < cards.size(); i++)
    {
        if (temp.face == ghost_face)
        {
            ghost_cards.push_back(cards[i - 1]);
            j = 0;
            temp = cards[i];
            continue;
        }
        else if (temp == cards[i])
        {
            j++;
        }
        else
        {
            if (j == 0)
            {
                card1.push_back(cards[i - 1]);
                line1.push_back(cards[i - 1]);
            }
            else if (j == 1)
            {
                card2.push_back(cards[i - 2]);
                card2.push_back(cards[i - 1]);
                line1.push_back(cards[i - 2]);
                line2.push_back(cards[i - 2]);
                line2.push_back(cards[i - 1]);
            }
            else if (j == 2)
            {
                card3.push_back(cards[i - 3]);
                card3.push_back(cards[i - 2]);
                card3.push_back(cards[i - 1]);
                line1.push_back(cards[i - 3]);
                line2.push_back(cards[i - 3]);
                line2.push_back(cards[i - 2]);
                line3.push_back(cards[i - 3]);
                line3.push_back(cards[i - 2]);
                line3.push_back(cards[i - 1]);
            }
            else if (j == 3)
            {
                card4.push_back(cards[i - 4]);
                card4.push_back(cards[i - 3]);
                card4.push_back(cards[i - 2]);
                card4.push_back(cards[i - 1]);
                line1.push_back(cards[i - 4]);
                line2.push_back(cards[i - 4]);
                line2.push_back(cards[i - 3]);
                line3.push_back(cards[i - 4]);
                line3.push_back(cards[i - 3]);
                line3.push_back(cards[i - 2]);
            }

            j = 0;
        }
        temp = cards[i];
    }

    if (temp.face != ghost_face)
    {
        if (j == 0)
        {
            card1.push_back(cards[i - 1]);
            line1.push_back(cards[i - 1]);
        }
        else if (j == 1)
        {
            card2.push_back(cards[i - 2]);
            card2.push_back(cards[i - 1]);
            line1.push_back(cards[i - 2]);
            line2.push_back(cards[i - 2]);
            line2.push_back(cards[i - 1]);
        }
        else if (j == 2)
        {
            card3.push_back(cards[i - 3]);
            card3.push_back(cards[i - 2]);
            card3.push_back(cards[i - 1]);
            line1.push_back(cards[i - 3]);
            line2.push_back(cards[i - 3]);
            line2.push_back(cards[i - 2]);
            line3.push_back(cards[i - 3]);
            line3.push_back(cards[i - 2]);
            line3.push_back(cards[i - 1]);
        }
        else if (j == 3)
        {
            card4.push_back(cards[i - 4]);
            card4.push_back(cards[i - 3]);
            card4.push_back(cards[i - 2]);
            card4.push_back(cards[i - 1]);
            line1.push_back(cards[i - 4]);
            line2.push_back(cards[i - 4]);
            line2.push_back(cards[i - 3]);
            line3.push_back(cards[i - 4]);
            line3.push_back(cards[i - 3]);
            line3.push_back(cards[i - 2]);
        }
    }
    else
    {
        ghost_cards.push_back(cards[i - 1]);
    }

    // debug();
    return 0;
}

void CardStatistics::debug()
{
	Card::dump_cards(card1, "card1");
	Card::dump_cards(card2, "card2");
	Card::dump_cards(card3, "card3");
	Card::dump_cards(card4, "card4");
	
	Card::dump_cards(line1, "line1");
	Card::dump_cards(line2, "line2");
	Card::dump_cards(line3, "line3");
	Card::dump_cards(ghost_cards, "ghost_cards");
    printf("[ghost_face]: [[0x%x]]\n", ghost_face);
}
