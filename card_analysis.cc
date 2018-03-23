#include "card.h"
#include "card_type.h"
#include "card_statistics.h"
#include "card_analysis.h"

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

CardAnalysis::CardAnalysis()
{
    ghost_face = 0;
    ghost_num = 0;
    type = 0;
    face = 0;
    back_up_face.clear();
    back_up_type.clear();
}

/*
 @brief 分析牌型
 @param anaCards[in] 要分析的牌型
 @param set_ghost_face[in] 鬼牌值
*/
CardAnalysis::CardAnalysis(const std::vector<Card> &anaCards, int set_ghost_face)
{
    card_stat.statistics(anaCards, set_ghost_face);
    do_analysis();
}

/*
 @brief 分析牌型
 @param anacard_stat[in] 要分析的牌型结构
*/
CardAnalysis::CardAnalysis(const CardStatistics &anacard_stat)
{
    card_stat = anacard_stat;
    do_analysis();
}

/*
 @brief 分析牌型
 @param anaCards[in] 要分析的牌型
 @param set_ghost_face[in] 鬼牌值
*/
int CardAnalysis::analysis(const std::vector<Card> &anaCards, int set_ghost_face)
{
    card_stat.statistics(anaCards, set_ghost_face);
    return do_analysis();
}

/*
 @brief 分析牌型,只有当CardAnalysis是默认初始化的时候，才需要这个函数来analysis。
 @param anacard_stat[in] 要分析的牌型结构
 @retur 返回分析的牌型
*/
int CardAnalysis::analysis(const CardStatistics &anacard_stat)
{
    card_stat = anacard_stat;
    return do_analysis();
}

/*
 @breif 查找分析出的牌型中，某个特定牌型的面值
 @param expect_type[in] 要查找的牌型
 @retur 0表示没有找到特定牌型， 其它值，表示特定牌型的面值
*/
int CardAnalysis::get_card_face_of_type(CardType expect_type)
{
    if (type == expect_type) return face;

    for (unsigned i = 0; i < back_up_type.size(); ++i)
    {
        if (back_up_type[i] == expect_type) return back_up_face[i];
    }
    return 0;
}

/*
 @breif 比较两个CardAnalysis对象的牌型大小
 @param card_ana[in] 要比较的对象card_ana
 @retur 如果比card_ana小返回true, 其它情况返回false
*/
bool CardAnalysis::operator<(const CardAnalysis &card_ana) const
{
    if (type == 0) return false;
    if (card_ana.type == 0) return false;

    // if (type == card_ana.type) return true;

    if (type == card_ana.type)
    {
        return face < card_ana.face;
    }
    else if (type >= CARD_TYPE_SOFTBOMB || card_ana.type >= CARD_TYPE_SOFTBOMB)
    {
        return type < card_ana.type;
    }
    return false;
}

