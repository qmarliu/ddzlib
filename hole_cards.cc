#include "card.h"
#include "hole_cards.h"
#include "community_cards.h"
#include "card_statistics.h"
#include "card_analysis.h"
#include "card_find.h"


HoleCards::HoleCards()
{
    ghost_face = 0;
}

void HoleCards::add_card(Card card)
{
	cards[card.value] = card;
}

void HoleCards::copy_cards(std::vector<Card> *v)
{
	std::map<int, Card>::iterator it;
	for (it = cards.begin(); it != cards.end(); it++)
	{
		v->push_back(it->second);
	}
}

void HoleCards::copy_cards(std::vector<int> *v)
{
	std::map<int, Card>::iterator it;
	for (it = cards.begin(); it != cards.end(); it++)
	{
		Card &card = it->second;
		v->push_back(card.value);
	}
}

/*
 @brief 获取手上最小的牌
 @param includeGhostFace：为false时，表示不找鬼牌
                          为true时，表示找鬼牌 
*/
int HoleCards::get_one_little_card(bool includeGhostFace)
{
	std::vector<Card> v;
	
	std::map<int, Card>::iterator it;
	for (it = cards.begin(); it != cards.end(); it++)
	{
		v.push_back(it->second);
	}
	
	Card::sort_by_ascending(v);
	
    Card card;
    if (includeGhostFace == false) 
    {
        for (unsigned int i = 0; i < v.size(); ++i)
        {
            if (v[i].face != ghost_face) //除去鬼牌找最大的牌
            {
                card = v[i];
                return card.value;
            }
        }
    }

    if (card.value == 0 && v.size() >0) 
    {
        card = v.front();
    }
	
	// cards.erase(card.value);
	
	return card.value;
}

/*
 @brief 玩家从手牌中选则一个合理的牌打出。
 @param v[out] 返回应该打出去的牌
 @param landlord_seat[in] 地主坐位号
 @param lord_type[in] 斗地主类型
 @param my_seat[in] 玩家的坐位号
*/
int HoleCards::robot(std::vector<int> &v, int landlord_seat, int lord_type, int my_seat)
{
	std::vector<int> cards_int;
	copy_cards(&cards_int);
	
	// to find straight
	CardFind::find_straight(cards_int, v);
	if (v.size() > 0)
		return 0;
	
	
	vector<Card> cards_obj;
	copy_cards(&cards_obj);
	CardStatistics card_stat;
	card_stat.statistics(cards_obj);
	
	// to find three or with one or with two
	if (card_stat.card3.size() > 0)
	{
		v.push_back(card_stat.card3[0].value);
		v.push_back(card_stat.card3[1].value);
		v.push_back(card_stat.card3[2].value);
		
		if (card_stat.card1.size() > 0)
		{
			if (card_stat.card1[0].face != 16 && card_stat.card1[0].face != 17)
            {
				v.push_back(card_stat.card1[0].value);
            	return 0;
            }
		}
		
		if (card_stat.card2.size() > 0)
		{
			v.push_back(card_stat.card2[0].value);
			v.push_back(card_stat.card2[1].value);
			return 0;
		}
		return 0;
	}
	
	// is rocket
	if (card_stat.card1.size() == 2)
	{
		if (card_stat.card1[0].face == 16 &&
			card_stat.card1[1].face == 17)
		v.push_back(card_stat.card1[0].value);
		v.push_back(card_stat.card1[1].value);
		return 0;
	}

    bool has_bao_dan = false; //记录非队友是否有报单的
    bool has_bao_shuang = false; //记录非队友是否有报双
    get_dan_shuang(has_bao_dan, has_bao_shuang, landlord_seat, lord_type, my_seat);

	//the smallest single card
	if (card_stat.card1.size() > 0)
	{
        if (card_stat.card2.size() > 0 && //没有人报双的情况下，单和2张牌全部大于J，对子优先单牌
                card_stat.card1[0].face >= 11 && card_stat.card2[0].face >=11 && has_bao_shuang == false)
        {
            v.push_back(card_stat.card2[0].value);
            v.push_back(card_stat.card2[1].value);
            return 0;
        }
        else
        {
            if (has_bao_dan == false ) 
            {
                v.push_back(card_stat.card1[0].value);
                return 0;
            }
        }
		//remove(v);
	}

    //the samllest pair of card
	if (card_stat.card2.size() > 0)
	{
        if (has_bao_shuang == false)
        {
            v.push_back(card_stat.card2[0].value);
            v.push_back(card_stat.card2[1].value);
            return 0;
        }

        if (has_bao_shuang && card_stat.card4.size() == 0) //非队友报双，手上只有对子,拆对子
        {
            v.push_back(card_stat.card2[0].value);
            return 0;
        }
		//remove(v);
	}

    
    if (has_bao_dan) //非队友报单，手上只有单牌,出最大的单牌
    {
        unsigned card1Size = card_stat.card1.size();
        if ( card1Size > 0)
        {
            v.push_back(card_stat.card1[card1Size - 1].value);
            return 0;
        }
    }
	
	if (card_stat.card4.size() > 0)
	{
		v.push_back(card_stat.card4[0].value);
		v.push_back(card_stat.card4[1].value);
		v.push_back(card_stat.card4[2].value);
		v.push_back(card_stat.card4[3].value);
		return 0;
	}
    
	return -1;
}

