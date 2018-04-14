#include "card.h"
#include "card_analysis.h"
#include "card_statistics.h"
#include "card_find.h"


CardFind::CardFind(int g_face, bool robotFlag)
{
    compare_type = 0;
    compare_face = 0;
    robot_flag = robotFlag;
    set_ghost_face(g_face);
}

CardFind::CardFind(const vector<Card> &compare_cards, const vector<Card> &my_cards, int ctype, int cface, int gface, bool robotFlag)
{
    robot_flag = robotFlag;
    init(compare_cards, my_cards, ctype, cface, gface);
}

/*
 @breif 初始代成员变量
 @param compare_cards[in] 去比较的牌
 @param my_cards[in] 可用来查找的牌
 @param ctype[in] compare_cards中可能有多个牌型，指出要找的牌型
 @param cface[in] 要找出牌型的最大face值

 添加这个函数初始化成员变量，是让find_bigger_then系列函数可以单独调用，
 而不一定要在find函数里面才能调用。
*/
void CardFind::init(const vector<Card> &compare_cards, const vector<Card> &my_cards, int ctype, int cface, int gface)
{
    clear();

    compare_type = ctype;
    compare_face = cface;
    ghost_face = gface;

    card_stat.statistics(compare_cards, ghost_face);
    my_card_stat.statistics(my_cards, ghost_face);
    if (compare_type <= 0 || compare_face <= 0)
    { //没有指定要比的牌型和值， 可自己通过compare_cards算出来一个类型和牌值
        CardAnalysis card_ana(card_stat);
        compare_type = card_ana.type;
        compare_face = card_ana.face;
    }
}

/*
 @breif 设置鬼牌的牌值
 @param g_face[in] 鬼牌牌值
*/
void CardFind::set_ghost_face(const int g_face)
{
    if (g_face >= Card::Three && g_face <= Card::Two) ghost_face = g_face;
    else ghost_face = 0;
}

void CardFind::clear()
{
	results.clear();
    from_two.clear();
    from_three.clear();
    compare_type = 0;
    compare_face = 0;
}

/*
 @brief 在cur中查找比last中牌型大的牌，并将查找的结果放在results中。
 @param compare_cards[in] 去比较的牌
 @param my_cards[in] 可用来查找的牌
 @param ctype[in] compare_cards中可能有多个牌型，指出要找的牌型
 @param cface[in] 要找出牌型的最大face值
 @retur -1表示cards0没有牌， -2表示card2中没有牌， 0表示没找到  1表示找到了
*/
int CardFind::tip(const vector<int> &compare_cards, const vector<int> &my_cards, int ctype, int cface, int gface)
{
    if (compare_cards.size() == 0) return -1;
    if (my_cards.size() == 0) return -2;
    
	vector<Card> cards0;
	for (unsigned int i = 0; i < compare_cards.size(); i++)
	{
		Card card(compare_cards[i]);
		cards0.push_back(card);
	}

    vector<Card> cards1;
	for (unsigned int i = 0; i < my_cards.size(); i++)
	{
		Card card(my_cards[i]);
		cards1.push_back(card);
	}
    return find(cards0, cards1, ctype, cface, gface);
}

/*
 @brief 在my_cards中查找比compare_cards中牌型大的牌，并将查找的结果放在results中。
 @param compare_cards[in] 上家玩家打出去的脾 去比较的牌
 @param my_cards[in] 我自己的牌 可用来查找的牌
 @param ctype[in] compare_cards中可能有多个牌型，指出要找的牌型
 @param cface[in] 要找出牌型的最大face值
 @param gface[in] 鬼牌的面值
 @retur -1表示cards0没有牌， -2表示card2中没有牌， 0表示没找到  1表示找到了
*/
int CardFind::tip(const vector<Card> &compare_cards, const vector<Card> &my_cards, int ctype, int cface, int gface)
{
    if (compare_cards.size() == 0) return -1;
    if (my_cards.size() == 0) return -2;
	
    return find(compare_cards, my_cards, ctype, cface, gface);
}

/*
 @brief 在my_cards中查找比compare_cards中牌型大的牌，并将查找的结果放在results中。
 @param compare_cards[in] 去比较的牌
 @param my_cards[in] 可用来查找的牌
 @param ctype[in] compare_cards中可能有多个牌型，指出要找的牌型
 @param cface[in] 要找出牌型的最大face值
 @retur -1表示cards0没有牌， -2表示card2中没有牌， 0表示其它情况
*/
int CardFind::find(const vector<Card> &compare_cards, const vector<Card> &my_cards, int ctype, int cface, int gface)
{
    if (compare_cards.size() == 0) return -1;
    if (my_cards.size() == 0) return -2;

    init(compare_cards, my_cards, ctype, cface, gface);

    switch (compare_type)
    {
    case CARD_TYPE_ERROR:
        return -1;
    case CARD_TYPE_ROCKET: //没有比火箭还大的牌型
        return 0;
    case CARD_TYPE_ONE:
        find_type_bigger_then_compare_type(&CardFind::find_one, 1);
        break; 
    case CARD_TYPE_ONELINE:
        find_type_bigger_then_compare_type(&CardFind::find_oneline);
        break;
    case CARD_TYPE_TWO:
        find_type_bigger_then_compare_type(&CardFind::find_two, 2);
        break;
    case CARD_TYPE_TWOLINE:
        find_type_bigger_then_compare_type(&CardFind::find_twoline, 3);
        break;
    case CARD_TYPE_THREE:
        find_type_bigger_then_compare_type(&CardFind::find_three);
        break;
    case CARD_TYPE_THREELINE:
        find_type_bigger_then_compare_type(&CardFind::find_threeline);
        break;
    case CARD_TYPE_THREEWITHONE:
    case CARD_TYPE_PLANEWITHONE:
        find_type_bigger_then_compare_type(&CardFind::find_planewithone);
        break;
    case CARD_TYPE_THREEWITHTWO:
    case CARD_TYPE_PLANEWITHWING:
        find_type_bigger_then_compare_type(&CardFind::find_planewithtwo);
        break;
    case CARD_TYPE_FOURWITHONE:
        find_type_bigger_then_compare_type(&CardFind::find_fourwithone);
        break;
    case CARD_TYPE_FOURWITHTWO:
        find_type_bigger_then_compare_type(&CardFind::find_fourwithtwo);
        break;
    default: //炸弹情况，最后会统一处理
        break;
    }
    find_all_bomb_bigger_then_other_type();
    find_rocket_bigger_then_other_type();
    return 0;
}

