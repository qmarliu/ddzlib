#ifndef _CARD_TYPE_H_
#define _CARD_TYPE_H_

enum CardType
{
	CARD_TYPE_ERROR = 0,			// 错误类型
	CARD_TYPE_ONE = 1,			    // 单牌
	CARD_TYPE_ONELINE = 2,		    // 单连牌
	CARD_TYPE_TWO = 3,			    // 对子
	CARD_TYPE_TWOLINE = 4,		    // 连对		3个点数连续的
	CARD_TYPE_THREE = 5,			// 三张		
	CARD_TYPE_THREELINE = 6,		// 三顺		2个以上连续点数
	CARD_TYPE_THREEWITHONE = 7,	    // 三带一  带1个单牌	
	CARD_TYPE_THREEWITHTWO = 8,  	// 三带二  带1个对子
	CARD_TYPE_PLANEWITHONE = 9, 	// 飞机带羿	和三带一差不多
	CARD_TYPE_PLANEWITHWING = 10,	// 飞机带翅	和三带二差不多
	CARD_TYPE_FOURWITHONE = 11, 	// 4个带一 带2个单牌
	CARD_TYPE_FOURWITHTWO = 12, 	// 4个带二 带2个对子
	CARD_TYPE_SOFTBOMB = 13,		// 4个	软炸弹，有癞子
	CARD_TYPE_BOMB = 14,			// 4个		硬炸弹
	CARD_TYPE_GHOSTBOMB = 15,		// 4个	纯癞子炸弹
	CARD_TYPE_ROCKET = 16,		    // 2个鬼	火箭
};

#endif /* _CARD_TYPE_H_ */
