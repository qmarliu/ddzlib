#include "card.h"

static char face_symbols[] = {
	'3', '4', '5', '6', '7', '8', '9',
	'T', 'J', 'Q', 'K', 'A', '2', 'S', 'B'
};

static char suit_symbols[] = {
	'd', 'c', 'h', 's'
};

Card::Card()
{
	face = suit = value = 0;
}

Card::Card(int val)
{
	value = val;
	
	face = value & 0xF;
	suit = value >> 4;
	if (face < 3)
		face += 13;
	else if (face > 13)
		face += 2;		// 16是小王  17是大王
	// printf("Face[%d] Suit[%d]\n", face, suit);
}

string Card::get_card()
{
	string card;
	
	/*
	char buf[32];
	snprintf(buf, sizeof(buf), "%d-%d", face, suit);
	card.append(buf);
	*/
	
	card.append(1, face_symbols[face - 3]);
	card.append(1, suit_symbols[suit]);
	
	return card;
}