//************************下面是工具函数*****************************
/*
 @brief 在成员变量my_card_stat找出比card_stat中更大的牌(除了炸弹和火箭外)
 @param find_func[in] 函数指针，根据函数不同，从而比较不同牌型。
 @param ghost_max 比较单牌的时候，是1. 对子的是2，三张是3，其它情况是4
*/
void CardFind::find_type_bigger_then_compare_type(bool (CardFind::*find_func)(const vector<Card> &), int ghost_max)
{
    vector<Card> diceCards;
    if ((this->*find_func)(diceCards)) 
    {
        // return;
    }
    //加1张鬼牌
    if (ghost_max < 1) return;
    if (my_card_stat.ghost_cards.size() > 0)
    { 
        diceCards.push_back(my_card_stat.ghost_cards[0]);
        if ((this->*find_func)(diceCards)) 
        { 
            // return;
        }
    }
    //加2张鬼牌
    if (ghost_max < 2) return;
    if (my_card_stat.ghost_cards.size() > 1)
    { 
        diceCards.push_back(my_card_stat.ghost_cards[1]);
        if ((this->*find_func)(diceCards)) 
        { 
            // return;
        }
    }
    //加3张鬼牌
    if (ghost_max < 3) return;
    if (my_card_stat.ghost_cards.size() > 2)
    { 
        diceCards.push_back(my_card_stat.ghost_cards[2]);
        if ((this->*find_func)(diceCards))
        { 
            // return;
        }
    }
    // 加4张鬼牌
    if (ghost_max < 4) return;
    if (my_card_stat.ghost_cards.size() > 3)
    { 
        diceCards.push_back(my_card_stat.ghost_cards[3]);
        if ((this->*find_func)(diceCards))
        {
            // return;
        }
    }
}

/*
 @brief 在成员变量my_card_stat中,找出比compare_face大的所有炸弹 
*/
void CardFind::find_all_bomb_bigger_then_other_type()
{
    if (compare_type > CARD_TYPE_GHOSTBOMB) return;

    find_softbomb_bigger_then_other_type();
    find_bomb_bigger_then_other_type();
    find_ghostbomb_bigger_then_other_type();
}

/*
 @brief 在成员变量my_card_stat中,找出比compare_face大的软炸弹 
*/
void CardFind::find_softbomb_bigger_then_other_type()
{
    if (compare_type > CARD_TYPE_SOFTBOMB) return;

    const int ghost_cnt = my_card_stat.ghost_cards.size();
    if (ghost_cnt == 0) return; //没有鬼牌，就没有软炸弹

    if (ghost_cnt >= 1)
    {
        for (unsigned int i = 0; i < my_card_stat.card3.size(); i += 3)
        {
            if ((compare_type == CARD_TYPE_SOFTBOMB) && (my_card_stat.card3[i].face <= compare_face))
                continue;

            vector<Card> cards;
            cards.push_back(my_card_stat.card3[i]); 
            cards.push_back(my_card_stat.card3[i + 1]);
            cards.push_back(my_card_stat.card3[i + 2]);
            cards.push_back(my_card_stat.ghost_cards[0]);
            results.push_back(cards);
        }
    }

    if (ghost_cnt >= 2)
    {
        for (unsigned int i = 0; i < my_card_stat.card2.size(); i += 2)
        {
            if ((compare_type == CARD_TYPE_SOFTBOMB) && (my_card_stat.card2[i].face <= compare_face))
                continue;

            vector<Card> cards;
            cards.push_back(my_card_stat.card2[i]);
            cards.push_back(my_card_stat.card2[i + 1]);
            cards.push_back(my_card_stat.ghost_cards[0]);
            cards.push_back(my_card_stat.ghost_cards[1]);
            results.push_back(cards);
        }
    }

    if (ghost_cnt >= 3)
    {
        for (unsigned int i = 0; i < my_card_stat.card1.size(); i += 1)
        {
            if ((compare_type == CARD_TYPE_SOFTBOMB) && (my_card_stat.card1[i].face <= compare_face))
                continue;

            if ((my_card_stat.card1[i].face == 16) || (my_card_stat.card1[i].face == 17)) //防止癞子充当鬼牌
                continue;

            vector<Card> cards;
            cards.push_back(my_card_stat.card1[i]);
            cards.push_back(my_card_stat.ghost_cards[0]);
            cards.push_back(my_card_stat.ghost_cards[1]);
            cards.push_back(my_card_stat.ghost_cards[2]);
            results.push_back(cards);
        }
    }
}

/*
 @brief 在成员变量my_card_stat的四张中,找出比compare_face大的牌
*/
void CardFind::find_bomb_bigger_then_other_type()
{
    if (compare_type > CARD_TYPE_BOMB) return;

    for (unsigned int i = 0; i < my_card_stat.card4.size(); i += 4)
    {
        if ((compare_type == CARD_TYPE_BOMB) && (my_card_stat.card4[i] < compare_face))
            continue;
        vector<Card> cards;
        cards.push_back(my_card_stat.card4[i]);
        cards.push_back(my_card_stat.card4[i + 1]);
        cards.push_back(my_card_stat.card4[i + 2]);
        cards.push_back(my_card_stat.card4[i + 3]);
        results.push_back(cards);
    }
}

/*
 @brief 在成员变量my_card_stat中,找出比compare_face大的癞子炸弹
*/
void CardFind::find_ghostbomb_bigger_then_other_type()
{
    if (compare_type > CARD_TYPE_GHOSTBOMB) return;

    if (my_card_stat.ghost_cards.size() < 4) return;

    vector<Card> cards;
    cards.push_back(my_card_stat.ghost_cards[0]);
    cards.push_back(my_card_stat.ghost_cards[1]);
    cards.push_back(my_card_stat.ghost_cards[2]);
    cards.push_back(my_card_stat.ghost_cards[3]);
    results.push_back(cards);
}

/*
 @brief 在成员变量my_card_stat中,找出火箭
*/
void CardFind::find_rocket_bigger_then_other_type()
{
    int len = my_card_stat.card1.size();
	if (len < 2) return;

    if (my_card_stat.card1[len - 2].face == 16 &&
        my_card_stat.card1[len - 1].face == 17)
    {
        vector<Card> cards;
        cards.push_back(my_card_stat.card1[len - 2]);
        cards.push_back(my_card_stat.card1[len - 1]);
        results.push_back(cards);
    }
}
/*
 @brief 在my_card_stat中找到比card_stat中更大的单牌
*/
bool CardFind::find_one(const vector<Card> &diceCards)
{
    if (card_stat.len != 1) return false;
    if (diceCards.size() > 1) return false;
    vector<Card> cards;
    if (diceCards.size() == 0)
    {
        cards = my_card_stat.card1;
        unique_copy(my_card_stat.card2.begin(), my_card_stat.card2.end(), back_inserter(cards));
        unique_copy(my_card_stat.card3.begin(), my_card_stat.card3.end(), back_inserter(cards));
    }

    int diff = cards.size() + diceCards.size() - card_stat.len;
    if (diff < 0 ) return false; //长度上就不占优势，找不到的
    return find_anaCards_type_with_ghost(diff, cards, diceCards);
}

