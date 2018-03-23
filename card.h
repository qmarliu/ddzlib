#ifndef _CARD_H_
#define _CARD_H_

#include <string>
#include <iostream>
#include <cstdio>
#include <vector>
#include <map>
#include <algorithm>

#include "card_type.h"

using namespace std;

/**
 * suit  0  方块  1 梅花   2  红桃    3黑桃
 * 	
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
0x0E,						//小王
0x0F						//大王
 * @author luochuanting
 */

class Card
{
public:
	typedef enum {
		Three = 3,
		Four,
		Five,
		Six,
		Seven,
		Eight,
		Nine,
		Ten,
		Jack,
		Queen,
		King,
		Ace,
		Two,
		Small, // small Joker
		Big, // big Joker
		
		FirstFace = Three,
		LastFace = Big
	} Face;
	
	typedef enum {
		Diamonds = 0,
		Clubs,
		Hearts,
		Spades,
		
		FirstSuit = Diamonds,
		LastSuit = Spades
	} Suit;
	
	int face;
	int suit;
	
	int value;
	
	Card();
	Card(int val);
	
	std::string get_card();
	
	bool operator <  (const Card &c) const{ return (face < c.face); };
	bool operator >  (const Card &c) const { return (face > c.face); };
	bool operator == (const Card &c) const { return (face == c.face); };
	
	static int compare(const Card &a, const Card &b)
	{
		if (a.face > b.face)
		{
			return 1;
		}
		else if (a.face < b.face)
		{
			return -1;
		}
		else if (a.face == b.face)
		{
			if (a.suit > b.suit)
			{
				return 1;
			}
			else if (a.suit < b.suit)
			{
				return -1;
			}	
		}
	
		return 0;
	}

	static bool lesser_callback(const Card &a, const Card &b)
	{
		if (Card::compare(a, b) == -1)
			return true;
		else
			return false;
	}
	
	static bool greater_callback(const Card &a, const Card &b)
	{
		if (Card::compare(a, b) == 1)
			return true;
		else
			return false;
	}
	
	static void sort_by_ascending(std::vector<Card> &v)
	{
		sort(v.begin(), v.end(), Card::lesser_callback);
	}
	
	static void sort_by_descending(std::vector<Card> &v)
	{
		sort(v.begin(), v.end(), Card::greater_callback);
	}
	
	static void dump_cards(std::vector<Card> &v, string str = "cards")
	{
		fprintf(stdout, "[%s]: [[ ", str.c_str());
		for (std::vector<Card>::iterator it = v.begin(); it != v.end(); it++)
			fprintf(stdout, "%s ", it->get_card().c_str());
	
		fprintf(stdout, "]]\n");
	}
	
	static void dump_cards(std::map<int, Card> &m, string str = "cards")
	{
		fprintf(stdout, "[%s]: [[ ", str.c_str());
		for (std::map<int, Card>::iterator it = m.begin(); it != m.end(); it++)
			fprintf(stdout, "%s ", it->second.get_card().c_str());
	
		fprintf(stdout, "]]\n");
	}
};

#endif /* _CARD_H_ */