/*
 @breif 查看牌型是否比某个指定的牌型大
 @param anaCards[in] 需要比较的牌型
 @param ghost_face[in] 鬼牌
 @param ctype[in] 要比较的类型
 @param cface[in] 要比较的面值
 @retur 如果比card_ana小返回true, 其它情况返回false
*/
bool CardAnalysis::isGreater(const std::vector<Card> &anaCards, int ghost_face, int ctype, int cface)
{
    if (ctype == 0) return false;

    CardAnalysis card_ana(anaCards, ghost_face);
    if (card_ana.type == 0) return false;
    if (ctype >= CARD_TYPE_SOFTBOMB || card_ana.type >= CARD_TYPE_SOFTBOMB)
    {//有炸弹的情况
        if (card_ana.type == ctype)
            return card_ana.face > cface;
        else
            return card_ana.type > ctype;
    }

    const int find_face = card_ana.get_card_face_of_type(static_cast<CardType>(ctype));
    if (find_face == 0) return false;

    return find_face > cface;
}
//*****************************下面是工具函数*******************************
/*
 @brief 分析牌型成员变量card_stat中的牌型
 @retur 返回分析的牌型
*/
int CardAnalysis::do_analysis()
{
    type = CARD_TYPE_ERROR;
    face = 0;
    back_up_face.clear();
    back_up_type.clear();
	
	len = card_stat.len;
    ghost_face = card_stat.ghost_face;
    ghost_num = card_stat.ghost_cards.size();
	
	if (len == 0)
	{
		return type;
	}

    if (card_stat.has_ghost())
    {
        return ghost_analysis();
    }

	if (len == 1)
	{
		face = card_stat.card1[0].face;
		type = CARD_TYPE_ONE;
		return type;
	}
	
	if (len == 2)
	{
		if (card_stat.line1.size() == 1
			&& card_stat.card2.size() == 2)
		{
			face = card_stat.card2[1].face;
			type = CARD_TYPE_TWO;
			return type;
		} 
		else if (card_stat.line1.size() == 2
				&& card_stat.card1.size() == 2
				&& card_stat.card1[0].face == 16
				&& card_stat.card1[1].face == 17)
		{
			face = card_stat.card1[1].face;
			type = CARD_TYPE_ROCKET;
			return type;
		}
	}
	
	if (len == 3)
	{
		if (card_stat.card3.size() == 3)
		{
			face = card_stat.card3[2].face;
			type = CARD_TYPE_THREE;
			return type;
		}
	}
	
	if (len == 4)
	{
		if (card_stat.card4.size() == 4)
		{
			face = card_stat.card4[3].face;
			type = CARD_TYPE_BOMB;
			return type;
		}
		else if (card_stat.card1.size() == 1
				&& card_stat.card3.size() == 3)
		{
			face = card_stat.card3[2].face;
			type = CARD_TYPE_THREEWITHONE;
			return type;
		}
	}
	
	if (len == 5)
	{
		if (card_stat.card2.size() == 2
			&& card_stat.card3.size() == 3)
		{
			face = card_stat.card3[2].face;
			type = CARD_TYPE_THREEWITHTWO;
			return type;
		}
	}
	
	if (len == 6)
	{
		if (card_stat.card1.size() == 2
			&& card_stat.card4.size() == 4)
		{
			face = card_stat.card4[3].face;
			type = CARD_TYPE_FOURWITHONE;
			return type;
		}
	}
	
	if (len == 8)
	{
		if (card_stat.card2.size() == 4
			&& card_stat.card4.size() == 4)
		{
			face = card_stat.card4[3].face;
			type = CARD_TYPE_FOURWITHTWO;
			return type;
		}
	}
	
	if (card_stat.card1.size() == card_stat.line1.size()) {
		if (check_is_line(card_stat, 1)) {
            face = card_stat.card1[card_stat.card1.size() - 1].face;
            type = CARD_TYPE_ONELINE;
			return type;
		}
	}
	
	if (len == card_stat.card2.size()
		&& card_stat.card2.size() == card_stat.line2.size()) 
	{
		if (check_is_line(card_stat, 2)) 
		{
            face = card_stat.card2[card_stat.card2.size() - 1].face;
            type = CARD_TYPE_TWOLINE;
			return type;
		}
	}
	
	if (len < 6)
	{
		return type;
	}
	
	unsigned int left_card_len;
	if (card_stat.card3.size() == card_stat.line3.size()
		&& card_stat.card4.size() == 0 && card_stat.card3.size() != 0)
	{
		if (check_is_line(card_stat, 3))
		{

			left_card_len = card_stat.card1.size() + card_stat.card2.size();
			if (left_card_len == 0)
			{
                face = card_stat.card3[card_stat.line3.size() - 1].face;
                type = CARD_TYPE_THREELINE;
				return type;

			}
			else if (left_card_len * 3 == card_stat.card3.size())
			{
                face = card_stat.card3[card_stat.card3.size() - 1].face;
                type = CARD_TYPE_PLANEWITHONE;
				return type;
			}
			else if (card_stat.card1.size() == 0
				&& left_card_len * 3 == card_stat.card3.size() * 2)
			{
                face = card_stat.card3[card_stat.card3.size() - 1].face;
                type = CARD_TYPE_PLANEWITHWING;
				return type;
			}
		}
		
		if (check_arr_is_line(card_stat.card3, 3, 3, card_stat.card3.size())) 
		{
			left_card_len = card_stat.card1.size() + card_stat.card2.size() + 3;
			if (left_card_len * 3 == (card_stat.card3.size() - 3))
			{
                face = card_stat.card3[card_stat.card3.size() - 1].face;
                type = CARD_TYPE_PLANEWITHONE;
				return type;
			}
		}
	
		if (check_arr_is_line(card_stat.card3, 3, 0, card_stat.card3.size() - 3)) 
		{
			left_card_len = card_stat.card1.size() + card_stat.card2.size() + 3;
			if (left_card_len * 3 == (card_stat.card3.size() - 3))
			{
				face = card_stat.card3[card_stat.card3.size() - 1].face;
				type = CARD_TYPE_PLANEWITHONE;
				return type;
			}
		}
	}
	
	if (card_stat.card3.size() != 0 && check_arr_is_line(card_stat.card3, 3))
	{
		left_card_len = card_stat.card1.size() + card_stat.card2.size() + card_stat.card4.size();
		if (left_card_len * 3 == card_stat.card3.size()) 
		{
			face = card_stat.card3[card_stat.card3.size() - 1].face;
			type = CARD_TYPE_PLANEWITHONE;
			return type;
		}
		else if (card_stat.card1.size() == 0 
			&& left_card_len * 3 == card_stat.card3.size() * 2)
		{
			face = card_stat.card3[card_stat.card3.size() - 1].face;
			type = CARD_TYPE_PLANEWITHWING;
			return type;
		}
	}

	return type;
}

/*
 @brief 当需要分析的牌型中有鬼牌，则需要调用这个函数来分析当前牌型的值
 @retur 返回分析的牌型
*/
int CardAnalysis::ghost_analysis()
{
	if (len == 1)
	{
		face = card_stat.ghost_cards[0].face;
		type = CARD_TYPE_ONE;
		return type;
	}

	if (len == 2)
	{
        ghost_check_two();
        return type;
    }
	
	if (len == 3)
	{
        ghost_check_three();	
        return type;
	}
	
	if (len == 4)
	{
        ghost_check_bomb();
        ghost_check_threewithone();
        return type;
	}
	
	if (len == 5)
	{
        ghost_check_threewithtwo();
        ghost_check_line();
        return type;
	}

	if (len == 6)
	{
        ghost_check_fourwithone();
        ghost_check_line();
        ghost_check_doubleline();
        ghost_check_tripleline();
        return type;
	}
	
	if (len == 8)
	{
		ghost_check_plane();
		ghost_check_fourwithtwo();
        ghost_check_line();
        ghost_check_doubleline();
        return type;
	}

    ghost_check_plane();
    ghost_check_line();
    ghost_check_doubleline();
    ghost_check_tripleline();
	
    return type;
}