/*
 @brief 在my_card_stat中找到比card_stat中更大的顺子
 @param diceCards[in] 可用的鬼牌
 @retur true，找到了 false 没有找到
*/
bool CardFind::find_oneline(const vector<Card> &diceCards)
{
    if (card_stat.len < 5) return false;
    int diff = my_card_stat.line1.size() + diceCards.size() - card_stat.len;
    if (diff < 0 ) return false; //长度上就不占优势，找不到的
    return find_anaCards_type_with_ghost(diff, my_card_stat.line1, diceCards);
}

/*
 @brief 在my_card_stat中找到比card_stat中更大的对子
*/
bool CardFind::find_two(const vector<Card> &diceCards)
{
    if (card_stat.len != 2) return false;
    if (diceCards.size() > 2) return false;
    
    vector<Card> cards;
    copy_card1_without_king(my_card_stat.card1, cards);
    std::copy(my_card_stat.card2.begin(), my_card_stat.card2.end(), back_inserter(cards));
    std::copy(my_card_stat.card3.begin(), my_card_stat.card3.end(), back_inserter(cards));

    int diff = cards.size() + diceCards.size() - card_stat.len;
    if (diff < 0 ) return false; //长度上就不占优势，找不到的
    return find_anaCards_type_with_ghost(diff, cards, diceCards);
}

/*
 @brief 在my_card_stat中找到比card_stat中更大的双顺
 @param diceCards[in] 可用的鬼牌
 @retur true，找到了 false 没有找到
*/
bool CardFind::find_twoline(const vector<Card> &diceCards)
{
    if (card_stat.len % 2 != 0 || card_stat.len < 6) return false;

    vector<Card> cards;
    std::merge(my_card_stat.card1.begin(), my_card_stat.card1.end(),
              my_card_stat.line2.begin(), my_card_stat.line2.end(), back_inserter(cards)); //merge合并后依然是有序的，不用排序

    if (card_stat.len % 2 != 0) return false;
    int diff = cards.size() + diceCards.size() - card_stat.len;
    if (diff < 0 ) return false; //长度上就不占优势，找不到的

    return find_anaCards_type_with_ghost(diff, cards, diceCards);
}

/*
 @brief 在my_card_stat中找到比card_stat中更大的三张
*/
bool CardFind::find_three(const vector<Card> &diceCards)
{
    if (card_stat.len != 3) return false;
    if (diceCards.size() > 3) return false;
    vector<Card> cards;
	// copy_card1_without_king(my_card_stat.card1, cards);
	// std::copy(my_card_stat.card2.begin(), my_card_stat.card2.end(), back_inserter(cards));
	// std::copy(my_card_stat.card3.begin(), my_card_stat.card3.end(), back_inserter(cards));
	if (diceCards.size() == 0)
	{
		std::copy(my_card_stat.card3.begin(), my_card_stat.card3.end(), back_inserter(cards));
	}
	else if (diceCards.size() == 1)
	{
		std::copy(my_card_stat.card2.begin(), my_card_stat.card2.end(), back_inserter(cards));
	}
	else if (diceCards.size() == 3)
	{
		copy_card1_without_king(my_card_stat.card1, cards);
	}

    int diff = cards.size() + diceCards.size() - card_stat.len;
    if (diff < 0 ) return false; //长度上就不占优势，找不到的
    return find_anaCards_type_with_ghost(diff, cards, diceCards);
}

/*
 @brief 在my_card_stat中找到比card_stat中更大的三顺
 @param diceCards[in] 可用的鬼牌
 @retur true，找到了 false 没有找到
*/
bool CardFind::find_threeline(const vector<Card> &diceCards)
{
    if (card_stat.len % 3 != 0) return false;
    vector<Card> cards;
    std::merge(my_card_stat.card1.begin(), my_card_stat.card1.end(),
              my_card_stat.card2.begin(), my_card_stat.card2.end(), back_inserter(cards)); //merge合并后依然是有序的，不用排序
    std::copy(my_card_stat.line3.begin(), my_card_stat.line3.end(), back_inserter(cards));
    Card::sort_by_ascending(cards);

    int diff = cards.size() + diceCards.size() - card_stat.len;
    if (diff < 0 ) return false; //长度上就不占优势，找不到的

    return find_anaCards_type_with_ghost(diff, cards, diceCards);
}

/*
 @brief 在my_card_stat中找到比card_stat中更大的飞机带羿, 三带1也能处理
 @param diceCards[in] 可用的鬼牌
 @retur true，找到了 false 没有找到
*/
bool CardFind::find_planewithone(const vector<Card> &diceCards)
{
    if (card_stat.len % 4 != 0) return false;

    if (card_stat.len == 4 && diceCards.size() == 4) return false;

    const unsigned fixCardSize = card_stat.len / 4;

    vector<Card> cards;
    //首先找到要带出去的最小单牌，把单牌放在cards最前
    unsigned cnt = 0;
    for (; (cnt < my_card_stat.card1.size()) && (cnt < fixCardSize); ++cnt)
    {
        cards.push_back(my_card_stat.card1[cnt]);
    }

    int card1Cnt = cards.size();
    for (unsigned i = 0; (i < my_card_stat.card2.size()) && (cnt < fixCardSize); i += 2, cnt += 2)
    {
        cards.push_back(my_card_stat.card2[i]);
        cards.push_back(my_card_stat.card2[i + 1]);
    }
    int card2Cnt = cards.size() - card1Cnt;

    if (my_card_stat.card1.size() - card1Cnt > 0) 
    {
        std::copy(my_card_stat.card1.begin() + card1Cnt, my_card_stat.card1.end(), back_inserter(cards));
        std::copy(my_card_stat.card2.begin(), my_card_stat.card2.end(), back_inserter(cards));
    }
    else if (my_card_stat.card2.size() - card2Cnt > 0)
    {
        std::copy(my_card_stat.card2.begin() + card2Cnt, my_card_stat.card2.end(), back_inserter(cards));
    }
	std::copy(my_card_stat.card3.begin(), my_card_stat.card3.end(), back_inserter(cards));

	int diff = cards.size() + diceCards.size() - card_stat.len;
    if (diff < 0 ) return false; //长度上就不占优势，找不到的
    return find_anaCards_type_with_ghost(diff, cards, diceCards, fixCardSize);
}