/*
 @brief 检测敌对家是否有单双
 @param has_bao_ban[out] 返回是否有人报单
 @param has_bao_shuang[out] 返回是否有人报双
 @param landlord_seat[in] 地主坐位号
 @param lord_type[in] 斗地主类型
 @param my_seat[in] 玩家的坐位号

 has_bigger_than_ace
*/
void HoleCards::get_dan_shuang(bool &has_bao_dan, bool &has_bao_shuang, int landlord_seat, int lord_type, int my_seat)
{
    has_bao_dan = false; //记录非队友是否有报单的
    has_bao_shuang = false; //记录非队友是否有报双

    if (landlord_seat == my_seat || lord_type == 2)
    { //自己为地主和各自为战，检查其它玩家是否有报双和报单的情况
        for (unsigned i = 0 ; i < other_player_info.size(); ++i)
        {
            if (other_player_info[i].cards_left_num == 1)
            {
                has_bao_dan = true;
            }

            if (other_player_info[i].cards_left_num == 2)
            {
                has_bao_shuang = true;
            }
        }
    }
    else 
    { //自己不是地主，且不是各自为战，看地主有没有报单或报双
        for (unsigned i = 0 ; i < other_player_info.size(); ++i)
        {
            if (landlord_seat != other_player_info[i].seatid)
                continue;

            if (other_player_info[i].cards_left_num == 1)
            {
                has_bao_dan = true;
            }

            if (other_player_info[i].cards_left_num == 2)
            {
                has_bao_shuang = true;
            }
        }
    }
}

void HoleCards::remove(std::vector<Card> &v)
{
	for (unsigned int i = 0; i < v.size(); i++)
	{
		cards.erase(v[i].value);
	}
}

void HoleCards::remove(std::vector<int> &v)
{
	for (unsigned int i = 0; i < v.size(); i++)
	{
		cards.erase(v[i]);
	}
}

int HoleCards::size()
{
	return (int)(cards.size());
}

void HoleCards::debug()
{
	Card::dump_cards(cards);
}

/*
 @brief 设置other_player_info中某个玩家剩余的牌数
 @param seatid[in] 设置的玩家坐位号
 @param left_num[in] 该玩家剩余的牌数

 @ref 玩家出牌后，服务器会广播出牌玩家剩余牌的数量，机器人调用这个函数记录这个出牌玩家的牌数
*/
void HoleCards::set_left_num_of(int seatid, int left_num)
{
    for (unsigned int i = 0; i< other_player_info.size(); ++i)
    {
        if (other_player_info[i].seatid == seatid)
        {
            other_player_info[i].cards_left_num = left_num;
            return;
        }
    }
}