/*
 @brief 在有鬼牌的情况下，判断一对
 @retur 一对的面值(如果是0表示不是一对)
*/
int CardAnalysis::ghost_check_two()
{
    int max_face = 0;
    if (len != 2) return 0;

    if (ghost_num == 2)
    { //二张鬼牌的情况
        max_face = ghost_face;
    }

    else if (ghost_num == 1 && card_stat.card1.size() == 1)
    { //一张鬼牌的情况
        max_face = card_stat.card1[0].face;
        if (max_face > Card::Two) max_face = 0; //大王和小王不算
    }

    ghost_set_face_and_type(CARD_TYPE_TWO, max_face);
    return max_face;
}

/*
 @brief 在有鬼牌的情况下，判断三张不带的情况
 @retur 三张的面值(如果是0表示不是三张)
*/
int CardAnalysis::ghost_check_three()
{
    int max_face = 0;
    if (len != 3) return 0;

    if (ghost_num == 3)
    { //三张鬼牌的情况
        max_face = ghost_face;
    }

    else if (ghost_num == 2 && card_stat.card1.size() == 1)
    { //二张鬼牌的情况
        max_face = card_stat.card1[0].face;
    }

    else if (ghost_num == 1 && card_stat.card2.size() == 2)
    { //一张鬼牌的情况
        max_face = card_stat.card2[1].face;
    }

    ghost_set_face_and_type(CARD_TYPE_THREE, max_face);
    return max_face;
}

/*
 @brief 在有鬼牌的情况下，判断三带单牌
 @retur 三带的面值(如果是0表示不是三带单牌)
*/
int CardAnalysis::ghost_check_threewithone()
{
    int max_face = 0;
    if (len != 4) return 0;

    //三带1的情况
    // if (ghost_num == 3 && card_stat.card1.size() == 1)
    // {
    //     max_face = card_stat.card1[0].face;
    //     if (max_face < ghost_face)
    //     {
    //         max_face = ghost_face;
    //     }
    // }

    /*else */
    if (ghost_num == 2 && card_stat.card1.size() == 2)
    {
        max_face = card_stat.card1[1].face; //取card1中较大的牌做为最大的牌
    }

    // else if (ghost_num == 2 && card_stat.card2.size() == 2)
    // {
    //     max_face = card_stat.card2[1].face;
    // }

    else if (ghost_num == 1 && card_stat.card1.size() == 1 && card_stat.card2.size() == 2)
    {
        max_face = card_stat.card2[1].face;
    }
    ghost_set_face_and_type(CARD_TYPE_THREEWITHONE, max_face);
    return max_face;
}

/*
 @brief 在有鬼牌的情况下，判断三带对子
 @retur 三带的面值(如果是0表示不是三带对子)
*/
int CardAnalysis::ghost_check_threewithtwo()
{
    int max_face = 0;
    if (len != 5) return 0;
    if (ghost_num == 4 && card_stat.card1.size() == 1)
    {
        max_face = card_stat.card1[0].face;
        if (max_face < ghost_face)
        {
            max_face = ghost_face;
        }
        // return type;
    }

    else if (ghost_num == 3 && card_stat.card2.size() == 2)
    {
        max_face = card_stat.card2[1].face;
        if (max_face < ghost_face)
        {
            max_face = ghost_face;
        }
        // return type;
    }

    else if (ghost_num == 3 && card_stat.card1.size() == 2)
    {
        max_face = card_stat.card1[1].face; //取card1中较大的牌做为最大的牌
        // return type;
    }

    else if (ghost_num == 2 && card_stat.card3.size() == 3)
    {
        max_face = card_stat.card3[2].face;
        // return type;
    }

    else if (ghost_num == 2 && card_stat.card2.size() == 2 && card_stat.card1.size() == 1)
    {
        max_face = card_stat.card2[1].face;
        if (card_stat.card2[1] < card_stat.card1[0])
            max_face = card_stat.card1[0].face;
        // return type;
    }

    else if (ghost_num == 1 && card_stat.card3.size() == 3 && card_stat.card1.size() == 1)
    {
        max_face = card_stat.card3[2].face;
        // return type;
    }

    else if (ghost_num == 1 && card_stat.card2.size() == 4)
    {
        max_face = card_stat.card2[3].face;
        // return type;
    }
    ghost_set_face_and_type(CARD_TYPE_THREEWITHTWO, max_face);	
    return max_face;
}

/*
 @brief 在有鬼牌的情况下，判断顺子
 @retur 顺子中最大的牌值(如果是0表示不是顺子)
*/
int CardAnalysis::ghost_check_line()
{
    int max_face = 0;
    if (len < 5) return 0;

    //顺子，检测一条龙的时候，必须只有card1中有值
    if (card_stat.card2.size() != 0 || card_stat.card3.size() != 0 || card_stat.card4.size() != 0) return 0;


    if (has_bigger_than_ace()) return 0; //有2和王就不可能是顺子

    //计算不是鬼牌的牌值，总间距是多少。如6，7之间没有间距  6， 8 之间有1个间距
    //如果鬼牌的数量大于等于间距值，说明能组成顺子
    int gap = ghost_calc_gap();

    if (ghost_num < gap) return 0;

    //计算顺子中最大的牌值
    max_face = card_stat.line1[card_stat.line1.size() - 1].face + (ghost_num - gap);

    if (max_face > Card::Ace) max_face = Card::Ace;
    ghost_set_face_and_type(CARD_TYPE_ONELINE, max_face);
    return max_face;
}