/*
 @brief 在my_card_stat中找到比card_stat中更大的飞机带翅 三带对也能处理
 @param diceCards[in] 可用的鬼牌
 @retur true，找到了 false 没有找到
*/
bool CardFind::find_planewithtwo(const vector<Card> &diceCards)
{
    if (card_stat.len % 5 != 0 ) return false;
    const unsigned fixCardSize = (card_stat.len / 5) * 2;

    vector<Card> cards;
    //首先找到要带出去的最小对子，把对子放在cards最前
    unsigned cnt = 0;
    for (; (cnt < my_card_stat.card2.size()) && (cnt < fixCardSize); cnt += 2)
    {
        cards.push_back(my_card_stat.card2[cnt]);
        cards.push_back(my_card_stat.card2[cnt + 1]);
    }

    int card2Cnt = cards.size();
    int card1Cnt = 0;
    int card3Cnt = 0;
    for (unsigned i = 0; (i < my_card_stat.card3.size()) && (cnt < fixCardSize); cnt += 2, i += 3)
    {
        card3Cnt += 3;
        cards.push_back(my_card_stat.card3[i]);
        cards.push_back(my_card_stat.card3[i+1]);
        // cards.push_back(my_card_stat.card3[i+2]);
    }
    for (int i = 0; i < (card3Cnt / 3); ++i)
    {
        cards.push_back(my_card_stat.card3[i + 2]);
    }
    for (unsigned i = 0; (i < my_card_stat.card1.size()) && (cnt < fixCardSize); cnt += 2, ++i)
    {
        ++card1Cnt;
        cards.push_back(my_card_stat.card1[i]);
    }

    if (my_card_stat.card2.size() - card2Cnt > 0)
    {
        std::copy(my_card_stat.card1.begin(), my_card_stat.card1.end(), back_inserter(cards));
        std::copy(my_card_stat.card2.begin() + card2Cnt, my_card_stat.card2.end(), back_inserter(cards));
        std::copy(my_card_stat.card3.begin(), my_card_stat.card3.end(), back_inserter(cards));
    }
    else if (my_card_stat.card3.size() - card3Cnt> 0)
    {
        std::copy(my_card_stat.card3.begin() + card3Cnt, my_card_stat.card3.end(), back_inserter(cards));
        std::copy(my_card_stat.card1.begin(), my_card_stat.card1.end(), back_inserter(cards));
    }
    else if (my_card_stat.card1.size() - card1Cnt > 0)
    {
        std::copy(my_card_stat.card1.begin() + card1Cnt, my_card_stat.card1.end(), back_inserter(cards));
    }

	int diff = cards.size() + diceCards.size() - card_stat.len;
    if (diff < 0 ) return false; //长度上就不占优势，找不到的
    return find_anaCards_type_with_ghost(diff, cards, diceCards, fixCardSize);
}

/*
 @brief 在my_card_stat中找到比card_stat中更大的四带单牌
 @param diceCards[in] 可用的鬼牌
 @retur true，找到了 false 没有找到
*/
bool CardFind::find_fourwithone(const vector<Card> &diceCards)
{
    if (card_stat.len != 6) return false;

    vector<Card> cards;
    int card1Cnt = 0, card2Cnt = 0, card3Cnt = 0;
    //首先找到要带出去的两张最小单牌，把单牌放在cards最前
    if (my_card_stat.card1.size() > 0)
    {
        cards.push_back(my_card_stat.card1[0]);
        ++card1Cnt;
        if (my_card_stat.card1.size() > 1)
        {
            cards.push_back(my_card_stat.card1[1]);
            ++card1Cnt;
        }
    }
    if (cards.size() < 2 && my_card_stat.card2.size() > 0)
    {
        cards.push_back(my_card_stat.card2[0]);
        cards.push_back(my_card_stat.card2[1]);
        card2Cnt = 2;
    }
    if (cards.size() < 2 && my_card_stat.card3.size() > 0)
    {
        cards.push_back(my_card_stat.card3[0]);
        cards.push_back(my_card_stat.card3[1]);
        cards.push_back(my_card_stat.card3[2]);
        card2Cnt = 3;
    }
    std::copy(my_card_stat.card1.begin() + card1Cnt, my_card_stat.card1.end(), back_inserter(cards));
    std::copy(my_card_stat.card2.begin() + card2Cnt, my_card_stat.card2.end(), back_inserter(cards));
    std::copy(my_card_stat.card3.begin() + card3Cnt, my_card_stat.card3.end(), back_inserter(cards));
    std::copy(my_card_stat.card4.begin(), my_card_stat.card4.end(), back_inserter(cards));


    int diff = cards.size() + diceCards.size() - card_stat.len;
    if (diff < 0 ) return false; //长度上就不占优势，找不到的
    return find_anaCards_type_with_ghost(diff, cards, diceCards, 2);
}

/*
 @brief 在my_card_stat中找到比card_stat中更大的四带对
 @param diceCards[in] 可用的鬼牌
 @retur true，找到了 false 没有找到
*/
bool CardFind::find_fourwithtwo(const vector<Card> &diceCards)
{
    if (card_stat.len  != 8) return false;

    vector<Card> cards;
    int card1Cnt = 0, card2Cnt = 0, card3Cnt = 0, cnt = 0;
    //首先找到要带出去的最小对子，把对子放在cards最前
    if (my_card_stat.card2.size() > 0)
    {
        cards.push_back(my_card_stat.card2[0]);
        cards.push_back(my_card_stat.card2[1]);
        card2Cnt = 2;
        cnt = 2;
        if (my_card_stat.card2.size() > 2)
        {
            cards.push_back(my_card_stat.card2[2]);
            cards.push_back(my_card_stat.card2[3]);
            card2Cnt = 4;
            cnt = 4;
        }
    }
    if (cnt < 4 && my_card_stat.card3.size() > 0)
    {
        cards.push_back(my_card_stat.card3[0]);
        cards.push_back(my_card_stat.card3[1]);
        card3Cnt = 3;
        cnt += 3;
        if (my_card_stat.card2.size() > 4)
        {
            cards.push_back(my_card_stat.card2[4]);
            cards.push_back(my_card_stat.card2[5]);

            cards.push_back(my_card_stat.card3[2]);
            cards.push_back(my_card_stat.card2[6]);
            card3Cnt = 6;
            cnt += 3;
        }
        else
            cards.push_back(my_card_stat.card3[2]);
    }
    if (cnt < 4 && my_card_stat.card1.size() > 0)
    {
        for (unsigned i = 0; (i < my_card_stat.card1.size()) && (cnt < 4); cnt += 2, ++i)
        {
            ++card1Cnt;
            cards.push_back(my_card_stat.card1[i]);
        }
    }
    std::copy(my_card_stat.card1.begin() + card1Cnt, my_card_stat.card1.end(), back_inserter(cards));
    std::copy(my_card_stat.card2.begin() + card2Cnt, my_card_stat.card2.end(), back_inserter(cards));
    std::copy(my_card_stat.card3.begin() + card3Cnt, my_card_stat.card3.end(), back_inserter(cards));
    std::copy(my_card_stat.card4.begin(), my_card_stat.card4.end(), back_inserter(cards));

    int diff = cards.size() + diceCards.size() - card_stat.len;
    if (diff < 0 ) return false; //长度上就不占优势，找不到的
    return find_anaCards_type_with_ghost(diff, cards, diceCards, 4);
}

