#pragma once

//发牌堆类
class Cards{
public:
	Cards();
	void RandCards(void);//洗牌
	int GetCard();//抹牌
	int GetRemain(void){ return remain; };

private:
	int cards[54];//牌数组
	int remain;//剩余牌数
};