/*
 @brief 在有鬼牌的情况下，判断连对
 @retur 连对中最大的牌值(如果是0表示不是连对)
*/
int CardAnalysis::ghost_check_doubleline()
{
    int max_face = 0;
    if (len < 6 || len % 2 != 0) return 0;

    //顺子，检测连对的时候，必须只有card1和card2中有值
    if (card_stat.card3.size() != 0 || card_stat.card4.size() != 0) return 0;

    const vector<Card> &refCard1 = card_stat.card1;
    const int card1Len = refCard1.size();

    if (has_bigger_than_ace()) return 0; //有2和王就不可能是顺子

    //先和单牌配对，使其能组成对子
    int left_num = ghost_num;
    if (card1Len > 0)
    {
        if (ghost_num < card1Len) return 0;
        left_num -=  card1Len;
    }

    if (left_num % 2 != 0) return 0;

    //计算总间距。如6，7之间没有间距  6， 8 之间有1个间距
    //如果鬼牌的数量大于等于间距值*2，说明能组成连对
    int gap = ghost_calc_gap();

    if (left_num < gap*2) return 0;

    //计算连对中最大的牌值
    max_face = card_stat.line1[card_stat.line1.size() - 1].face + (left_num - 2 * gap) / 2;
    if (max_face > Card::Ace) max_face = Card::Ace;
    ghost_set_face_and_type(CARD_TYPE_TWOLINE, max_face);
    return max_face;
}

/*
 @brief 在有鬼牌的情况下，判断三顺
 @retur 三顺中最大的牌值(如果是0表示不是三顺)
*/
int CardAnalysis::ghost_check_tripleline()
{
    int max_face = 0;
    if (len < 6 || len % 3 != 0) return 0;

    if (card_stat.card4.size() != 0) return 0;

    const vector<Card> &refCard1 = card_stat.card1;
    const int card1Len = refCard1.size();
    const vector<Card> &refCard2 = card_stat.card2;
    const int card2Len = refCard2.size();
    if (has_bigger_than_ace()) return 0; //有2和王就不可能是顺子

    //先和单牌配对，使其能组成三个
    int left_num = ghost_num;
    if (card1Len > 0)
    {
        if (ghost_num < card1Len * 2) return 0;
        left_num -= card1Len * 2;
    }

    //和对子配对，使其能组成三个
    if (card2Len > 0)
    {
        if (left_num < card2Len / 2) return 0;
        left_num -= card2Len / 2;
    }
    if (left_num % 3 != 0) return 0;


    //计算总间距。如6，7之间没有间距  6， 8 之间有1个间距
    //如果鬼牌的数量大于等于间距值*3，说明能组成三顺
    int gap = ghost_calc_gap();

    if (left_num < gap*3) return 0;

    //计算三顺中最大的牌值
    max_face = card_stat.line1[card_stat.line1.size() - 1].face + (left_num - 3 * gap) / 3;
    if (max_face > Card::Ace) max_face = Card::Ace;

    ghost_set_face_and_type(CARD_TYPE_THREELINE, max_face);
    return max_face;
}

/*
 @brief 在有鬼牌的情况下，判断炸弹（软炸，硬炸，纯癞子炸）
 @retur 炸弹面值(如果是0表示炸弹)
*/
int CardAnalysis::ghost_check_bomb()
{
    int max_face = 0;
    int max_type = 0;
    if (len != 4) return 0;

    if (ghost_num == 4)
    {
        max_face = ghost_face;
        max_type = CARD_TYPE_GHOSTBOMB; //纯癞子炸弹
    }

    else if (ghost_num == 3 && card_stat.card1.size() == 1)
    {
        max_face = card_stat.card1[0].face;
        max_type = CARD_TYPE_SOFTBOMB;
    }

    else if (ghost_num == 2 && card_stat.card2.size() == 2)
    {
        max_face = card_stat.card2[1].face;
        max_type = CARD_TYPE_SOFTBOMB;
    }

    else if (ghost_num == 1 && card_stat.card3.size() == 3)
    {
        max_face = card_stat.card3[2].face;
        max_type = CARD_TYPE_SOFTBOMB;
    }
    ghost_set_face_and_type(max_type, max_face);
    return max_face;
}

/*
 @brief 在有鬼牌的情况下，判断四带二个单牌
 @retur 四个的面值(如果是0表示不能组成四带二)
*/
int CardAnalysis::ghost_check_fourwithone()
{
    int max_face = 0;
    if (len != 6) return 0;
    if (ghost_num == 4 && card_stat.card1.size() == 2)
    {
        max_face = Card::Two;
    }

    else if (ghost_num == 4 && card_stat.card2.size() == 2)
    {
        max_face = card_stat.card2[1].face;
    }

    else if (ghost_num == 3 && card_stat.card1.size() == 3)
    {
        max_face = card_stat.card1[2].face;
    }

    else if (ghost_num == 3 && card_stat.card1.size() == 1 && card_stat.card2.size() == 2)
    {
        max_face = card_stat.card2[1].face;
    }

    else if (ghost_num == 3 && card_stat.card3.size() == 3)
    {
        max_face = card_stat.card3[2].face;
    }
    
    else if (ghost_num == 2 && card_stat.card1.size() == 2 && card_stat.card2.size() == 2)
    {
        max_face = card_stat.card2[1].face;
    }

    else if (ghost_num == 2 && card_stat.card2.size() == 4)
    {
        max_face = card_stat.card2[3].face;
    }

    else if (ghost_num == 2 && card_stat.card1.size() == 1 && card_stat.card3.size() == 3)
    {
        max_face = card_stat.card3[2].face;
    }

    else if (ghost_num == 2 && card_stat.card4.size() == 4)
    {
        max_face = card_stat.card4[3].face;
    }

    else if (ghost_num == 1 && card_stat.card1.size() == 2 && card_stat.card3.size() == 3)
    {
        max_face = card_stat.card3[2].face;
    }

    else if (ghost_num == 1 && card_stat.card1.size() == 1 && card_stat.card4.size() == 4)
    {
        max_face = card_stat.card4[3].face;
    }

    ghost_set_face_and_type(CARD_TYPE_FOURWITHONE, max_face);
    return max_face;
}