/*
 @brief 在my_card_stat中找到比card_stat中更大的四带单牌或对
 @param diceCards[in] 可用的鬼牌
 @retur true，找到了 false 没有找到
*/
bool CardFind::find_anaCards_type_with_ghost(int diff, const vector<Card> &cards, const vector<Card> &diceCards, unsigned fixCardSize)
{
    bool founded = false; //假设找不到

    vector<Card> fixCards = diceCards;
    for (unsigned i = 0; i < fixCardSize; ++i)
    {
        fixCards.push_back(cards[i]);
    }
    if (fixCards.size() >= card_stat.len) //固定的牌就已经可以比较了
    {
        if (CardAnalysis::isGreater(fixCards, ghost_face, compare_type, compare_face))
        {
            results.push_back(fixCards);
            robot_data_analysis(fixCards);
            return true;
        }
        return false;
    }

    vector<Card> cards2 = cards;
    //除固定牌外还需要加入其它牌进行比较
    for (int index = 0; index <= diff; ++index)
    {
        vector<Card> anaCards = fixCards;
        for (unsigned i = anaCards.size(), j = 0; i < card_stat.len; ++i, ++j)
        { //加入其它牌
            anaCards.push_back(cards[index + fixCardSize + j]);
        }
        if (CardAnalysis::isGreater(anaCards, ghost_face, compare_type, compare_face))
        {
			// 去掉面值重复的
			bool already_has = check_face_reprat(results, anaCards, compare_type);
			if (!already_has)
			{
				results.push_back(anaCards);
				robot_data_analysis(anaCards);
				founded = true;
			}
			
        }
        while (cards.size() > index + card_stat.len - diceCards.size())
        { //加入分析的牌等于去的分析的牌，就会重叠，直接跳过
            if (cards[fixCardSize + index] == cards[index + card_stat.len - diceCards.size()]) 
            {
                ++index;
                continue;
            }
            break;
        }
    }
    return founded;
}

/*
 @brief 复制card1中的牌到cards。但不包括大小王
 @param card1[in] 需要复制的牌
 @param cards[in] 复制的位置
*/
void CardFind::copy_card1_without_king(const vector<Card> &card1, vector<Card> &cards)
{
    for (unsigned i = 0; i< card1.size(); ++i)
    {
        if (card1[i].face == Card::Small || card1[i].face == Card::Big) continue;
        cards.push_back(card1[i]);
    }
}


/*
 @brief 分析找到的ana_cards的牌，看这个牌有没有拆对子，拆三张（只用要比较的牌是对子或）
 @param ana_cards[in] 当前找到的牌ana_cards
*/
void CardFind::robot_data_analysis(const vector<Card> &ana_cards)
{
    if (!robot_flag) return;

    if (compare_type == CARD_TYPE_ONE)
    {
        if (ana_cards.size() <= 0) return;
        if (std::binary_search(my_card_stat.card2.begin(), my_card_stat.card2.end(), ana_cards[0]))
        {
            from_two.push_back(results.size() - 1);
            return;
        }
    }

    if (compare_type == CARD_TYPE_ONE || compare_type == CARD_TYPE_TWO)
    {
        if (ana_cards.size() <= 0) return;
        if (std::binary_search(my_card_stat.card3.begin(), my_card_stat.card3.end(), ana_cards[0]))
        {
            from_three.push_back(results.size() - 1);
            return;
        }
    }
}

/*
 @brief 
*/

bool CardFind::check_face_reprat(const vector<vector<Card> > & compare_cards, const vector<Card> & my_cards, const int compare_type)
{
	vector<vector<Card> > _results = compare_cards;
	vector<Card> _my_cards = my_cards;
	for (unsigned k = 0; k < _results.size(); k++)
	{
		CardAnalysis _resCardAna(_results[k], ghost_face);
		CardAnalysis _myCardAna(_my_cards, ghost_face);
		const int res_find_face = _resCardAna.get_card_face_of_type(compare_type);
		const int my_find_face = _myCardAna.get_card_face_of_type(compare_type);
		if (my_find_face == res_find_face)
		{
			return true;
		}
	}
	return false;
}
//end 工具函数
/**************下面是机器人相关的函数*************************************/

void CardFind::set_robot_flag(bool robotFlag)
{
    robot_flag = robotFlag;
}

//找到指定的炸弹，则返回炸弹在results中的下标，否则返回-1
int CardFind::has_bomb_type_of(int find_type)
{
	//vector<vector<Card> > results;
    for (int i = results.size() - 1; i >= 0; --i)
    {
        if (results[i].size() != 4) continue;

        CardStatistics st(results[i], ghost_face);
        if (st.card4.size() == 4 && find_type == CARD_TYPE_BOMB)
            return i;
        if (st.ghost_cards.size() == 4 && find_type == CARD_TYPE_GHOSTBOMB)
            return i;
        if (find_type == CARD_TYPE_SOFTBOMB)
        {
            if (st.ghost_cards.size() + st.card1.size() == 4 ||
                st.ghost_cards.size() + st.card2.size() == 4 ||
                st.ghost_cards.size() + st.card3.size() == 4)
                return i;
        }
    }
    return -1;
}

//***********************下面是以前，无癞子的代码****************************
int CardFind::find(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &target_card_stat)
{
	clear();
	if (card_ana.type != CARD_TYPE_ERROR)
	{
		if (card_ana.type == CARD_TYPE_ROCKET)
		{
			return 0;
		}
		
		if (card_ana.type == CARD_TYPE_ONE)
		{
			find_one(card_ana, card_stat, target_card_stat);
			find_two(card_ana, card_stat, target_card_stat);
			find_three(card_ana, card_stat, target_card_stat);
		}
		else if (card_ana.type == CARD_TYPE_TWO)
		{
			find_two(card_ana, card_stat, target_card_stat);
			find_three(card_ana, card_stat, target_card_stat);
		}
		else if (card_ana.type == CARD_TYPE_THREE)
		{
			find_three(card_ana, card_stat, target_card_stat);
		}
		else if (card_ana.type == CARD_TYPE_ONELINE)
		{
			find_one_ine(card_ana, card_stat, target_card_stat);
		}
		else if (card_ana.type == CARD_TYPE_TWOLINE)
		{
			find_two_ine(card_ana, card_stat, target_card_stat);
		}
		else if (card_ana.type == CARD_TYPE_THREELINE)
		{
			find_three_line(card_ana, card_stat, target_card_stat);
		}
		else if (card_ana.type == CARD_TYPE_THREEWITHONE)
		{
			find_three_with_one(card_ana, card_stat, target_card_stat);
		}
		else if (card_ana.type == CARD_TYPE_THREEWITHTWO)
		{
			find_three_with_two(card_ana, card_stat, target_card_stat);
		}
		else if (card_ana.type == CARD_TYPE_PLANEWITHONE)
		{
			find_plane_with_one(card_ana, card_stat, target_card_stat);
		}
		else if (card_ana.type == CARD_TYPE_PLANEWITHWING)
		{
			find_plane_with_wing(card_ana, card_stat, target_card_stat);
		}
		else if (card_ana.type == CARD_TYPE_FOURWITHONE)
		{
			find_four_with_one(card_ana, card_stat, target_card_stat);
		}
		else if (card_ana.type == CARD_TYPE_FOURWITHTWO)
		{
			find_four_with_two(card_ana, card_stat, target_card_stat);
		}
		find_bomb(card_ana, card_stat, target_card_stat);
		find_rocket(card_ana, card_stat, target_card_stat);
	}
	
	return 0;
}

