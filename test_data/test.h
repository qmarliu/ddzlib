#pragma once

#include <gtest/gtest.h>
#include <vector>
#include <fstream>
#include <sstream>
#include "../hole_cards.h"
#include <nlohmann/json.hpp>
// #include "test_data/json.hpp"

ostream &out = cout; 
// ofstream out("test_data/split_text.txt");

static char *card_type_str[] = 
{
	(char*)"CARD_TYPE_ERROR", 
	(char*)"CARD_TYPE_ONE",
	(char*)"CARD_TYPE_ONELINE",
	(char*)"CARD_TYPE_TWO",
	(char*)"CARD_TYPE_TWOLINE",
	(char*)"CARD_TYPE_THREE",
	(char*)"CARD_TYPE_THREELINE",
	(char*)"CARD_TYPE_THREEWITHONE",
	(char*)"CARD_TYPE_THREEWITHTWO",
	(char*)"CARD_TYPE_PLANEWITHONE",
	(char*)"CARD_TYPE_PLANEWITHWING",
	(char*)"CARD_TYPE_FOURWITHONE",
	(char*)"CARD_TYPE_FOURWITHTWO",
	(char*)"CARD_TYPE_SOFTBOMB",
	(char*)"CARD_TYPE_BOMB",
	(char*)"CARD_TYPE_GHOSTBOMB",
	(char*)"CARD_TYPE_ROCKET"
};

//convenience for output vector<Card>
ostream &operator<< (ostream &output, const vector<Card> &cards)
{
    output << "[ ";
    for (const auto &card : cards)
    {
        output << card.value << " ";
    }
    output << "]";
    return output;
}

ostream &operator<< (ostream &output, const HoleCards &hc)
{
    output << "[ ";
    for (const auto &card : hc.cards)
    {
        output << card.second.value << " ";
    }
    output << "]";
    return output;
}

ostream &operator<< (ostream &output, const vector<int> &cards)
{
    output << "[ ";
    for (const auto &card : cards)
    {
        output << card << " ";
    }
    output << "]";
    return output;
}