/*
 @brief 在有鬼牌的情况下，判断四带二个对子
 @retur 四个的面值(如果是0表示不能组成四带二个对子)
*/
int CardAnalysis::ghost_check_fourwithtwo()
{
    int max_face = 0;
    if (len != 8) return 0;
    if (ghost_num == 4 && card_stat.card1.size() == 2 && card_stat.card2.size() == 2)
    {
        max_face =  card_stat.card2[1].face;
        if (max_face < card_stat.card1[1].face)
        {
            max_face = card_stat.card1[1].face;
        }
    }

    else if (ghost_num == 4 && card_stat.card1.size() == 1 && card_stat.card3.size() == 3)
    {
        max_face = card_stat.card3[2].face;
    }

    else if (ghost_num == 4 && card_stat.card2.size() == 4)
    {
        max_face = Card::Two;
    }

    else if (ghost_num == 4 && card_stat.card4.size() == 4)
    {
        max_face = card_stat.card4[3].face;
    }

    else if (ghost_num == 3 && card_stat.card1.size() == 2 && card_stat.card3.size() == 3)
    {
        max_face = card_stat.card3[2].face;
    }

    else if (ghost_num == 3 && card_stat.card1.size() == 1 && card_stat.card2.size() == 4)
    {
        max_face = card_stat.card1[0].face;
    }

    else if (ghost_num == 3 && card_stat.card1.size() == 1 && card_stat.card4.size() == 4)
    {
        max_face = card_stat.card4[3].face;
    }

    else if (ghost_num == 3 && card_stat.card2.size() == 2 && card_stat.card3.size() == 3)
    {
        max_face = card_stat.card3[2].face;
    }
    
    else if (ghost_num == 2 && card_stat.card1.size() == 2 && card_stat.card4.size() == 4)
    {
        max_face = card_stat.card4[3].face;
    }

    else if (ghost_num == 1 && card_stat.card1.size() == 1 && 
            card_stat.card2.size() == 2 && card_stat.card4.size() == 4 )
    {
        max_face = card_stat.card4[3].face;
    }

    else if (ghost_num == 1 && card_stat.card2.size() == 4 && card_stat.card3.size() == 3)
    {
        max_face = card_stat.card3[2].face;
    }

    ghost_set_face_and_type(CARD_TYPE_FOURWITHTWO, max_face);
    return max_face;
}

/*
 @brief 在有鬼牌的情况下，判断飞机带羿或飞机带翅的情况
 @retur 飞机的面值(如果是0表示不能组成飞机带羿或飞机带翅)
*/
int CardAnalysis::ghost_check_plane()
{
    int max_face = 0;
    int plane_type = 0;
    if (len >= 8 && len % 4 == 0) plane_type = 0; //飞机带羿的情况
    else if (len >= 10 && len % 5 == 0) plane_type = 1; //飞机带翅的情况
    else return 0; 

    while(true)
    { //可能同时出现飞机带羿和翅，如果出20张牌时，就有可能出现这各情况,最外层循环确保涉及了这种情况
        //飞机中三张的数量，如666 777 89 需要2个三张
        const unsigned three_num = len / (4 + plane_type);

        const vector<Card> &refCard2 = card_stat.card2;
        const vector<Card> &refCard3 = card_stat.card3;
        const vector<Card> &refCard4 = card_stat.card4;
        const vector<Card> &refLine1 = card_stat.line1;

        vector<int> triple_card_face; //记录当前组成三个的面值
        int three_need = three_num;
        int left_num = ghost_num;

        bool i_change = true;
        for (int i = refLine1.size() - 1; i >= 0;
             --i, triple_card_face.clear()) //triple_card_face每次都要清空，防止影响下次的飞机牌判断。
        {
            if (refLine1[i].face > Card::Ace)
            {
                i_change = true;
                continue;
            }
            three_need = three_num;
            left_num = ghost_num;

            if (left_num >= 3 && i_change == true)
            { //在检测refLine1中i下标的牌最为最大的飞机牌时，鬼牌尝试当最大的飞机牌值
                i_change = false;
                int max_face = refLine1[i].face + 1;
                if (max_face < Card::Two)
                {
                    left_num -= 3;
                    triple_card_face.push_back(max_face);
                    if (ghost_has_plane(triple_card_face, three_need - 1, left_num))
                    {
                        if (plane_type == 0)
                            break;

                        //检查剩余的牌能否组成对子
                        if (!ghost_has_pane_with_two(triple_card_face, left_num))
                        {
                            ++i;
                            continue;
                        }
                        break;
                    }
                    ++i;
                    continue;
                }
                ++i;
                continue;
            }
            i_change = true; //下面的代码，都不会对i++，而每次循环都会--i。所以i的值必定会变。

            if (std::binary_search(refCard4.begin(), refCard4.end(), refLine1[i]))
            {
                --three_need;
            }
            else if (std::binary_search(refCard3.begin(), refCard3.end(), refLine1[i]))
            {
                --three_need;
            }
            else if (std::binary_search(refCard2.begin(), refCard2.end(), refLine1[i]))
            {
                --three_need;
                --left_num;
            }
            else
            {
                --three_need;
                left_num -= 2;
            }
            triple_card_face.push_back(refLine1[i].face);
            if (left_num < 0)
            {
                continue;
            }
            if (ghost_has_plane(triple_card_face, three_need, left_num))
            {
                if (plane_type == 0)
                    break;

                //检查剩余的牌能否组成对子
                if (ghost_has_pane_with_two(triple_card_face, left_num))
                {
                    break;
                }
            }
        }

        if (triple_card_face.size() > 0)
            max_face = triple_card_face[0] + left_num / 3;
        if (max_face > Card::Ace)
            max_face = Card::Ace;

        if (plane_type == 0)
            ghost_set_face_and_type(CARD_TYPE_PLANEWITHONE, max_face);
        else
            ghost_set_face_and_type(CARD_TYPE_PLANEWITHWING, max_face);
        if (plane_type == 0 && (len % 5 == 0))
        { //可能同时出现飞机带羿和翅，如果出20张牌时，就有可能出现这各情况
            plane_type = 1;
            max_face = 0;
            continue;
        }
        break;
    }
    return max_face;
}