void CardFind::find_one(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat)
{
	if (card_ana.type == CARD_TYPE_ONE) 
	{
		for (unsigned int i = 0; i < my_card_stat.card1.size(); i++)
		{
			if (my_card_stat.card1[i].face > card_ana.face)
			{
				vector<Card> cards;
				cards.push_back(my_card_stat.card1[i]);
				results.push_back(cards);
			}
		}
	}
}

void CardFind::find_two(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat)
{
	if (card_ana.type == CARD_TYPE_ONE) 
	{
		for (unsigned int i = 0; i < my_card_stat.card2.size(); i += 2)
		{
			if (my_card_stat.card2[i].face > card_ana.face)
			{
				vector<Card> cards;
				cards.push_back(my_card_stat.card2[i]);
				results.push_back(cards);
                robot_data_analysis(cards);
			}
		}
	}
	else if (card_ana.type == CARD_TYPE_TWO) 
	{
		for (unsigned int i = 0; i < my_card_stat.card2.size(); i += 2)
		{
			if (my_card_stat.card2[i].face > card_ana.face)
			{
				vector<Card> cards;
				cards.push_back(my_card_stat.card2[i]);
				cards.push_back(my_card_stat.card2[i + 1]);
				results.push_back(cards);
                robot_data_analysis(cards);
			}
		}
	}
}

void CardFind::find_three(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat)
{
	if (card_ana.type == CARD_TYPE_ONE) 
	{
		for (unsigned int i = 0; i < my_card_stat.card3.size(); i += 3)
		{
			if (my_card_stat.card3[i].face > card_ana.face)
			{
				vector<Card> cards;
				cards.push_back(my_card_stat.card3[i]);
				results.push_back(cards);
                robot_data_analysis(cards);
			}
		}
	}
	else if (card_ana.type == CARD_TYPE_TWO) 
	{
		for (unsigned int i = 0; i < my_card_stat.card3.size(); i += 3)
		{
			if (my_card_stat.card3[i].face > card_ana.face)
			{
				vector<Card> cards;
				cards.push_back(my_card_stat.card3[i]);
				cards.push_back(my_card_stat.card3[i + 1]);
				Card::dump_cards(cards);
				results.push_back(cards);
                robot_data_analysis(cards);
			}
		}
	}
	else if (card_ana.type == CARD_TYPE_THREE) 
	{
		for (unsigned int i = 0; i < my_card_stat.card3.size(); i += 3)
		{
			if (my_card_stat.card3[i].face > card_ana.face)
			{
				vector<Card> cards;
				cards.push_back(my_card_stat.card3[i]);
				cards.push_back(my_card_stat.card3[i + 1]);
				cards.push_back(my_card_stat.card3[i + 2]);
				results.push_back(cards);
                robot_data_analysis(cards);
			}
		}
	}
}

void CardFind::find_one_ine(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat)
{
	int count = my_card_stat.line1.size() - card_stat.line1.size();
	for (int i = 0; i <= count; i++)
	{
		if (my_card_stat.line1[i].face > card_ana.face)
		{
			int end = i + card_ana.len;
			if (card_ana.check_arr_is_line(my_card_stat.line1, 1, i, end))
			{
				vector<Card> cards;
				for (int j = i; j < end; j++)
				{
					cards.push_back(my_card_stat.line1[j]);	
				}
				results.push_back(cards);
			}	
		}
	}
}

void CardFind::find_two_ine(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat)
{
	int count = my_card_stat.line2.size() - card_stat.line2.size();
	for (int i = 0; i <= count; i += 2)
	{
		if (my_card_stat.line2[i].face > card_ana.face)
		{
			int end = i + card_ana.len;
			if (card_ana.check_arr_is_line(my_card_stat.line2, 2, i, end))
			{
				vector<Card> cards;
				for (int j = i; j < end; j++)
				{
					cards.push_back(my_card_stat.line2[j]);	
				}
				results.push_back(cards);
			}	
		}
	}
}

void CardFind::find_three_line(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat)
{
	int count = my_card_stat.line3.size() - card_stat.line3.size();
	for (int i = 0; i <= count; i += 3)
	{
		if (my_card_stat.line3[i].face > card_ana.face)
		{
			int end = i + card_ana.len;
			if (card_ana.check_arr_is_line(my_card_stat.line3, 3, i, end))
			{
				vector<Card> cards;
				for (int j = i; j < end; j++)
				{
					cards.push_back(my_card_stat.line3[j]);	
				}
				results.push_back(cards);
			}	
		}
	}	
}

void CardFind::find_three_with_one(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat)
{
	if (my_card_stat.len < 4)
	{
		return;
	}
	
	for (unsigned int i = 0; i < my_card_stat.card3.size(); i += 3)
	{
		if (my_card_stat.card3[i].face > card_ana.face)
		{
			vector<Card> cards;
			cards.push_back(my_card_stat.card3[i]);
			cards.push_back(my_card_stat.card3[i + 1]);
			cards.push_back(my_card_stat.card3[i + 2]);
			if (my_card_stat.card1.size() > 0)
			{
				cards.push_back(my_card_stat.card1[0]);
			}
			else
			{
				for (unsigned int j = 0; j < my_card_stat.line1.size(); j++)
				{
					if (my_card_stat.line1[j].face != cards[0].face) {
						cards.push_back(my_card_stat.line1[j]);
						break;
					}
				}
			}
			results.push_back(cards);
		}
	}
}

