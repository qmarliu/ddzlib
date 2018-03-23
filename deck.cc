#include <algorithm>
#include "deck.h"

static int card_arr[] = {
0x01, 0x11, 0x21, 0x31,		//A 14 
0x02, 0x12, 0x22, 0x32,		//2 15
0x03, 0x13, 0x23, 0x33,		//3 3
0x04, 0x14, 0x24, 0x34,		//4 4
0x05, 0x15, 0x25, 0x35,		//5 5
0x06, 0x16, 0x26, 0x36,		//6 6
0x07, 0x17, 0x27, 0x37,		//7 7
0x08, 0x18, 0x28, 0x38,		//8 8
0x09, 0x19, 0x29, 0x39,		//9 9
0x0A, 0x1A, 0x2A, 0x3A,		//10 10
0x0B, 0x1B, 0x2B, 0x3B,		//J 11
0x0C, 0x1C, 0x2C, 0x3C,		//Q 12
0x0D, 0x1D, 0x2D, 0x3D,		//K 13
0x0E, 0x0F};

void Deck::fill()
{
	cards.clear();
	
	for (int i = 0; i < 54; i++)
	{
        Card c(card_arr[i]);
        push(c);
    }
}

void Deck::empty()
{
	cards.clear();
}

int Deck::count() const
{
	return cards.size();
}

bool Deck::push(Card card)
{
	cards.push_back(card);
	return true;
}

bool Deck::pop(Card &card)
{
	if (!count())
		return false;
	
	card = cards.back();
	cards.pop_back();
	return true;
}

bool Deck::shuffle(int seed)
{
    srand(time(NULL) + seed);
	random_shuffle(cards.begin(), cards.end());
	return true;
}

void Deck::get_hole_cards(HoleCards &holecards)
{
	Card card;
	
	holecards.clear();
	for (int i = 0; i < 17; i++)
	{
		pop(card);
		holecards.add_card(card);
	}
}

void Deck::get_community_cards(CommunityCards &communitycards)
{
	Card card;
	communitycards.clear();
	for (int i = 0; i < 3; i++)
	{
		pop(card);
		communitycards.add_card(card);
	}
}

void Deck::debug()
{
	Card::dump_cards(cards);
    //输出鬼牌
    printf("ghost_face: %d\n", ghost_face);
}

/*
 @brief 指定face值的牌为鬼牌
*/
void Deck::set_ghost_face(int face)
{
    if (face >= 0 && face <= 0x0D)
        ghost_face = face;
}

/*
 @brief 获取鬼牌，如果鬼牌没有确定，调用这个函数也会选取鬼牌并确定鬼牌
*/
int Deck::get_ghost_face()
{
    if (has_ghost()) //已经设置鬼牌，不用选取
        return ghost_face;

    //选取鬼牌
    choose_ghost_face();
    return ghost_face;
}

/*
 @brief 在cards中（除大小王）选则一个牌当鬼牌
*/
void Deck::choose_ghost_face()
{
    if (cards.size() == 0)
        return ;
    srand(time(NULL));
    while(true)
    {
        int index = rand() % cards.size();
        if (cards[index].face != Card::Small && cards[index].face != Card::Big)
        { //大小王，不当鬼牌
            ghost_face = cards[index].face;    
            return;
        }
    }
}

/*
 @brief 判断是否有鬼牌设置
*/
bool Deck::has_ghost()
{
    return ghost_face != 0 ;
}