/*
 @brief 在有鬼牌的情况下，给定一个飞机牌的最大牌值，检查和其它牌值能否组成飞机牌
 @param card_stat[in]
 @param triple_card_face[in] 已经算到飞机牌里面的牌值
 @param three_need[in] 还缺少的飞机牌的面值个数
 @param left_num[in] 剩余的鬼牌数
 @retur true能组成飞机牌， false不能
*/
bool CardAnalysis::ghost_has_plane(vector<int>& triple_card_face, int three_need, int& left_num) const 
{
    const vector<Card> &refCard1 = card_stat.card1;
    const vector<Card> &refCard2 = card_stat.card2;
    const vector<Card> &refCard3 = card_stat.card3;

    int left_card_face = triple_card_face.back() - 1;
    if (std::binary_search(refCard3.begin(), refCard3.end(), Card(left_card_face)))
    { //找到三张
        --three_need;
    }
    else if (std::binary_search(refCard2.begin(), refCard2.end(), Card(left_card_face)))
    { //找到二张
        if (--left_num >= 0) --three_need;
        else return false;
    }
    else if (std::binary_search(refCard1.begin(), refCard1.end(), Card(left_card_face)))
    { //找到一张
        left_num -= 2;
        if (left_num >= 0) --three_need;
        else return false;
    }
    else
    { //没有找到
        left_num -= 3;
        if (left_num >= 0) --three_need;
        else return false;
    }

    triple_card_face.push_back(left_card_face);
    if (three_need > 0) return ghost_has_plane(triple_card_face, three_need, left_num);
    return true;
}

/*
 @brief 在有鬼牌的情况下，已经找出飞机牌，查看剩下的牌能否组成对子
 @param card_stat[in]
 @param triple_card_face[in] 已经算到飞机牌里面的牌值
 @param left_num[in] 剩余的鬼牌数
 @retur true能组成对子， false不能
*/
bool CardAnalysis::ghost_has_pane_with_two(vector<int>& triple_card_face, int &left_num) const
{
    vector<int> left_cards_face;
    int need_pair = triple_card_face.size(); //有多少个三张就应该有多少个对子
    const vector<Card> &refLine1 = card_stat.line1;
    for (unsigned i = 0; i < refLine1.size(); ++i)
    {
        if (std::find(triple_card_face.begin(), triple_card_face.end(), refLine1[i].face) == triple_card_face.end())
        {
            left_cards_face.push_back(refLine1[i].face);
        }
    }

    const vector<Card> &refCard4 = card_stat.card4; 
    for (unsigned i = 0; i < refCard4.size(); i += 4)
    { //当有4张的牌在飞机牌中，必定有一张是剩余的牌
        if (std::find(triple_card_face.begin(), triple_card_face.end(), refCard4[i].face) != triple_card_face.end())
        { 
            //需要消耗一张鬼牌才能组成对子
            if (--left_num >= 0)
            {
                if(--need_pair <= 0) return true;
            }
            else return false;
        }
    }

    const vector<Card> &refCard1 = card_stat.card1; 
    const vector<Card> &refCard2 = card_stat.card2; 
    const vector<Card> &refCard3 = card_stat.card3; 
    for(unsigned i = 0; i<left_cards_face.size(); ++i)
    {
        if (std::binary_search(refCard1.begin(), refCard1.end(), Card(left_cards_face[i])))
        { 
            //需要消耗一张鬼牌才能组成对子
            if (--left_num >= 0)
            {
                if(--need_pair <= 0) return true;
            }
            else return false;
        }
        if (std::binary_search(refCard2.begin(), refCard2.end(), Card(left_cards_face[i])))
        { 
            if (--need_pair <= 0) return true;
        }
        if (std::binary_search(refCard3.begin(), refCard3.end(), Card(left_cards_face[i])))
        { 
            //需要消耗一张鬼牌才组成2个对子
            if (--left_num >= 0)
            {
                need_pair -= 2;
                if(need_pair <= 0) return true;
            }
            else return false;
        }
    }

    return true;
}

