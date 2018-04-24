#include "card.h"
#include "hole_cards.h"
#include "community_cards.h"
#include "card_statistics.h"
#include "card_analysis.h"
#include "card_find.h"

HoleCards::HoleCards()
{
    ghost_face = 0;
    landlord_seat = -1;
    table_type = -1;
    seatid = -1;
    pre_play_seat = -1;
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

/**************************机器人相关方法*******************************************/
/****************拆牌的相关方法*************/
/***
 * 拆分手上的牌
    1.	先确定火箭：判断是否有大小王。         
    2.	再确定炸弹：判明是否有四头。          
    3.	再确定三顺：，应该使三顺的数量尽可能大。即如果有444555666，则将其合成一个三顺，而不是分成444555一个三顺和666一个三条。         
    4.	再确定单顺：判断单顺时必须去除四个2以外的所有炸弹。首先判断是否存在除了三条牌（这里的三条是指所有的三条）以外的连牌，如果有，则将其提取出来。其 次，将剩余的牌与每一个三条（不包含三顺）进行试组合，如果能够重新组成单顺和对子，则将原有的三条取消，重新组合成连牌和单顺（例子4566678重新 组成45678和66）。最后，将已知的连牌、三条（不包含三顺）和剩下的牌张再试组合，将所有如45678（已知的连牌）999（三条）10J（单 牌），重新组合成45678910J和99。通过以上的方法，就能将连牌和三条很好地重新组合。   
    5.	再确定三条：找不到三顺，则在除了炸弹以外的牌中判断是否包含三条。     
    6.	再确定双顺：首先，如果两单顺牌完全重合，则将其重新组合成双顺。其次，在除炸弹、三顺、三条、单顺以外的牌中检测是否包含双顺。如果有，将其提取出来。         
    7.	再确定对子：在炸弹、三顺、三条、连牌、双顺以外的牌中检测是否存在对子，如果存在将其提取出来。          
    8.	再确定单牌：除了炸弹、三顺、三条、连牌、双顺、对子以外的所有牌张都是单牌。
   ***/
void HoleCards::split_cards()
{
    splitedCards.clear();
    splitedTypes.clear();
    vector<Card> splits;
    copy_cards(&splits);
    // cout << "hole_cards.";
    // Card::dump_cards(splits, "splits");
    split_rocket(splits);
    split_bomb(splits);
    split_threeline(splits);
    split_oneline(splits);
    split_three(splits);
    split_twoline(splits);
    split_two(splits);
    split_one(splits);
}

/****拆牌的工具方法****/
void HoleCards::split_rocket(vector<Card> &splits)
{
   if (splits.size() < 2) return; 

   //这里不用card_statistics是怕影响效率
   vector<Card>::iterator ite;
   vector<Card>::iterator ite2;
   ite = find(splits.begin(), splits.end(), Card(14));
   ite2 = find(splits.begin(), splits.end(), Card(15));

   if (ite != splits.end() &&
       ite2 != splits.end())
   {
       vector<Card> cards;
       cards.push_back(*ite);
       cards.push_back(*ite2);
       ite2 = splits.erase(ite);
       splits.erase(ite2);
       splitedCards.push_back(cards);
       splitedTypes.push_back(CARD_TYPE_ROCKET);
    //    cout << "split_rocket.";
    //    Card::dump_cards(splits, "splits");
   }
}

void HoleCards::split_bomb(vector<Card> &splits)
{
   if (splits.size() < 4) return; 
    CardFind cf;
    vector<Card> compareCard;
    compareCard.resize(1);
    cf.find(compareCard, splits, CARD_TYPE_ONE, Card::Big, ghost_face);
    vector<Card>::iterator ite;
    int gret = cf.has_bomb_type_of(CARD_TYPE_GHOSTBOMB);
    if (gret >= 0)
    {
        vector<Card> cards;
        for( unsigned i = 0; i<cf.results[gret].size(); ++i )
        {
            ite = find(splits.begin(), splits.end(), cf.results[gret][i]);
            cards.push_back(*ite);
            splits.erase(ite);
        }
        splitedCards.push_back(cards);
        splitedTypes.push_back(CARD_TYPE_GHOSTBOMB);
        // cout << "split_ghostbomb.";
        // Card::dump_cards(splits, "splits");
    }

    int ret = cf.has_bomb_type_of(CARD_TYPE_BOMB);
    while (ret >= 0)
    {
        vector<Card> cards;
        for( unsigned i = 0; i<cf.results[ret].size(); ++i )
        {
            ite = find(splits.begin(), splits.end(), cf.results[ret][i]);
            cards.push_back(*ite);
            splits.erase(ite);
        }
        splitedCards.push_back(cards);
        splitedTypes.push_back(CARD_TYPE_BOMB);
        // cout << "split_bomb.";
        // Card::dump_cards(splits, "splits");
        if (splits.size() < 4) return; 
        cf.find(compareCard, splits, CARD_TYPE_ONE, Card::Big, ghost_face);
        ret =cf.has_bomb_type_of(CARD_TYPE_BOMB); 
    }

    ret = cf.has_bomb_type_of(CARD_TYPE_SOFTBOMB);
    while (ret >= 0 && gret == -1)
    {
        vector<Card> cards;
        for( unsigned i = 0; i<cf.results[ret].size(); ++i )
        {
            ite = find(splits.begin(), splits.end(), cf.results[ret][i]);
            cards.push_back(*ite);
            splits.erase(ite);
        }
        splitedCards.push_back(cards);
        splitedTypes.push_back(CARD_TYPE_SOFTBOMB);
        // cout << "split_softbomb.";
        // Card::dump_cards(splits, "splits");
        if (splits.size() < 4) return; 
        cf.find(compareCard, splits, CARD_TYPE_ONE, Card::Big, ghost_face);
        ret =cf.has_bomb_type_of(CARD_TYPE_SOFTBOMB); 
    }
}

void HoleCards::split_threeline(vector<Card> &splits)
{
    while (find_max_len_threeline(splits))
    {
        // cout << "split_threeline."; 
        // Card::dump_cards(splits, "splits");
    }
}

void HoleCards::split_three(vector<Card> &splits)
{
    while (find_splite_three(splits))
    {
        // cout << "split_three.";
        // Card::dump_cards(splits, "splits");
        if (splits.size() < 3) return; 
    }
}

void HoleCards::split_oneline(vector<Card> &splits)
{
    int lineCnt = 0;
    while (find_max_len_oneline(splits))
    {
        ++lineCnt;
        // cout << "split_oneline.";
        // Card::dump_cards(splits, "splits");
    }

    //合并相同的单顺
    int index = splitedCards.size() - lineCnt;
    for (unsigned nindex = index + 1; nindex < splitedCards.size(); ++index, ++nindex)
    {
        if (splitedCards[index] == splitedCards[nindex])
        {
            std::copy(splitedCards[nindex].begin(), splitedCards[nindex].end(), back_inserter(splitedCards[index]));
            std::sort(splitedCards[index].begin(), splitedCards[index].end());
            splitedTypes[index] = CARD_TYPE_TWOLINE;
            splitedTypes[nindex] = -1;
            splitedCards[nindex].resize(0);
        }
    }

    //删除合并的单顺
    vector<vector<Card> >::iterator ite = splitedCards.begin();
    for(; ite != splitedCards.end();)
    {
        if (ite->size() == 0)
            ite = splitedCards.erase(ite);
        else
            ++ite;
    }
    //删除合并的单顺记录的类型
    vector<int>::iterator ite2 = splitedTypes.begin();
    for(; ite2 != splitedTypes.end();)
    {
        if (*ite2 == -1)
            ite2 = splitedTypes.erase(ite2);
        else
            ++ite2;
    }
}

void HoleCards::split_twoline(vector<Card> &splits)
{
    while (find_max_len_twoline(splits))
    {
        // cout << "split_twoline.";
        // Card::dump_cards(splits, "splits");
    }
}

void HoleCards::split_two(vector<Card> &splits)
{
    if (splits.size() < 2 ) return;
    CardStatistics splits_stat(splits, ghost_face);
    Card::sort_by_descending(splits_stat.card2);

    for (unsigned i = 0; i < splits_stat.card2.size(); i += 2)
    {
        vector<Card> cards;
        cards.push_back(splits_stat.card2[i]);
        cards.push_back(splits_stat.card2[i+1]);
        delete_cards_from_splits(splits, cards, CARD_TYPE_TWO);
        // cout << "split_two. ";
        // Card::dump_cards(splits, "splits");
    }

    if (splits_stat.ghost_cards.size() == 2)
    {
        vector<Card> cards;
        cards.push_back(splits_stat.ghost_cards[0]);
        cards.push_back(splits_stat.ghost_cards[1]);
        delete_cards_from_splits(splits, cards, CARD_TYPE_TWO);
        // cout << "split_two. ";
        // Card::dump_cards(splits, "splits");
    }
}

void HoleCards::split_one(vector<Card> &splits)
{
    if (splits.size() == 0) return;
    vector<Card>::iterator ite;
    Card::sort_by_descending(splits);
    for(ite = splits.begin(); ite != splits.end();)
    {
        vector<Card> cards;
        cards.push_back(*ite);
        splitedCards.push_back(cards);
        splitedTypes.push_back(CARD_TYPE_ONE);
        ite = splits.erase(ite);
        // cout << "split_one. ";
        // Card::dump_cards(splits, "splites");
    }
}

//计算三顺最大的牌面值个数
int HoleCards::calc_max_line_cnt_of(const CardStatistics &my_card_stat)
{
    int max_cnt = my_card_stat.line3.size() / 3;
    int diceCnt = my_card_stat.ghost_cards.size();
    int card2Size = my_card_stat.card2.size();
    int card1Size = my_card_stat.card1.size();
    if (diceCnt > 0)
    {
        if (diceCnt >card2Size)
        {
            max_cnt += card2Size;
            diceCnt -= card2Size;
            if (diceCnt > card1Size*2)
            {
                max_cnt += card1Size;
                diceCnt -= card1Size * 2; 
            }
            else
            {
                max_cnt += diceCnt / 2;
            }
        }
        else
        {
            max_cnt += diceCnt;
        }
    }
    return max_cnt;
}

//找长度最大的三顺,如果找到，则从splits中删除,加到splitedCards中
bool HoleCards::find_max_len_threeline(vector<Card> &splits)
{
    if (splits.size() < 6) return false; 
    CardStatistics my_card_stat(splits, ghost_face);

    vector<Card> cards;
    std::merge(my_card_stat.card1.begin(), my_card_stat.card1.end(),
              my_card_stat.card2.begin(), my_card_stat.card2.end(), back_inserter(cards)); //merge合并后依然是有序的，不用排序
    std::copy(my_card_stat.line3.begin(), my_card_stat.line3.end(), back_inserter(cards));
    std::sort(cards.begin(), cards.end());

    CardAnalysis cardAna;
    int max_cnt = calc_max_line_cnt_of(my_card_stat);
    for (unsigned cnt = max_cnt; cnt >= 2; --cnt)
    {//从可能最长的三顺开始找
        int diff = my_card_stat.len - cnt * 3;
        for (int index = 0; index <= diff; ++index)
        {
            vector<Card> anaCards = my_card_stat.ghost_cards;
            for (unsigned i = anaCards.size(), j = 0; i < static_cast<unsigned>(cnt * 3); ++i, ++j)
            { //加入其它牌
                anaCards.push_back(cards[index + j]);
            }
            cardAna.analysis(anaCards, ghost_face);
            if (cardAna.get_card_face_of_type(CARD_TYPE_THREELINE) != 0)
            {
                delete_cards_from_splits(splits, anaCards, CARD_TYPE_THREELINE);
                return true;
            }
        }
    }
    return false;
}

//找长度最大的双顺,如果找到，则从splits中删除,加到splitedCards中
bool HoleCards::find_max_len_twoline(vector<Card> &splits)
{
    if (splits.size() < 6 && splits.size() % 2 == 0)
        return false;

    CardStatistics my_card_stat(splits, ghost_face);

    vector<Card> cards;
    std::copy(my_card_stat.line2.begin() ,my_card_stat.line2.end(), back_inserter(cards));

    CardAnalysis cardAna;
    for (unsigned i = 0; i < cards.size(); i+=2)
    {
        vector<Card> anaCards = my_card_stat.ghost_cards;
        for (unsigned j = i; j < cards.size(); ++j)
        { //加入其它牌
            anaCards.push_back(cards[j]);
        }
        if (anaCards.size() < 6)
            break;
        cardAna.analysis(anaCards, ghost_face);
        if (cardAna.get_card_face_of_type(CARD_TYPE_TWOLINE) != 0)
        {
            delete_cards_from_splits(splits, anaCards, CARD_TYPE_TWOLINE);
            return true;
        } 
    }
    for (int i = cards.size() - 1; i >= 0; i-=2)
    {
        vector<Card> anaCards = my_card_stat.ghost_cards;
        for (int j = 0; j <= i; ++j)
        { //加入其它牌
            anaCards.push_back(cards[j]);
        }
        if (anaCards.size() < 6)
            break;
        cardAna.analysis(anaCards, ghost_face);
        if (cardAna.get_card_face_of_type(CARD_TYPE_TWOLINE) != 0)
        {
            delete_cards_from_splits(splits, anaCards, CARD_TYPE_TWOLINE);
            return true;
        } 
    }
    return false;
}

//找长度最大的顺子,如果找到，则从splits中删除,加到splitedCards中
bool HoleCards::find_max_len_oneline(vector<Card> &splits)
{
    if (splits.size() < 5)
        return false;
    CardStatistics my_card_stat(splits, ghost_face);

    vector<Card> cards;
    std::copy(my_card_stat.line1.begin() ,my_card_stat.line1.end(), back_inserter(cards));

    CardAnalysis cardAna;
    for (unsigned i = 0; i < cards.size(); ++i)
    {
        vector<Card> anaCards = my_card_stat.ghost_cards;
        for (unsigned j = i; j < cards.size(); ++j)
        { //加入其它牌
            anaCards.push_back(cards[j]);
        }
        if (anaCards.size() < 5)
            break;
        cardAna.analysis(anaCards, ghost_face);
        if (cardAna.get_card_face_of_type(CARD_TYPE_ONELINE) != 0)
        {
            delete_cards_from_splits(splits, anaCards, CARD_TYPE_ONELINE);
            return true;
        } 
    }
    for (int i = cards.size() - 1; i >= 0; --i)
    {
        vector<Card> anaCards = my_card_stat.ghost_cards;
        for (int j = 0; j <= i; ++j)
        { //加入其它牌
            anaCards.push_back(cards[j]);
        }
        if (anaCards.size() < 5)
            break;
        cardAna.analysis(anaCards, ghost_face);
        if (cardAna.get_card_face_of_type(CARD_TYPE_ONELINE) != 0)
        {
            delete_cards_from_splits(splits, anaCards, CARD_TYPE_ONELINE);
            return true;
        } 
    }
    return false;
}

//找三张,如果找到，则从splits中删除,加到splitedCards中
bool HoleCards::find_splite_three(vector<Card> &splits)
{
    if (splits.size() < 3)
        return false;
    CardStatistics my_card_stat(splits, ghost_face);

    vector<Card> cards;
    std::merge(my_card_stat.card1.begin(), my_card_stat.card1.end(),
              my_card_stat.card2.begin(), my_card_stat.card2.end(), back_inserter(cards)); //merge合并后依然是有序的，不用排序
    std::copy(my_card_stat.line3.begin(), my_card_stat.line3.end(), back_inserter(cards));
    std::sort(cards.begin(), cards.end());
    CardAnalysis cardAna;
    int diff = my_card_stat.len - 3;
    for (int index = 0; index <= diff; ++index)
    {
        vector<Card> anaCards = my_card_stat.ghost_cards;
        for (unsigned i = anaCards.size(), j = 0; i < 3; ++i, ++j)
        { //加入其它牌
            anaCards.push_back(cards[index + j]);
        }
        cardAna.analysis(anaCards, ghost_face);
        if (cardAna.get_card_face_of_type(CARD_TYPE_THREE) != 0)
        {
            delete_cards_from_splits(splits, anaCards, CARD_TYPE_THREE);
            return true;
        }
    }
    return false;
}

//从splits中删除anaCards的牌
void HoleCards::delete_cards_from_splits(vector<Card> &splits, const vector<Card> &anaCards, int splitType)
{
    vector<Card>::iterator ite;
    vector<Card> cards;
    for(unsigned i = 0 ; i < anaCards.size(); ++i)
    {
        // ite = std::find(splits.begin(), splits.end(), anaCards[i]);
        unsigned j = 0;
        for(; j < splits.size(); ++j)
        {
            if (splits[j].value == anaCards[i].value)
                break;
        }
        cards.push_back(*(splits.begin() + j));
        splits.erase(splits.begin() + j);
    }
    splitedCards.push_back(cards);
    splitedTypes.push_back(splitType);
}

/****************robot的相关方法*************/
/*
 @brief 玩家从手牌中选则一个合理的牌打出。
 @param v[out] 返回应该打出去的牌
*/
void HoleCards::robot(std::vector<Card> &v)
{
    int ret = check_three_with_other();
    int ret_type = 0;
    if (ret > 0)
    {
        bool has_bao_dan = false; //记录非队友是否有报单的
        bool has_bao_shuang = false; //记录非队友是否有报双
        get_dan_shuang(has_bao_dan, has_bao_shuang);
        //出单牌
        if (!has_bao_dan)
        { //敌家没报单，尝试出单牌
            ret = get_smallest_type_of(CARD_TYPE_ONE);
            if (ret >= 0)
            {
                v = splitedCards[ret];
                ret_type = CARD_TYPE_ONE;
            }
        }
        else if (has_bao_dan && (splitedTypes.size() - count_type_of(CARD_TYPE_ONE) == 0))
        { //敌家报单，玩家只有单牌可出
            //计算庄家是否为下家
            bool next_seat_is_zhuang = ((landlord_seat - seatid) % 3 == 1);

            if (landlord_seat == seatid || next_seat_is_zhuang)
            { //自己是庄家，或下家就是庄家，则出最大的
                Card max_card(get_one_max_card());
                if (max_card.face > 0)
                {
                    v.pop_back();
                    v.push_back(max_card);
                }
            }
            else
            { //下家是友军，庄家报单，出最小的单牌没事，有下家可以罩着。
                ret = get_smallest_type_of(CARD_TYPE_ONE);
                if (ret >= 0)
                {
                    v = splitedCards[ret];
                    ret_type = CARD_TYPE_ONE;
                }
            }
        }
        //出对子
        else if (!has_bao_shuang) //敌家没报双，尝试出对子
        { 
            ret = get_smallest_type_of(CARD_TYPE_TWO);
            if (ret >=0 )
            {
                v = splitedCards[ret];
                ret_type = CARD_TYPE_TWO;
            }
        }
        else if (has_bao_dan && (splitedTypes.size() - count_type_of(CARD_TYPE_TWO) == 0))
        { //敌家报双，玩家只有对子可出
            Card max_card(get_one_little_card(false));
            if (max_card.face > 0) //找一个最小的牌打出去,拆他人的对子
            {
                v.clear();
                v.push_back(max_card);
            }
        }
        //如果只有对子和单牌，且敌家即有报双，又有报单的情况
        else if (has_bao_dan && has_bao_shuang &&
                 (static_cast<int>(splitedTypes.size()) == count_type_of(CARD_TYPE_ONE) + count_type_of(CARD_TYPE_TWO)))
        {
            Card max_card(get_one_max_card(true));
            if (max_card.face > 0) //找一个最大的牌打出去
            {
                v.clear();
                v.push_back(max_card);
            }
        }
    }
    else
    {
        switch(0)
        {
            case 0: //双顺
                ret = get_smallest_type_of(CARD_TYPE_TWOLINE);
                if (ret >= 0)
                {
                    v = splitedCards[ret];
                    ret_type = CARD_TYPE_TWOLINE;
                    break;
                }
            case 1://连牌
                ret = get_smallest_type_of(CARD_TYPE_ONELINE);
                if (ret >=0 )
                {
                    v = splitedCards[ret];
                    ret_type = CARD_TYPE_ONELINE;
                    break;
                }
            case 2: //出三顺
                ret = get_smallest_type_of(CARD_TYPE_THREELINE);
                if (ret >= 0)
                {
                    v = splitedCards[ret];
                    ret_type = calc_with_cards(v);
                    break;
                }
            case 3: //出三条
                ret = get_smallest_type_of(CARD_TYPE_THREE);
                if (ret >= 0)
                {
                    v = splitedCards[ret];
                    ret_type = calc_with_cards(v);
                    break;
                }
            case 4: //炸弹
                ret = get_smallest_type_of(CARD_TYPE_SOFTBOMB);
                if (ret >= 0)
                {
                    v = splitedCards[ret];
                    ret_type = CARD_TYPE_SOFTBOMB;
                    break;
                }
            case 5:
                ret = get_smallest_type_of(CARD_TYPE_BOMB);
                if (ret >= 0)
                {
                    v = splitedCards[ret];
                    ret_type = CARD_TYPE_BOMB;
                    break;
                }
            case 6:
                ret = get_smallest_type_of(CARD_TYPE_GHOSTBOMB);
                if (ret >= 0)
                {
                    v = splitedCards[ret];
                    ret_type = CARD_TYPE_GHOSTBOMB;
                    break;
                }
            case 7: //火箭
                ret = get_smallest_type_of(CARD_TYPE_ROCKET);
                if (ret >= 0)
                {
                    v = splitedCards[ret];
                    ret_type = CARD_TYPE_ROCKET;
                    break;
                }
        }
    }

    if (ret_type == 0) return ;
    //特殊处理
    special_condition_check(v, ret_type, false);
}

/***robot的工具方法****/
//检测单牌和对子与三条，三顺组合，是否还有多余的单牌和顺子
int HoleCards::check_three_with_other()
{
    int threeCnt = count_threeline_three() +  count_type_of(CARD_TYPE_THREE);
    //首先与对子组合
    int twoCnt = count_type_of(CARD_TYPE_TWO);
    if (twoCnt - threeCnt > 0)
        return twoCnt - threeCnt;

    //与单牌组合
    int oneCnt = count_type_of(CARD_TYPE_ONE);
    oneCnt += twoCnt; //对子可拆，看成两个单牌
    return oneCnt - threeCnt;
}

//计算三顺中三个的个数
int HoleCards::count_threeline_three()
{
    int cnt = 0;
    for (unsigned i = 0; i < splitedTypes.size(); ++i)
    {
        if(splitedTypes[i] == CARD_TYPE_THREELINE)
        {
            cnt += splitedCards[i].size() / 3;
        }
    }
    return cnt;
}

//计算type拆出的个数
int HoleCards::count_type_of(int type)
{
    int cnt = 0;

    vector<int>::iterator ite = splitedTypes.begin();
    for(; ite != splitedTypes.end(); ++ite)
    {
        if (*ite == type)
        {
            ++cnt;
        }
    }
    return cnt;
}

//获取手牌中炸弹的个数，包括火箭
int HoleCards::get_bomb_size()
{
    int cnt = 0;
    for(int i = splitedTypes.size() -1; i >= 0; --i)
    {
        if(splitedTypes[i] >= CARD_TYPE_SOFTBOMB)
        {
            ++cnt;
        }
    }
    return cnt;
}

//获取type类型中最小的那个，并返回最小的在splitedCards中的下标
int HoleCards::get_smallest_type_of(int type)
{
    for(int i = splitedTypes.size() -1; i >= 0; --i)
    {
        if(splitedTypes[i] == type)
        {
            return i;
        }
    }
    return -1;
}

//计算三带多时，需要带什么牌
int HoleCards::calc_with_cards(vector<Card> &v)
{
    //与单牌组合
    int threeLineCnt = count_threeline_three();
    if (threeLineCnt == 0) //说明没有三顺，只有三张
    {
        if (count_type_of(CARD_TYPE_THREE) == 0)
            return 0; //三张都没有，这个函数就不应该被调用
        threeLineCnt = 1;
    }

    const int oneCnt = count_type_of(CARD_TYPE_ONE);
    const int twoCnt = count_type_of(CARD_TYPE_TWO);
    // const int threeCnt = count_type_of(CARD_TYPE_THREE);
    if (threeLineCnt > oneCnt + twoCnt *2 /*+ threeCnt *3*/)
    {
        int lessCard = threeLineCnt - (oneCnt + twoCnt *2); //带的牌不够，减去lessCard的数量的三张
        if (lessCard == threeLineCnt) //没有带的牌， 直接出三连
        {
            if (v.size() == 3)
                return CARD_TYPE_THREE;
            return CARD_TYPE_THREELINE;
        }
        for(int i = 0; i< lessCard; ++i)
        {
            v.pop_back();
            v.pop_back();
            v.pop_back();
        }
        threeLineCnt = oneCnt + twoCnt *2;
    }

    int leftOne = oneCnt - threeLineCnt;
    bool oneBiggerThenK = false; //是否要带出A和2
    vector<Card> oneOut;
    vector<Card> oneBigOut;
    // if (leftOne >= 0) //单牌足够
    // {
        int cnt = 0;
        for(int i = splitedTypes.size() -1; i >= 0; --i)
        {
            if(splitedTypes[i] == CARD_TYPE_ONE)
            {
                ++cnt;
                if (splitedCards[i][0].face > Card::King)
                {
                    oneBiggerThenK = true;
                    oneBigOut.push_back(splitedCards[i][0]);
                }
                else
                {
                    oneOut.push_back(splitedCards[i][0]);
                }
                if (cnt == threeLineCnt)
                    break;
            }
        }
        //单牌够，且面值都小于A，则带出，否则在双牌也不够的时候拆小的对子
        if (leftOne >= 0 && oneBiggerThenK == false)
        {
            std::copy(oneOut.begin(), oneOut.end(), back_inserter(v));
            if (v.size() == 4)
                return CARD_TYPE_THREEWITHONE;
            return CARD_TYPE_PLANEWITHONE;
        }
    // }

    //与对子组合
    int leftTwo = twoCnt - threeLineCnt;
    bool pairBiggerThenK = false; //是否要带出AA和22
    vector<Card> twoOut;
    vector<Card> twoBigOut;
    // if (leftTwo >= 0) //对子足够
    // {
        cnt = 0;
        for(int i = splitedTypes.size() -1; i >= 0; --i)
        {
            if(splitedTypes[i] == CARD_TYPE_TWO)
            {
                ++cnt;
                if (splitedCards[i][0].face > Card::King)
                {
                    pairBiggerThenK = true;
                    twoBigOut.push_back(splitedCards[i][0]);
                    twoBigOut.push_back(splitedCards[i][1]);
                }
                else 
                {
                    twoOut.push_back(splitedCards[i][0]);
                    twoOut.push_back(splitedCards[i][1]);
                }
                if (cnt == threeLineCnt)
                    break;
            }
        }
        //双牌够，且面值都小于A，则带出
        if (leftTwo >= 0 && pairBiggerThenK == false)
        {
            std::copy(twoOut.begin(), twoOut.end(), back_inserter(v));
            if (v.size() == 5)
                return CARD_TYPE_THREEWITHTWO;
            return CARD_TYPE_PLANEWITHWING;
        }
    // }

    //带A以下的单牌，然后拆对子。
    std::copy(oneOut.begin(), oneOut.end(), back_inserter(v));
    int left = threeLineCnt - oneOut.size(); //还拆的个数 
    if (left % 2 == 1 && oneBigOut.size() > 0)
    {//成单，少拆对子
        v.push_back(oneBigOut[0]);
        oneBigOut.erase(oneBigOut.begin());
        --left;
    }

    if (left <= static_cast<int>(twoOut.size()))
    { //小的对子够，拆小的对子
        for(int i = 0; i<left; ++i)
        {
            v.push_back(twoOut[i]);
        }
        if (v.size() == 4)
            return CARD_TYPE_THREEWITHONE;
        return CARD_TYPE_PLANEWITHONE;
    }
    else
    { //小的不够，算上所有小的对子
        std::copy(twoOut.begin(), twoOut.end(), back_inserter(v));
        left -= twoOut.size();
        if (static_cast<int>(oneBigOut.size()) <= left )
        { //将大单牌也算上
            std::copy(oneBigOut.begin(), oneBigOut.end(), back_inserter(v));
            left -= oneBigOut.size();
        }
        if (left > 0 && static_cast<int>(twoBigOut.size()) >= left)
        { //拆大的对子
            for(int i =0; i<left; ++i)
            {
                v.push_back(twoBigOut[i]);
            }
        }
        if (v.size() == 5)
            return CARD_TYPE_THREEWITHONE;
        return CARD_TYPE_PLANEWITHONE;
    }
    return 0;
}

/****************get_play_cards的相关方法*************/
/*
 @brief 分析当前玩家手中的牌,并找出合适的牌型组成存入out_cards,使能合理的管上对家出的last_cards的牌
 @param last_cards[in] 
 @param out_cards[out] 

1.	如果手中有独立的，与所出的牌一样牌型的牌时，先跟之。        
2.	可以作为单牌、对子、三条等形式跟出。          
3.	当手中没有相应牌跟时，如果是本方人员出的牌，可以不跟，如果是对方出的牌，则必须拆牌跟，如果再没有，出炸弹或火箭，否则PASS。          
4.	如果手中的牌除了炸弹。火箭外还剩一手牌，则如果牌型相符，则先跟之，否则炸之。          
5.	单牌的跟牌原则：如果手中有单牌，则跟之，否则拆2跟之，否则拆对牌跟之，否则拆6连以上的单顺顶张跟之，否则拆三条跟之，否则拆三顺跟之，否则拆5连单顺跟之，否则拆双顺跟之，否则炸之，否则PASS。          
6.	对牌的根牌原则：如果手中有对子，则跟之，否则拆4连以上的双顺顶张跟之，否则拆三条跟之，否则拆双顺跟之，否则拆三顺跟之，否则炸之，否则PASS。          
7.	三条、三带1、三带2等牌的根牌原则：如果手中有相同牌型的牌则跟之，否则拆三顺跟之，否则炸之，否则PASS。注意，只有在手中牌在出了以后还剩一手牌时，或直接出完的情况下，才允许带王或2。在没有足够牌带的情况下，参照单牌。对子的拆牌原则进行拆牌处理。          
8.	三顺及三顺带牌的根牌原则：如果有相应的牌型，则跟之，否则可以将大的三顺拆成小的三顺跟之，否则炸之，否则PASS。注意，只有在手中牌在出了以后还剩一手牌时，或直接出完的情况下，才允许带王或2。在没有足够牌带的情况下，参照单牌。对子的拆牌原则进行拆牌处理。         
9.	连牌的跟牌原则：如果有相应的牌型，则跟之，否则拆相同张数的双顺，否则拆相同张数的三顺，否则拆不同张数的连牌，否则拆不同张数的双顺，否则拆不同张数的三顺，否则炸之，否则PASS。          
10.	双顺的跟牌原则：有相同牌型的牌，则跟之，否则拆不同张数的双顺，否则拆不同张数的三顺，否则拆相同张数的三顺，否则炸之，否则PASS。         
11.	炸弹的跟牌原则：有超过所出炸弹的炸弹，或有火箭，则炸之，否则PASS。         
12.	炸弹带两手牌的跟牌原则：如果有炸弹，则炸之，否则PASS。
*/
void HoleCards::get_play_cards(const vector<int> &last_cards, vector<int> &out_cards, int ctype, int cface)
{
    out_cards.clear();
    vector<Card> outCards;

    vector<Card> lastCards;
    for (unsigned i = 0; i < last_cards.size(); ++i)
    {
        lastCards.push_back(Card(last_cards[i]));
    }
    if (ctype == 0 || cface == 0)
    {
        CardAnalysis ana(lastCards, ghost_face);
        if (ana.type == 0 ) return;
        ctype = ana.type;
        cface = ana.face;
    }
    std::vector<Card> curCards;
    copy_cards(&curCards);
   	CardFind *cf = new CardFind();

    int ret = cf->tip(lastCards, curCards, ctype, cface, ghost_face);
    if (ret != 0 || cf->results.size() == 0)
    {
        return;
    }

    int ttype = ctype;
    if (ctype == CARD_TYPE_THREEWITHONE || ctype == CARD_TYPE_THREEWITHTWO)
    { //三带1、三带2当三张处理，带的牌最后统一处理。
        ttype = CARD_TYPE_THREE;
    }
    if (ctype == CARD_TYPE_PLANEWITHONE || ctype == CARD_TYPE_PLANEWITHWING)
    { //飞机带羿、飞机带翅，带的牌最后统一处理。
        ttype = CARD_TYPE_THREELINE;
    }

    ret = find_splite_bigger_of(ttype, cface);
    if (ret == -1)
    { //说明要拆牌或打炸弹
        if (table_type != 7 && landlord_seat != seatid && pre_play_seat != landlord_seat) //7是各自为战
        { //友军，我们选择不打他
            ;
        }
        else
        {//敌军, 尽量打他
            if (splitedCards.size() - get_bomb_size() <= 1)            
            { //如果手中的牌除了炸弹。火箭外还剩一手牌，则如果牌型相符，则先跟之，否则炸之。
                play_bomb(outCards);
            }
            switch (ctype)
            {
            case CARD_TYPE_ONE:
                //单牌的跟牌原则：
                split_cards_for_oneType(curCards, cface, outCards);
                play_bomb(outCards);
                break;
            case CARD_TYPE_TWO:
                //对牌的根牌原则：
                split_cards_for_twoType(cface, outCards);
                play_bomb(outCards);
                break;
            case CARD_TYPE_THREE:
            case CARD_TYPE_THREEWITHONE:
            case CARD_TYPE_THREEWITHTWO:
                //三条、三带1、三带2等牌的根牌原则：
                split_cards_for_threeType(cface, outCards);
                get_followd_with_cards(ctype, outCards);
                play_bomb(outCards);
                break;
            case CARD_TYPE_THREELINE:
            case CARD_TYPE_PLANEWITHONE:
            case CARD_TYPE_PLANEWITHWING:
                //三顺及三顺带牌的根牌原则, [如果有相应的牌型，则跟之，否则可以将大的三顺拆成小的三顺跟之，在ret!=-1的时候处理这种情况]
                get_followd_with_cards(ctype, outCards, lastCards);
                play_bomb(outCards);
                break;
            case CARD_TYPE_ONELINE:
                //连牌的跟牌原则
                split_cards_for_oneLineType(cface, outCards, lastCards.size());
                play_bomb(outCards);
                break;
            case CARD_TYPE_TWOLINE:
                //双顺的跟牌原则：
                split_cards_for_twoLineType(cface, outCards, lastCards.size());
                play_bomb(outCards);
                break;
            case CARD_TYPE_SOFTBOMB:
            case CARD_TYPE_BOMB:
            case CARD_TYPE_GHOSTBOMB:
                //炸弹的跟牌原则：
                //有超过所出炸弹的炸弹，或有火箭，则炸之，否则PASS。
                play_bomb(outCards, ctype, cface);
                break;
            case CARD_TYPE_FOURWITHONE:
            case CARD_TYPE_FOURWITHTWO:
                // 炸弹带两手牌的跟牌原则：如果有炸弹，则炸之，否则PASS。
                play_bomb(outCards);
                break;
            }
        }
    }
    else
    { //跟牌。
        outCards = splitedCards[ret];
    }
    //特殊处理
    // Card::dump_cards(outCards, "outCards");
    special_condition_check(outCards, ctype, true);
    for (unsigned i = 0; i < outCards.size(); ++i)
    {
        out_cards.push_back(outCards[i].value);
    }
}

//在拆分的牌中，找到比ctype,cface大的牌型
int HoleCards::find_splite_bigger_of(int ctype, int cface)
{
    if (ctype >= CARD_TYPE_SOFTBOMB)
        return -1;

    if (ctype == CARD_TYPE_ONELINE || ctype == CARD_TYPE_TWOLINE ||
        ctype == CARD_TYPE_THREELINE || ctype == CARD_TYPE_THREEWITHONE ||
        ctype == CARD_TYPE_THREEWITHTWO || ctype == CARD_TYPE_PLANEWITHONE ||
        ctype == CARD_TYPE_PLANEWITHWING) //这些牌型，涉及到带牌或长度的问题，不用完成匹配，所以返回-1，当做需要拆牌处理
        return -1;

    for (int i = splitedTypes.size() - 1; i >= 0; --i)
    {
        if(splitedTypes[i] == ctype)
        {
            CardAnalysis card_ana(splitedCards[i]);
            int find_face = card_ana.get_card_face_of_type(ctype);
            if (find_face > cface)
                return i;
        }
    }
    return -1;
}

//用炸弹打type牌, 如果找到炸弹，则存到outCards中
//@param type[in] 如果type值不是炸弹，可使用默认0.
//@param face[in] 如果type值不是炸弹，可使用默认0.因为只有炸弹才需要比面值
int HoleCards::play_bomb(vector<Card>& outCards, int type, int face)
{
    if (outCards.size() > 0)
    { //说明有可以打的牌，不打炸弹
        return -1;
    }
    for (int i = splitedTypes.size() - 1; i >= 0; --i)
    {
        if (splitedTypes[i] >= CARD_TYPE_SOFTBOMB)
        {
            if (type < CARD_TYPE_SOFTBOMB)
            {
                outCards = splitedCards[i];
                return 0;
            }
            else if (splitedTypes[i] > type)
            {
                outCards = splitedCards[i];
                return 0;
            }
            else
            { //type值相等的情况，需要比face值
                CardAnalysis ana( splitedCards[i]);
                if (ana.face > face)
                {
                    outCards = splitedCards[i];
                    return 0;
                }
            }
        }
    }
    return -1;
}

//拆牌打单牌
//@prama curCards 当前玩家的手牌
//@prama cface 上次玩家出的单牌
//@prama outCards[out] 机器人应该打出去的牌，也有可能没有需要打出去的牌。
void HoleCards::split_cards_for_oneType(vector<Card> &curCards, int cface, vector<Card> &outCards)
{
    //拆2跟之，
    if (ghost_face != Card::Two)
    { //鬼牌不是2的情况下，才拆2
        std::vector<Card>::iterator ite = std::find(curCards.begin(), curCards.end(), Card(2));
        if (ite != curCards.end())
        {
            outCards.push_back(*ite);
            return;
        }
    }
    //否则拆对牌跟之
    find_splited_bigger_then_one(cface, CARD_TYPE_TWO, outCards);
    if (outCards.size() > 0)
    {
        return;
    }

    //否则拆6连以上的单顺顶张跟之
    find_splited_oneLine_bigger_then_one(cface, 6, outCards);
    if (outCards.size() > 0)
    {
        return;
    }

    //否则拆三条跟之，
    find_splited_bigger_then_one(cface, CARD_TYPE_THREE, outCards);
    if (outCards.size() > 0)
    {
        return;
    }

    //否则拆三顺跟之
    find_splited_bigger_then_one(cface, CARD_TYPE_THREELINE, outCards);
    if (outCards.size() > 0)
    {
        return;
    }

    //否则拆5连单顺跟之
    find_splited_oneLine_bigger_then_one(cface, 5, outCards);
    if (outCards.size() > 0)
    {
        return;
    }

    //否则拆双顺跟之
    find_splited_bigger_then_one(cface, CARD_TYPE_TWOLINE, outCards);
    if (outCards.size() > 0)
    {
        return;
    }
}

//拆type牌值打单牌，如果有能拆的type牌
void HoleCards::find_splited_bigger_then_one(int cface, int ctype, vector<Card> &outCards)
{
    for (int i = splitedTypes.size() - 1; i >= 0; --i)
    {
        if (splitedTypes[i] == ctype)
        {
            const int cnt = splitedCards[i].size();
            if (splitedCards[i][cnt-1].face > cface)
            {//从最后限不会取到鬼牌
                outCards.push_back(splitedCards[i][cnt-1]);
                return ;
            }
        }
    }
}

//拆lineNum长度以上长度的单顺（包括lineNum长度）打单牌
void HoleCards::find_splited_oneLine_bigger_then_one(int cface, int lineNum, vector<Card> &outCards)
{
    for (int i = splitedTypes.size() - 1; i >= 0; --i)
    {
        if (splitedTypes[i] == CARD_TYPE_ONELINE)
        {
            const int lineCnt = splitedCards[i].size();
            if (lineCnt < lineNum) 
                continue;

            if (splitedCards[i][lineCnt - 1].face > cface)
            { //取最大的一个打出。
                outCards.push_back(splitedCards[i][lineCnt - 1]);
            }
        }
    }
}

//拆牌打对子
void HoleCards::split_cards_for_twoType(int cface, vector<Card> &outCards)
{
    //拆4连以上的双顺顶张跟之，
    find_splited_twoLine_bigger_then_two(cface, 4, outCards);
    if (outCards.size() > 0)
    {
        return;
    }

    //否则拆三条跟之
    find_splited_bigger_then_two(cface, CARD_TYPE_THREE, outCards);
    if (outCards.size() > 0)
    {
        return;
    }
    
    //否则拆双顺跟之
    find_splited_twoLine_bigger_then_two(cface, 3, outCards);
    if (outCards.size() > 0)
    {
        return;
    }
    
    //否则拆三顺跟之
    find_splited_bigger_then_two(cface, CARD_TYPE_THREELINE, outCards);
    if (outCards.size() > 0)
    {
        return;
    }
}

//拆lineNum长度以上长度的双顺（包括lineNum长度）打对子
void HoleCards::find_splited_twoLine_bigger_then_two(int cface, int lineNum, vector<Card> & outCards)
{
    for (int i = splitedTypes.size() - 1; i >= 0; --i)
    {
        if (splitedTypes[i] == CARD_TYPE_TWOLINE)
        {
            const int lineCnt = splitedCards[i].size();
            if (lineCnt/2 < lineNum) 
                continue;

            if (splitedCards[i][lineCnt - 1].face > cface)
            { //取最大的一个打出。
                outCards.push_back(splitedCards[i][lineCnt - 1]);
                outCards.push_back(splitedCards[i][lineCnt - 2]);
                return ;
            }
        }
    }
}

//拆type牌值打对子
void HoleCards::find_splited_bigger_then_two(int cface, int ctype, vector<Card> &outCards)
{
    for (int i = splitedTypes.size() - 1; i >= 0; --i)
    {
        if (splitedTypes[i] == ctype)
        {
            const int cnt = splitedCards[i].size();
            if (splitedCards[i][cnt-1].face > cface)
            {//从最后限不会取到鬼牌
                outCards.push_back(splitedCards[i][cnt-1]);
                outCards.push_back(splitedCards[i][cnt-2]);
                return ;
            }
        }
    }
}

//拆牌打三张，三带1，三带
void HoleCards::split_cards_for_threeType(int cface, vector<Card> &outCards)
{
    //拆三顺跟之，注意，只有在手中牌在出了以后还剩一手牌时，
    //或直接出完的情况下，才允许带王或2。
    for (int i = splitedTypes.size() - 1; i >= 0; --i)
    {
        if (splitedTypes[i] == CARD_TYPE_THREELINE)
        {
            const int cnt = splitedCards[i].size();
            if (splitedCards[i][cnt-1].face > cface)
            {//找到三顺
                outCards.push_back(splitedCards[i][cnt-1]); 
                outCards.push_back(splitedCards[i][cnt-2]);
                outCards.push_back(splitedCards[i][cnt-3]);
                return ;
            }
        }
    }
}

//跟牌的时候，计算三带1，三带2等带的牌
void HoleCards::get_followd_with_cards(int ctype, vector<Card> &outCards, const vector<Card> &lastCards)
{
    if (ctype == CARD_TYPE_THREEWITHONE)
    { //在单牌中找带的
        calc_planewithone(outCards);
    }

    else if (ctype == CARD_TYPE_THREEWITHTWO)
    { //在对子中找带的
        calc_planewithtwo(outCards);
    }

    else if (ctype == CARD_TYPE_THREELINE)
    {
        delete_extra_three_cards(outCards, lastCards);
    }
    else if (ctype == CARD_TYPE_PLANEWITHONE)
    {
        delete_extra_three_cards(outCards, lastCards);
        calc_planewithone(outCards);
    }
    else if (ctype == CARD_TYPE_PLANEWITHWING)
    {
        delete_extra_three_cards(outCards, lastCards);
        calc_planewithtwo(outCards);
    }
    return ;
}

//拆牌打三顺，飞机带羿，飞机带翅
void HoleCards::split_cards_for_threeLineType(int cface, vector<Card> &outCards)
{
    //因为三顺的牌都没有可以用来拆的牌，这个函数就没有调用.
}

//计算飞机带羿时，计算带的牌
void HoleCards::calc_planewithone(vector<Card> &v)
{
    //与单牌组合
    const int threeLineCnt = v.size() / 3;

    const int oneCnt = count_type_of(CARD_TYPE_ONE);
    const int twoCnt = count_type_of(CARD_TYPE_TWO);
    // const int threeCnt = count_type_of(CARD_TYPE_THREE);
    if (threeLineCnt > oneCnt + twoCnt *2 /*+ threeCnt *3*/)
    { //带的牌不够,不出
        v.clear();
        return ;
    }

    int leftOne = oneCnt - threeLineCnt;
    bool oneBiggerThenK = false; //是否要带出A和2
    vector<Card> oneOut;
    vector<Card> oneBigOut;
    int cnt = 0;
    for (int i = splitedTypes.size() - 1; i >= 0; --i)
    {
        if (splitedTypes[i] == CARD_TYPE_ONE)
        {
            ++cnt;
            if (splitedCards[i][0].face > Card::King)
            {
                oneBiggerThenK = true;
                oneBigOut.push_back(splitedCards[i][0]);
            }
            else
            {
                oneOut.push_back(splitedCards[i][0]);
            }
            if (cnt == threeLineCnt)
                break;
        }
    }
    std::copy(oneOut.begin(), oneOut.end(), back_inserter(v));
    if (leftOne >= 0 && oneBiggerThenK == false)
    { //单牌够，且面值都小于A，则不用再找了。
        return;
    }

    //用对子补
    int needTwoCnt = threeLineCnt - oneOut.size();
    bool pairBiggerThenK = false; //是否要带出AA和22
    vector<Card> twoOut;
    vector<Card> twoBigOut;
    cnt = 0;
    for (int i = splitedTypes.size() - 1; i >= 0; --i)
    {
        if (splitedTypes[i] == CARD_TYPE_TWO)
        {
            if (splitedCards[i][0].face > Card::King)
            {
                pairBiggerThenK = true;
                twoBigOut.push_back(splitedCards[i][0]);
                if (cnt == needTwoCnt)
                    break;
                twoBigOut.push_back(splitedCards[i][1]);
                if (cnt == needTwoCnt)
                    break;
            }
            else
            {
                twoOut.push_back(splitedCards[i][0]);
                if (cnt == needTwoCnt)
                    break;
                twoOut.push_back(splitedCards[i][1]);
                if (cnt == needTwoCnt)
                    break;
            }
        }
    }
    std::copy(twoOut.begin(), twoOut.end(), back_inserter(v));
    if (cnt == needTwoCnt && pairBiggerThenK == false)
    { //小的对子够，且面值都小于A，则不用再找了。
        return;
    }

    //要带A，2,王的情况，目前逻辑先不压。
    v.clear(); 
    return ;
}

//计算飞机带翅时，计算带的牌
void HoleCards::calc_planewithtwo(vector<Card> &v)
{
    const int threeLineCnt = v.size() / 3;

    const int twoCnt = count_type_of(CARD_TYPE_TWO);
    // const int threeCnt = count_type_of(CARD_TYPE_THREE);
    if (threeLineCnt > twoCnt/*+ threeCnt *3*/)
    { //带的牌不够,不出
        v.clear();
        return ;
    }

    bool pairBiggerThenK = false; //是否要带出AA和22
    vector<Card> twoOut;
    vector<Card> twoBigOut;
    int cnt = 0;
    for (int i = splitedTypes.size() - 1; i >= 0; --i)
    {
        if (splitedTypes[i] == CARD_TYPE_TWO)
        {
            ++cnt;
            if (splitedCards[i][0].face > Card::King)
            {
                pairBiggerThenK = true;
                twoBigOut.push_back(splitedCards[i][0]);
                twoBigOut.push_back(splitedCards[i][1]);
            }
            else
            {
                twoOut.push_back(splitedCards[i][0]);
                twoOut.push_back(splitedCards[i][1]);
            }
            if (cnt == threeLineCnt)
                break;
        }
    }
    std::copy(twoOut.begin(), twoOut.end(), back_inserter(v));
    if (cnt == threeLineCnt && pairBiggerThenK == false)
    { //小的对子够，且面值都小于A，则不用再找了。
        return;
    }

    //要带A，2的情况，目前逻辑先不压。
    v.clear(); 
    return ;
}

void HoleCards::delete_extra_three_cards(vector<Card> &outCards ,const vector<Card>& lastCards)
{
    if (outCards.size() < lastCards.size())
    { //说明长度不够
        outCards.clear();
        outCards.clear();
        play_bomb(outCards);
    }
    int diff = outCards.size() - lastCards.size();
    for (int i = 0; i< diff; ++i)
    { //去掉多余的长度
        outCards.erase(outCards.begin());
    }
}

//拆牌打单顺
void HoleCards::split_cards_for_oneLineType(int cface, vector<Card> &outCards, int lineCnt)
{
    //拆单顺
    find_splited_oneLine_bigger_then_oneLine(cface, outCards, lineCnt, 0);
    if (outCards.size() != 0)
        return;

    //拆不同张数的单顺
    find_splited_oneLine_bigger_then_oneLine(cface, outCards, lineCnt, 1);
    if (outCards.size() != 0)
        return;

    //拆相同张数的双顺，
    find_splited_twoLine_bigger_then_oneLine(cface, outCards, lineCnt, 0);
    if (outCards.size() != 0)
        return;
    
    //否则拆相同张数的三顺，
    find_splited_threeLine_bigger_then_oneLine(cface, outCards, lineCnt, 0);
    if (outCards.size() != 0)
        return;

    //否则拆不同张数的连牌，否则拆不同张数的双顺，
    find_splited_twoLine_bigger_then_oneLine(cface, outCards, lineCnt, 1);
    if (outCards.size() != 0)
        return;

    find_splited_threeLine_bigger_then_oneLine(cface, outCards, lineCnt, 1);
    if (outCards.size() != 0)
        return;
}

//拆clineCnt长度以上长度的单顺（包括clineCnt长度）打单顺
//type[in]  0表示拆和clineCnt相同张数的顺子， 1表示拆更长的张数的顺子
void HoleCards::find_splited_oneLine_bigger_then_oneLine(int cface, vector<Card> & outCards, int clineCnt, int type)
{
    if (type == 0)
    {
        for (int i = splitedTypes.size() - 1; i >= 0; --i)
        { //找正好长度相等的单顺
            if (splitedTypes[i] == CARD_TYPE_ONELINE)
            {
                const int lineCnt = splitedCards[i].size();
                if (lineCnt != clineCnt)
                    continue;

                if (splitedCards[i][lineCnt - 1].face <= cface)
                    continue; //顺子的面值太小

                for (int j = 0; j < lineCnt; ++j)
                {
                    outCards.push_back(splitedCards[i][j]);
                }
                return;
            }
        }
    } //end if type
    else
    {
        for (int i = splitedTypes.size() - 1; i >= 0; --i)
        { //拆长的单顺
            if (splitedTypes[i] == CARD_TYPE_ONELINE)
            {
                const int lineCnt = splitedCards[i].size();
                if (lineCnt <= clineCnt)
                    continue;

                if (splitedCards[i][lineCnt - 1].face <= cface)
                    continue; //顺子的面值太小

                //取刚好大于cface的顺子
                int clowFace = cface - clineCnt + 1;
                int j = 0;
                while (splitedCards[i][j].face <= clowFace)
                {
                    ++j;
                }

                for (; j < lineCnt; ++j)
                {
                    outCards.push_back(splitedCards[i][j]);
                }
                return;
            }
        }
    }
}

//拆clineCnt长度以上长度的双顺（包括clineCnt长度）打单顺
//type[in]  0表示拆和clineCnt相同张数的顺子， 1表示拆更长的张数的顺子
void HoleCards::find_splited_twoLine_bigger_then_oneLine(int cface, vector<Card> & outCards, int clineCnt, int type)
{
    if (type == 0)
    {
        for (int i = splitedTypes.size() - 1; i >= 0; --i)
        { //找正好长度相等的
            if (splitedTypes[i] == CARD_TYPE_TWOLINE)
            {
                const int lineCnt = splitedCards[i].size();
                if (lineCnt / 2 != clineCnt)
                    continue;

                if (splitedCards[i][lineCnt - 1].face <= cface)
                    continue; //顺子的面值太小

                for (int j = 0; j < lineCnt; j += 2)
                {
                    outCards.push_back(splitedCards[i][j]);
                }
                return;
            }
        }
    }
    else
    {
        for (int i = splitedTypes.size() - 1; i >= 0; --i)
        { //拆长的
            if (splitedTypes[i] == CARD_TYPE_TWOLINE)
            {
                const int lineCnt = splitedCards[i].size();
                if (lineCnt / 2 <= clineCnt)
                    continue;

                if (splitedCards[i][lineCnt - 1].face <= cface)
                    continue; //顺子的面值太小

                //取刚好大于cface的顺子
                int clowFace = cface - clineCnt + 1;
                int j = 0;
                while (splitedCards[i][j].face <= clowFace)
                {
                    j += 2;
                }

                for (; j < lineCnt; j += 2)
                {
                    outCards.push_back(splitedCards[i][j]);
                }
                return;
            }
        }
    }
}

//拆clineCnt长度以上长度的三顺（包括clineCnt长度）打单顺
//type[in]  0表示拆和clineCnt相同张数的顺子， 1表示拆更长的张数的顺子
void HoleCards::find_splited_threeLine_bigger_then_oneLine(int cface, vector<Card> & outCards, int clineCnt, int type)
{
    if (type == 0)
    {
        for (int i = splitedTypes.size() - 1; i >= 0; --i)
        { //找正好长度相等的
            if (splitedTypes[i] == CARD_TYPE_THREELINE)
            {
                const int lineCnt = splitedCards[i].size();
                if (lineCnt / 3 != clineCnt)
                    continue;

                if (splitedCards[i][lineCnt - 1].face <= cface)
                    continue; //顺子的面值太小

                for (int j = 0; j < lineCnt; j += 3)
                {
                    outCards.push_back(splitedCards[i][j]);
                }
                return;
            }
        }
    }
    else
    {
        for (int i = splitedTypes.size() - 1; i >= 0; --i)
        {
            if (splitedTypes[i] == CARD_TYPE_THREELINE)
            {
                const int lineCnt = splitedCards[i].size();
                if (lineCnt / 3 <= clineCnt)
                    continue;

                if (splitedCards[i][lineCnt - 1].face <= cface)
                    continue; //顺子的面值太小

                //取刚好大于cface的顺子
                int clowFace = cface - clineCnt + 1;
                int j = 0;
                while (splitedCards[i][j].face <= clowFace)
                {
                    j += 3;
                }

                for (; j < lineCnt; j += 3)
                {
                    outCards.push_back(splitedCards[i][j]);
                }
                return;
            }
        }
    }
}

//拆牌打双顺
void HoleCards::split_cards_for_twoLineType(int cface, vector<Card> &outCards, int lineCnt)
{
    //拆相同牌型的牌
    find_splited_twoLine_bigger_then_twoLine(cface, outCards, lineCnt, 0);
    if (outCards.size() != 0)
        return;

    //否则拆不同张数的双顺，
    find_splited_twoLine_bigger_then_twoLine(cface, outCards, lineCnt, 1);
    if (outCards.size() != 0)
        return;
    
    //否则拆相同张数的三顺，
    find_splited_threeLine_bigger_then_twoLine(cface, outCards, lineCnt, 0);
    //拆不同数量的三顺
    find_splited_threeLine_bigger_then_twoLine(cface, outCards, lineCnt, 1);
}

//拆clineCnt长度以上长度的双顺（包括clineCnt长度）打双顺
//type[in]  0表示拆和clineCnt相同张数的顺子， 1表示拆更长的张数的顺子
void HoleCards::find_splited_twoLine_bigger_then_twoLine(int cface, vector<Card> & outCards, int clineCnt, int type)
{
    if (type == 0)
    {
        for (int i = splitedTypes.size() - 1; i >= 0; --i)
        { //找正好长度相等的
            if (splitedTypes[i] == CARD_TYPE_TWOLINE)
            {
                const int lineCnt = splitedCards[i].size();
                if (lineCnt != clineCnt)
                    continue;

                if (splitedCards[i][lineCnt - 1].face <= cface)
                    continue; //顺子的面值太小

                for (int j = 0; j < lineCnt; ++j)
                {
                    outCards.push_back(splitedCards[i][j]);
                }
                return;
            }
        }
    }
    else
    {
        for (int i = splitedTypes.size() - 1; i >= 0; --i)
        { //拆长的
            if (splitedTypes[i] == CARD_TYPE_TWOLINE)
            {
                const int lineCnt = splitedCards[i].size();
                if (lineCnt <= clineCnt)
                    continue;

                if (splitedCards[i][lineCnt - 1].face <= cface)
                    continue; //顺子的面值太小

                //取刚好大于cface的顺子
                int clowFace = cface - clineCnt + 1;
                int j = 0;
                while (splitedCards[i][j].face <= clowFace)
                {
                    j += 2;
                }

                for (; j < lineCnt; ++j)
                {
                    outCards.push_back(splitedCards[i][j]);
                }
                return;
            }
        }
    }
}

//拆clineCnt长度以上长度的三顺（包括clineCnt长度）打双顺
//type[in]  0表示拆和clineCnt相同张数的顺子， 1表示拆更长的张数的顺子
void HoleCards::find_splited_threeLine_bigger_then_twoLine(int cface, vector<Card> & outCards, int clineCnt, int type)
{
    if (type == 0)
    {
        for (int i = splitedTypes.size() - 1; i >= 0; --i)
        { //找正好长度相等的
            if (splitedTypes[i] == CARD_TYPE_THREELINE)
            {
                const int lineCnt = splitedCards[i].size();
                if ((lineCnt / 3)*2 != clineCnt)
                    continue;

                if (splitedCards[i][lineCnt - 1].face <= cface)
                    continue; //顺子的面值太小

                for (int j = 0; j < lineCnt; j += 3)
                {
                    outCards.push_back(splitedCards[i][j]);
                    outCards.push_back(splitedCards[i][j+1]);
                }
                return;
            }
        }
    }
    else
    {
        for (int i = splitedTypes.size() - 1; i >= 0; --i)
        {
            if (splitedTypes[i] == CARD_TYPE_THREELINE)
            {
                const int lineCnt = splitedCards[i].size();
                if ((lineCnt / 3)*2 <= clineCnt)
                    continue;

                if (splitedCards[i][lineCnt - 1].face <= cface)
                    continue; //顺子的面值太小

                //取刚好大于cface的顺子
                int clowFace = cface - clineCnt + 1;
                int j = 0;
                while (splitedCards[i][j].face <= clowFace)
                {
                    j += 3;
                }

                for (; j < lineCnt; j += 3)
                {
                    outCards.push_back(splitedCards[i][j]);
                    outCards.push_back(splitedCards[i][j+1]);
                }
                return;
            }
        }
    }
}

/****************特殊情况的相关函数************************/
/*
1.	自己是庄家
    a)	在出牌时，如果偏家有一个人只剩一张牌时，尽量不出单牌，否则单牌由大到小出。                  
    b)	在跟牌时，如果偏家有一个人只剩一张牌时，跟手中最大的牌。 
    c)	如果有玩家报双，不打对子，如果只有对子，则拆最小的对子打

2.	偏家打法：
    a)	如果是庄家的上家，且庄家只剩一张牌时，尽量不出单牌，否则单牌由大到小出。                  
    b)	在跟牌时，如果是庄家的上家，且庄家只剩一张牌时，跟手中最大的牌。 
    c)	当一个偏家打出的是单牌时，一般情况下能跟就跟。如果手中必须跟J或以上的牌时，选择PASS。 
    d)	当一个偏家打出的是对子时，一般情况下能跟就跟。如果手中必须跟JJ或以上的牌时，选择PASS。
    e)	如果一个偏家打出的牌是除了单牌及对子以外的牌型，则选择PASS。 
    f)	如果处在下家的偏家只剩一张牌时，在出牌时出手中最小的牌。跟牌还按照一般的原则。
    g)	如果庄家报双，不打对子，如果只有对子，则拆最小的对子打

*/
void HoleCards::special_condition_check(std::vector<Card> &v, int ret_type, bool isFollow)
{
    if (table_type == 7 || landlord_seat == seatid) //自己是庄家，或各自为战
    {
        if (!isFollow)
        { //在出牌时，如果偏家有一个人只剩一张牌时，尽量不出单牌，否则单牌由大到小出。
            //如果有玩家报双，不打对子，如果只有对子，则拆最小的对子打
            //这种情况在robot方法中已经处理。
        }
        else
        { //在跟牌时，如果偏家有一个人只剩一张牌时，跟手中最大的牌。
            if (ret_type == CARD_TYPE_ONE && v.size() == 1)
            {
                bool has_bao_dan = false;    //记录非队友是否有报单的
                bool has_bao_shuang = false; //记录非队友是否有报双
                get_dan_shuang(has_bao_dan, has_bao_shuang);
                //在跟牌时，如果有一个人只剩一张牌时，跟手中最大的牌。
                if (has_bao_dan)
                {
                    Card max_card(get_one_max_card());
                    if (max_card.face != v[0].face)
                    {
                        v.clear();
                        v.push_back(max_card);
                    }
                } // end has_bao_dan
            }
        }
    }
    else //偏家打法
    {
        //计算庄家是否为下家
        int diffSeat = landlord_seat - seatid;
        if (diffSeat < 0)
            diffSeat += 3;
        bool next_seat_is_zhuang = (diffSeat == 1);

        if (isFollow) //跟牌
        {
            bool has_bao_dan = false;    //记录非队友是否有报单的
            bool has_bao_shuang = false; //记录非队友是否有报双
            get_dan_shuang(has_bao_dan, has_bao_shuang);

            if (pre_play_seat != landlord_seat)
            {
                if (ret_type == CARD_TYPE_ONE && v.size() == 1)
                { // 当一个偏家打出的是单牌时，一般情况下能跟就跟。如果手中必须跟J或以上的牌时，选择PASS。(报单不能选择pass)
                    if (v[0].face >= Card::Jack && !has_bao_dan)
                        v.clear();
                }
                else if (ret_type == CARD_TYPE_TWO && v.size() == 2)
                { // 当一个偏家打出的是对子时，一般情况下能跟就跟。如果手中必须跟JJ或以上的牌时，选择PASS。（报双不能选择pass）
                    if (v[0].face >= Card::Jack && !has_bao_shuang)
                        v.clear();
                }
                else if (ret_type != CARD_TYPE_BOMB)
                { // 如果一个偏家打出的牌是除了单牌及对子以外的牌型，则选择PASS。
                    v.clear();
                }
            }

            if (next_seat_is_zhuang)
            {//在跟牌时，如果下家是庄家，且庄家只剩一张牌时，跟手中最大的牌。
                if (ret_type == CARD_TYPE_ONE && v.size() == 1)
                {
                    //在跟牌时，如果有一个人只剩一张牌时，跟手中最大的牌。
                    if (has_bao_dan)
                    {
                        Card max_card(get_one_max_card());
                        if (max_card.face != v[0].face)
                        {
                            v.pop_back();
                            v.push_back(max_card);
                        }
                    } // end has_bao_dan
                }
            }
        }
        else
        {
            if (next_seat_is_zhuang)
            { //如果庄家的下家，且庄家只剩一张牌时，尽量不出单牌，否则单牌由大到小出
                //如果庄家报双，不打对子，如果只有对子，则拆最小的对子打
                //这种情况在robot方法中已经处理。
            }
            else
            {  //如果处在下家的偏家只剩一张牌时，在出牌时出手中最小的牌。跟牌还按照一般的原则。
                bool has_bao_dan = false;    //记录非队友是否有报单的
                bool has_bao_shuang = false; //记录非队友是否有报双
                get_dan_shuang(has_bao_dan, has_bao_shuang);
                Card little_card(get_one_little_card(true));
                if (little_card.face != v[0].face)
                {
                    v.clear();
                    v.push_back(little_card);
                }
            }
        }
    }
}

/*
 @brief 检测敌对家是否有单双
 @param has_bao_ban[out] 返回是否有人报单
 @param has_bao_shuang[out] 返回是否有人报双
*/
void HoleCards::get_dan_shuang(bool &has_bao_dan, bool &has_bao_shuang)
{
    has_bao_dan = false; //记录非队友是否有报单的
    has_bao_shuang = false; //记录非队友是否有报双

    if (landlord_seat == seatid || table_type == 7)
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