void CardFind::find_three_with_two(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat)
{
	if (my_card_stat.len < 5)
	{
		return;
	}
	
	for (unsigned int i = 0; i < my_card_stat.card3.size(); i += 3)
	{
		if (my_card_stat.card3[i].face > card_ana.face)
		{
			vector<Card> cards;
			cards.push_back(my_card_stat.card3[i]);
			cards.push_back(my_card_stat.card3[i + 1]);
			cards.push_back(my_card_stat.card3[i + 2]);
			if (my_card_stat.card2.size() > 0)
			{
				cards.push_back(my_card_stat.card2[0]);
				cards.push_back(my_card_stat.card2[1]);
			}
			else
			{
				for (unsigned int j = 0; j < my_card_stat.line2.size(); j++)
				{
					if (my_card_stat.line2[j].face != cards[0].face) {
						cards.push_back(my_card_stat.line2[j]);
						cards.push_back(my_card_stat.line2[j + 1]);
						break;
					}
				}
			}
			results.push_back(cards);
		}
	}
}

void CardFind::find_plane_with_one(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat)
{
	int count = my_card_stat.line3.size() - card_stat.line3.size();
	for (int i = 0; i <= count; i += 3)
	{
		if (my_card_stat.line3[i].face > card_ana.face)
		{
			int end = i + card_stat.card3.size();
			if (card_ana.check_arr_is_line(my_card_stat.line3, 3, i, end))
			{
				vector<Card> cards;
				for (int j = i; j < end; j++)
				{
					cards.push_back(my_card_stat.line3[j]);
				}
				// printf("aaa1[%u][%u]\n", cards.size(), card_ana.len);
				for (unsigned int j = 0; j < my_card_stat.card1.size(); j++)
				{
					cards.push_back(my_card_stat.card1[j]);
					if (cards.size() == card_ana.len)
					{
						break;
					}
				}
					
				if (cards.size() == card_ana.len)
				{ 
					results.push_back(cards);
					continue;
				}
					
				int flag = 0;
				for (unsigned int j = 0; j < my_card_stat.line1.size(); j++)
				{
					flag = 0;
					for (unsigned int k = 0; k < cards.size(); k++)
					{
						if (cards[k].face == my_card_stat.line1[j].face)
						{
							flag = 1;
							break;
						}
					}
						
					if (flag == 1)
					{
						continue;
					}
						
					cards.push_back(my_card_stat.line1[j]);	
					if (cards.size() == card_ana.len)
					{
						break;
					}
				}
				
				if (cards.size() == card_ana.len)
				{
					results.push_back(cards);
					continue;
				}
			}
		}
	}
}

void CardFind::find_plane_with_wing(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat)
{
	int count = my_card_stat.line3.size() - card_stat.line3.size();
	for (int i = 0; i <= count; i += 3)
	{
		if (my_card_stat.line3[i].face > card_ana.face)
		{
			int end = i + card_stat.card3.size();
			if (card_ana.check_arr_is_line(my_card_stat.line3, 3, i, end))
			{
				vector<Card> cards;
				for (int j = i; j < end; j++)
				{
					cards.push_back(my_card_stat.line3[j]);
				}
				// printf("aaa1[%u][%u]\n", cards.size(), card_ana.len);
				for (unsigned int j = 0; j < my_card_stat.card2.size(); j += 2)
				{
					cards.push_back(my_card_stat.card2[j]);
					cards.push_back(my_card_stat.card2[j + 1]);
					if (cards.size() == card_ana.len)
					{
						break;
					}
				}
					
				if (cards.size() == card_ana.len)
				{ 
					results.push_back(cards);
					continue;
				}
					
				int flag = 0;
				for (unsigned int j = 0; j < my_card_stat.line2.size(); j += 2)
				{
					flag = 0;
					for (unsigned int k = 0; k < cards.size(); k++)
					{
						if (cards[k].face == my_card_stat.line2[j].face)
						{
							flag = 1;
							break;
						}
					}
						
					if (flag == 1)
					{
						continue;
					}
						
					cards.push_back(my_card_stat.line2[j]);
					cards.push_back(my_card_stat.line2[j + 1]);
					if (cards.size() == card_ana.len)
					{
						break;
					}
				}
				
				if (cards.size() == card_ana.len)
				{
					results.push_back(cards);
					continue;
				}
			}
		}
	}	
}

void CardFind::find_four_with_one(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat)
{
	if (my_card_stat.len < 6)
	{
		return;
	}
	
	for (unsigned int i = 0; i < my_card_stat.card4.size(); i += 4)
	{
		if (my_card_stat.card4[i].face > card_ana.face)
		{
			vector<Card> cards;
			cards.push_back(my_card_stat.card4[i]);
			cards.push_back(my_card_stat.card4[i + 1]);
			cards.push_back(my_card_stat.card4[i + 2]);
			cards.push_back(my_card_stat.card4[i + 3]);

			for (unsigned int j = 0; j < my_card_stat.card1.size(); j++)
			{
				cards.push_back(my_card_stat.card1[j]);
				if (cards.size() == card_ana.len)
				{
					break;
				}
			}
					
			if (cards.size() == card_ana.len)
			{ 
				results.push_back(cards);
				continue;
			}
					
			int flag = 0;
			for (unsigned int j = 0; j < my_card_stat.line1.size(); j++)
			{
				flag = 0;
				for (unsigned int k = 0; k < cards.size(); k++)
				{
					if (cards[k].face == my_card_stat.line1[j].face)
					{
						flag = 1;
						break;
					}
				}
						
				if (flag == 1)
				{
					continue;
				}
						
				cards.push_back(my_card_stat.line1[j]);	
				if (cards.size() == card_ana.len)
				{
					break;
				}
			}
				
			if (cards.size() == card_ana.len)
			{
				results.push_back(cards);
				continue;
			}
		}
	}
}

void CardFind::find_four_with_two(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat)
{
	if (my_card_stat.len < 8)
	{
		return;
	}
	
	for (unsigned int i = 0; i < my_card_stat.card4.size(); i += 4)
	{
		if (my_card_stat.card4[i].face > card_ana.face)
		{
			vector<Card> cards;
			cards.push_back(my_card_stat.card4[i]);
			cards.push_back(my_card_stat.card4[i + 1]);
			cards.push_back(my_card_stat.card4[i + 2]);
			cards.push_back(my_card_stat.card4[i + 3]);

			for (unsigned int j = 0; j < my_card_stat.card2.size(); j += 2)
			{
				cards.push_back(my_card_stat.card2[j]);
				cards.push_back(my_card_stat.card2[j + 1]);
				if (cards.size() == card_ana.len)
				{
					break;
				}
			}
					
			if (cards.size() == card_ana.len)
			{ 
				results.push_back(cards);
				continue;
			}
					
			int flag = 0;
			for (unsigned int j = 0; j < my_card_stat.line2.size(); j += 2)
			{
				flag = 0;
				for (unsigned int k = 0; k < cards.size(); k++)
				{
					if (cards[k].face == my_card_stat.line2[j].face)
					{
						flag = 1;
						break;
					}
				}
						
				if (flag == 1)
				{
					continue;
				}
						
				cards.push_back(my_card_stat.line2[j]);
				cards.push_back(my_card_stat.line2[j + 1]);
				if (cards.size() == card_ana.len)
				{
					break;
				}
			}
				
			if (cards.size() == card_ana.len)
			{
				results.push_back(cards);
				continue;
			}
		}
	}	
}