/*
 @brief 在有鬼牌的情况下，设置分析出的牌型
 @param set_type[in] 要设置的牌型
 @param set_face[in] 要设置的面值
*/
void CardAnalysis::ghost_set_face_and_type(int set_type, int set_face)
{
    if (set_face <= 0 || set_type <= 0) return ;

    if (type > CARD_TYPE_ERROR) 
    {//已经至少分析出了一个牌型，后面分析出的牌型，加到back_up_type中
        back_up_type.push_back(set_type);
        back_up_face.push_back(set_face);
    }
    else
    {
        type = set_type;
        face = set_face;
    }
}

/*
 @brief 判断连张中是否有2或王, 如果2是鬼牌，则有也没事
 @retur true表示有， false表示没有
*/
bool CardAnalysis::has_bigger_than_ace() const
{
    const int cardLen = card_stat.line1.size();
    if (cardLen > 0)
    {
        if (card_stat.line1[cardLen - 1].face > Card::Ace)
        {
            return true;
        }
    }
    return false;
}

/*
 @brief 判断连张不够的情况下，缺多少连张的牌值
 @retur 缺连张的牌值的个数
*/
int CardAnalysis::ghost_calc_gap() const
{
    //计算不是鬼牌的牌值，总间距是多少。如6，7之间没有间距  6， 8 之间有1个间距
    //如果鬼牌的数量大于等于间距值，说明能组成顺子
    int gap = 0;
    for (unsigned int i = 0, j = 1; j < card_stat.line1.size(); ++i, ++j)
    {
        gap += card_stat.line1[j].face - card_stat.line1[i].face - 1;
    }
    return gap;
}
/**********************分析无癞子时的用的函数**********************/
bool CardAnalysis::check_is_line(const CardStatistics &card_stat, int line_type)
{
	if (line_type == 1)
	{
		return check_arr_is_line(card_stat.line1, line_type);	
	}
	else if (line_type == 2)
	{
		return check_arr_is_line(card_stat.line2, line_type);
	}
	else if (line_type == 3)
	{
		return check_arr_is_line(card_stat.line3, line_type);	
	}
	
	return false;
}

bool CardAnalysis::check_arr_is_line(const std::vector<Card> &line, int line_type)
{
	/*
	int len = 1;
	Card card = line[0];
	for (unsigned int i = line_type; i < line.size(); i += line_type)
	{
		if ((card.face + 1) == line[i].face && line[i].face != 15) { // 2 is not straight (Line)
			len++;
			card = line[i];
		}
		else
		{
			return false;
		}
	}
	
	if (line_type == 1 && len > 4) // single straight
		return true;
	else if (line_type == 2 && len > 2) // double straight
		return true;
	else if (line_type == 3 && len > 1) // three straight
		return true;
	
	return false;
	*/
	return check_arr_is_line(line, line_type, 0, line.size());
}

bool CardAnalysis::check_arr_is_line(const std::vector<Card> &line, int line_type, unsigned int begin, unsigned int end)
{
	int len = 1;
	Card card = line[begin];
	for (unsigned int i = (line_type + begin); i < end; i += line_type)
	{
		if ((card.face + 1) == line[i].face && line[i].face != 15) { // 2 is not straight (Line)
			len++;
			card = line[i];
		}
		else
		{
			return false;
		}
	}
	
	if (line_type == 1 && len > 4) // single straight
		return true;
	else if (line_type == 2 && len > 2) // double straight
		return true;
	else if (line_type == 3 && len > 1) // three straight
		return true;
	
	return false;
}

bool CardAnalysis::compare(CardAnalysis &card_analysis)
{
	// printf("compare type[%s] len[%d] face[%d] vs type[%s] len[%d] face[%d]\n",
	// 			card_type_str[type], len, face, card_type_str[card_analysis.type], card_analysis.len, card_analysis.face);
    // vector<Card> myCur;
    // myCur = card_stat.card4;
    // cur = stat.card4; //to do 为什么这里不能用back_insert?
    //    std::copy(card_stat.card3.begin(), card_stat.card3.end(), back_insert(myCur));
    // std::copy(stat.card2.begin(), stat.card2.end(), back_insert(cur));
    // std::copy(stat.card1.begin(), stat.card1.end(), back_insert(cur));
    if (card_analysis.type == CARD_TYPE_ERROR)
	{
		return false;
	}
	
	if (type == CARD_TYPE_ROCKET)
	{
		return true;
	}
	
	if (card_analysis.type == CARD_TYPE_ROCKET)
	{
		return false;
	}
	
	if (type == card_analysis.type)
	{
		if (len == card_analysis.len
			&& face > card_analysis.face)
		{
			return true;
		}
	}
	else
	{
		if (type == CARD_TYPE_BOMB)
		{
			return true;	
		}
	}
	return false;
}

void CardAnalysis::debug()
{
	cout << "type: " << card_type_str[type] << " face: " << face << 
         "   ghost_face: " << ghost_face << " ghost_num: "<< ghost_num << endl;
    for (unsigned int i = 0; i < back_up_face.size(); ++i)
    {
        cout << "-back_up_type: " << card_type_str[back_up_type[i]] << 
                " back_up_face: " << back_up_face[i] << endl;
    }
}

