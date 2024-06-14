#pragma once
#include "player.h"

class Robot : public Player {
public:
    Robot(Game &game) : Player(game) {}

    bool IsHuman() const override {
        return false;
    }
	int GetBaseScore(int questioned,int nowscore);//本局是否想当地主，并给出基本分
    void DivideIntoGroups(void); //分析并拆分牌型
	void ThreeplusAndAirplane(void);//从分析后的基本牌型中组合三带一和飞机
	void DeleteUnkown(void);//删除牌型集合中未知类型
	void SelectCards(bool hint=false);//AI选牌
	void Myself();//直接出牌
	void Friend();//跟友方牌
	void Enemy(bool hint);//跟敌方牌
	void NeedSigle();//拆出单张
	void NeedDouble();
	void NeedSigleSeq();
	void NeedThreePlus();
	void NeedAirplane();
	bool Discard(void) override; //AI出牌

};