/*
 @brief 分析当前玩家手中的牌,并找出合适的牌型组成存入out_cards,使能合理的管上对家出的last_cards的牌
 @param last_cards[in] 返回是否有人报单
 @param out_cards[out] 返回是否有人报双
 @param landlord_seat[in] 地主坐位号
 @param lord_type[in] 斗地主类型
 @param my_seat[in] 玩家的坐位号
 @param pre_play_seat[in] 上个出牌的玩家
*/
void HoleCards::get_play_cards(vector<int> &last_cards, vector<int> &out_cards, int landlord_seat, int lord_type, int my_seat, int pre_play_seat)
{
	std::vector<int> cur_cards;
	copy_cards(&cur_cards);
    out_cards.clear();

   	CardFind *cf = new CardFind();
    // cf->set_robot_flag(true); //机器人的话要设置这一选项

    int ret = cf->tip(last_cards, cur_cards);
    if (ret != 0)
    {
        return;
    }
    if (cf->results.size() == 0)
    {
        return ;
    }

    //计算last_cards的类型
	vector<Card> cards0;
	for (unsigned int i = 0; i < last_cards.size(); i++)
	{
		Card card(last_cards[i]);
		cards0.push_back(card);
	}
	CardStatistics card_stat0;
	card_stat0.statistics(cards0);
	CardAnalysis card_ana0;
	card_ana0.analysis(card_stat0);
    int last_cards_type = card_ana0.type;
    bool has_bao_dan = false;
    bool has_bao_shuang = false;
    get_dan_shuang(has_bao_dan, has_bao_shuang, landlord_seat, lord_type, my_seat);
    if (has_bao_dan && last_cards_type == CARD_TYPE_ONE)
    { //敌家报单，出最大的单牌
        out_cards.push_back(get_one_max_card(true));
        return ;
    }

    if (has_bao_shuang && last_cards_type == CARD_TYPE_TWO)
    { //敌家报双,打炸弹，没炸弹，打最大的对子（拆3张等牌型）
        vector<Card> cards_obj;
        copy_cards(&cards_obj);
        CardStatistics card_stat;
        card_stat.statistics(cards_obj);

        for (unsigned int i = 0; i < card_stat.card4.size(); i += 4)
        { //炸弹
            out_cards.push_back(card_stat.card4[i].value);
            out_cards.push_back(card_stat.card4[i + 1].value);
            out_cards.push_back(card_stat.card4[i + 2].value);
            out_cards.push_back(card_stat.card4[i + 3].value);
            return;
        }

        int line2Index = card_stat.line2.size() - 1;
        for (int i = line2Index; i >= 0; i -= 2)
        { //最大的对子
            out_cards.push_back(card_stat.line2[i].value);
            out_cards.push_back(card_stat.line2[i - 1].value);
            return ;
        }
    }

    if (lord_type == 0 || lord_type == 1)
    { 
        if (landlord_seat != my_seat && pre_play_seat != landlord_seat)
        {//友军，我们选择不打他
            return;
        }
    }

    for (unsigned int cnt = 0; cnt < cf->results.size(); cnt++)
    {
        for (unsigned int i = 0; i < cf->results[cnt].size(); i++)
        {
            if (last_cards_type == CARD_TYPE_ONE)
            { //单牌不拆J以下的对子
                if (std::binary_search(cf->from_two.begin(), cf->from_two.end(), cnt) &&
                    cf->results[cnt][i].face < 11)
                {
                    break;
                }
                // if (cf->results_type[cnt] == CARD_TYPE_TWO && 
                //         cf->results[cnt][i].face < 11)
                // {
                //     break;
                // }
            }
            if (last_cards_type == CARD_TYPE_TWO)
            { //对子不拆A以下的三张
                if (std::binary_search(cf->from_three.begin(), cf->from_three.end(), cnt) &&
                    cf->results[cnt][i].face < 14)
                {
                    break;
                }
                // if (cf->results_type[cnt] == CARD_TYPE_THREE && 
                //         cf->results[cnt][i].face < 14)
                // {
                //     break;
                // }
            }

            for (unsigned int j = 0; j < cf->results[cnt].size(); j++)
            {
                Card card = cf->results[cnt][j];
                out_cards.push_back(card.value);
            }
            return; 
        }
    }
}

/*
 @brief 获取手上最大的牌
 @param includeGhostFace：为false时，表示不找鬼牌
                          为true时，表示找鬼牌 
*/
int HoleCards::get_one_max_card(bool includeGhostFace)
{
	std::vector<Card> v;
	
	std::map<int, Card>::iterator it;
	for (it = cards.begin(); it != cards.end(); it++)
	{
		v.push_back(it->second);
	}
	
	Card::sort_by_descending(v);
	
	Card card;
    if (includeGhostFace == false) 
    {
        for (unsigned int i = 0; i < v.size(); ++i)
        {
            if (v[i].face != ghost_face) //除去鬼牌找最大的牌
            {
                card = v[i];
                return card.value;
            }
        }
    }

    if (card.value == 0 && v.size() >0) 
    {
        card = v.front();
    }
	
	// cards.erase(card.value);
	
	return card.value;
}

//
/*
 @brief 记录其它玩家的一些信息到other_player_info中，用于判断之后如何的出牌
 @param seatVec[in] 需要记录玩家的坐位号
 @param cardLeft[in] 玩家的剩余牌数

 @ref 用于游戏一开始，记录其它玩家的初始信息，目前只记录了坐位号和对应的剩余牌数
*/
void HoleCards::set_other_player_info(vector<int> &seatVec, vector<int> &cardLeft)
{
    other_player_info.clear();
    for (unsigned i = 0; i < seatVec.size(); ++i)
    { 
        SeatInfo si;
        si.seatid = seatVec[i];
        si.cards_left_num = cardLeft[i];
        other_player_info.push_back(si);
    }
}