void CardAnalysis::format(const CardStatistics &stat, vector<int> &cur)
{
	int len;
	cur.clear();
	
	len = stat.card4.size() - 1;
	for (int i = len; i >= 0; i--)
	{
		cur.push_back(stat.card4[i].value);
	}

	len = stat.card3.size() - 1;
	for (int i = len; i >= 0; i--)
	{
		cur.push_back(stat.card3[i].value);
	}

	len = stat.card2.size() - 1;
	for (int i = len; i >= 0; i--)
	{
		cur.push_back(stat.card2[i].value);
	}

	len = stat.card1.size() - 1;
	for (int i = len; i >= 0; i--)
	{
		cur.push_back(stat.card1[i].value);
	}
}

void CardAnalysis::format(const CardStatistics &stat, vector<Card> &cur)
{
	int len;
	cur.clear();

	len = stat.card4.size() - 1;
	for (int i = len; i >= 0; i--)
	{
		Card card(stat.card4[i].value);
		cur.push_back(card);
	}

	len = stat.card3.size() - 1;
	for (int i = len; i >= 0; i--)
	{
		Card card(stat.card3[i].value);
		cur.push_back(card);
	}

	len = stat.card2.size() - 1;
	for (int i = len; i >= 0; i--)
	{
		Card card(stat.card2[i].value);
		cur.push_back(card);
	}

	len = stat.card1.size() - 1;
	for (int i = len; i >= 0; i--)
	{
		Card card(stat.card1[i].value);
		cur.push_back(card);
	}
}

int CardAnalysis::isGreater(vector<int> &last, vector<int> &cur, int *card_type)
{
    if (last.size() == 0)
    {
        return -1;
    }
    
    if (cur.size() == 0)
    {
        return -2;
    }
    
	vector<Card> cards0;
	for (unsigned int i = 0; i < last.size(); i++)
	{
		Card card(last[i]);
		cards0.push_back(card);
	}
	CardStatistics card_stat0;
	card_stat0.statistics(cards0);
	CardAnalysis card_ana0;
	card_ana0.analysis(card_stat0);
    if (card_ana0.type == 0)
    {
        return -1;
    }
	
    vector<Card> cards1;
	for (unsigned int i = 0; i < cur.size(); i++)
	{
		Card card(cur[i]);
		cards1.push_back(card);
	}
	CardStatistics card_stat1;
	card_stat1.statistics(cards1);
	CardAnalysis card_ana1;
	card_ana1.analysis(card_stat1);
    if (card_ana1.type == 0)
    {
        return -2;
    }
    
	*card_type = card_ana1.type;
	bool res = card_ana1.compare(card_ana0);
    if (res)
    {
    	CardAnalysis::format(card_stat1, cur);
        return 1;
    }
    else
    {
        return 0;
    }
}

int CardAnalysis::isGreater(vector<Card> &last, vector<Card> &cur, int *card_type)
{
    if (last.size() == 0)
    {
        return -1;
    }
    
    if (cur.size() == 0)
    {
        return -2;
    }
	CardStatistics card_stat0;
	card_stat0.statistics(last);
	CardAnalysis card_ana0;
	card_ana0.analysis(card_stat0);
    if (card_ana0.type == 0)
    {
        return -1;
    }
	
	CardStatistics card_stat1;
	card_stat1.statistics(cur);
	CardAnalysis card_ana1;
	card_ana1.analysis(card_stat1);
    if (card_ana1.type == 0)
    {
        return -2;
    }
    
	*card_type = card_ana1.type;
	bool res = card_ana1.compare(card_ana0);
    if (res)
    {
    	CardAnalysis::format(card_stat1, cur);
        return 1;
    }
    else
    {
        return 0;
    }
}

int CardAnalysis::get_card_type(vector<int> &input)
{
    if (input.size() == 0)
    {
        return 0;
    }
    
	vector<Card> cards;
	for (unsigned int i = 0; i < input.size(); i++)
	{
		Card card(input[i]);
		cards.push_back(card);
	}
	CardStatistics card_stat;
	card_stat.statistics(cards);
	CardAnalysis card_ana;
	card_ana.analysis(card_stat);
	CardAnalysis::format(card_stat, input);
    
    return card_ana.type;
}

int CardAnalysis::get_card_type(vector<Card> &input)
{
	CardStatistics card_stat;
	card_stat.statistics(input);
	CardAnalysis card_ana;
	card_ana.analysis(card_stat);
    CardAnalysis::format(card_stat, input);
    
    return card_ana.type;
}

void CardAnalysis::test(int input[], int len)
{
	vector<Card> cards;
	for (int i = 0; i < len ; i++)
	{
		Card card(input[i]);
		cards.push_back(card);	
	}
	CardStatistics card_stat;
	card_stat.statistics(cards);
	CardAnalysis card_ana;
	card_ana.analysis(card_stat);
	Card::dump_cards(cards);
	card_ana.debug();
}

void CardAnalysis::test(int input0[], int len0, int input1[], int len1)
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
	CardAnalysis card_ana1;
	card_ana1.analysis(card_stat1);
	Card::dump_cards(cards1);
	card_ana1.debug();
	
	bool res = card_ana0.compare(card_ana1);
	cout << "res: " << res << endl;
}
//end 无癞子函数