void CardFind::find_bomb(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat)
{
	if (card_ana.type == CARD_TYPE_BOMB)
	{
		for (unsigned int i = 0; i < my_card_stat.card4.size(); i += 4)
		{
			if (my_card_stat.card4[i].face > card_ana.face)
			{
				vector<Card> cards;
				cards.push_back(my_card_stat.card4[i]);
				cards.push_back(my_card_stat.card4[i + 1]);
				cards.push_back(my_card_stat.card4[i + 2]);
				cards.push_back(my_card_stat.card4[i + 3]);
				results.push_back(cards);
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < my_card_stat.card4.size(); i += 4)
		{
			vector<Card> cards;
			cards.push_back(my_card_stat.card4[i]);
			cards.push_back(my_card_stat.card4[i + 1]);
			cards.push_back(my_card_stat.card4[i + 2]);
			cards.push_back(my_card_stat.card4[i + 3]);
			results.push_back(cards);
		}
	}	
}

void CardFind::find_rocket(CardAnalysis &card_ana, CardStatistics &card_stat, CardStatistics &my_card_stat)
{
	int len = my_card_stat.card1.size();
	if (len >= 2)
	{
		if (my_card_stat.card1[len - 2].face == 16
			&& my_card_stat.card1[len - 1].face == 17)
		{
			vector<Card> cards;
			cards.push_back(my_card_stat.card1[len - 2]);
			cards.push_back(my_card_stat.card1[len - 1]);
			results.push_back(cards);		
		}
	}	
}

int CardFind::find_straight(vector<int> &input, vector<int> &output)
{
    if (input.size() == 0)
    {
        return -1;
    }
    
	vector<Card> cards0;
	vector<Card> cards1;
	for (unsigned int i = 0; i < input.size(); i++)
	{
		Card card(input[i]);
		cards0.push_back(card);
	}
	
	CardStatistics card_stat;
	card_stat.statistics(cards0);
	CardFind::get_straight(card_stat, cards1);
	
	output.clear();
	
	for (unsigned int i = 0; i < cards1.size(); i++)
	{
		output.push_back(cards1[i].value);
	}
	
	return 0;
}

int CardFind::get_straight(CardStatistics &card_stat, vector<Card> &output)
{
	vector<Card> straight_one_longest;
	vector<Card> straight_two_longest;
	vector<Card> straight_three_longest;

	get_longest_straight(card_stat.line1, 1, straight_one_longest);
	get_longest_straight(card_stat.line2, 2, straight_two_longest);
	get_longest_straight(card_stat.line3, 3, straight_three_longest);
	
	Card::dump_cards(straight_one_longest, "One");
	Card::dump_cards(straight_two_longest, "Two");
	Card::dump_cards(straight_three_longest, "Three");
	
	output.clear();

	int cnt = get_max(straight_one_longest.size(), straight_two_longest.size(), straight_three_longest.size());
	if (cnt == 1)
	{
		output = straight_one_longest;
	}
	else if (cnt == 2)
	{
		output = straight_two_longest;
	}
	else if (cnt == 3)
	{
		output = straight_three_longest;
#if 0
		int one = straight_three_longest.size();
		int cnt = card_stat.card1.size();
		if (cnt > one)
		{
			for (int i = 0; i < cnt; i++)
			{
				if (i == one)
				{
					for (int j = 0; i <= one; j++)
					{
						output.push_back(card_stat.card1[i]);
					}
					return 0;
				}
				if (card_stat.card1[i].face == 16 ||
						card_stat.card1[i].face == 17)
	            {
	            	break;
	            }
			}
		}
#endif
	}
	
	return 0;
}

int CardFind::get_max(unsigned int a, unsigned int b, unsigned int c)
{
	if (c >= a && c >= b && c >= 6)
	{
		return 3;
	}
	
	if (b >= a && b >= c && b >= 6)
	{
		return 2;
	}
	
	if (a >= b && a >= c && a >= 5)
	{
		return 1;
	}
	
	return 0;
}

void CardFind::get_longest_straight(vector<Card> &input, int type, vector<Card> &output)
{
	unsigned int cnt = 0;
	unsigned int last_cnt = 0;
	unsigned int index = 0;
	unsigned int i = 0;
	Card temp;
	int flag = 0;
	for (i = 0; i < input.size(); i += type)
	{
		if (input[i].face >= 15)
		{
			break;
		}
		// printf("[%d][%d][%d][%d][%d][%d]\n", type, input[i].face, temp.face, cnt,last_cnt, index);
		if ((input[i].face - temp.face) != 1)
		{
			if (cnt > last_cnt)
			{
				index = i;
				last_cnt = cnt;
			}
			flag = 1;
			cnt = 0;
		}
		else
		{
			flag = 0;
		}
		cnt += type;
		temp = input[i];
	}
	
	if (flag == 0)
	{
		if (cnt > last_cnt)
		{
			index = i;
			last_cnt = cnt;
		}
	}
	output.clear();
	// printf("copy[%d][%u][%u]\n", type, index - last_cnt, index);
	for (unsigned int i = (index - last_cnt); i < index; i++)
	{
		output.push_back(input[i]);
	}
}

void CardFind::debug()
{
	for (unsigned int i = 0; i < results.size(); i++)
	{
		Card::dump_cards(results[i], "tip");
	}
}

void CardFind::test(int input0[], int len0, int input1[], int len1)
{
	vector<Card> cards0;
	for (int i = 0; i < len0; i++)
	{
		Card card(input0[i]);
		cards0.push_back(card);
	}
	CardStatistics card_stat0;
	card_stat0.statistics(cards0);
	CardAnalysis card_ana0;
	card_ana0.analysis(card_stat0);
	Card::dump_cards(cards0);
	card_ana0.debug();
	
	vector<Card> cards1;
	for (int i = 0; i < len1; i++)
	{
		Card card(input1[i]);
		cards1.push_back(card);
	}
	CardStatistics card_stat1;
	card_stat1.statistics(cards1);
	
	CardFind card_find;
	// card_find.find(card_ana0, card_stat0, card_stat1);
	card_find.debug();
}

void CardFind::test(int input[], int len)
{
	vector<Card> cards;
	for (int i = 0; i < len; i++)
	{
		Card card(input[i]);
		cards.push_back(card);	
	}
	CardStatistics card_stat;
	card_stat.statistics(cards);
	CardFind::get_straight(card_stat, cards);
	Card::dump_cards(cards, "Longest");
}
