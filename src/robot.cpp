#include "robot.h"
#include <map>
#include "cardgroup.h"
#include "game.h"
using namespace std;


//本局是否想当地主，并给出基本分
//叫牌原则参考：http://hi.baidu.com/lvhxwhcuygafpyr/item/1cf4b335205f78627c034ba9
//因为在斗地主中，火箭、炸弹、王和2可以认为是大牌，所以叫牌需要按照这些牌的多少来判断。下面是一个简单的原则：
//假定火箭为8分，炸弹为6分，大王4分，小王3分，一个2为2分，则当分数
//大于等于7分时叫三倍；
//大于等于5分时叫二倍；
//大于等于3分时叫一倍；
//小于三分不叫。
int Robot::GetBaseScore(int questioned, int nowscore){
	if (questioned == 2 && nowscore == 0)//如果前两位都未叫牌，直接3分当地主，你懂得~
		return 3;

	int sum = 0;
	map<int, int> needanalyse;//方便分析的权值-数量集合
	for (auto mem : cards)
		++needanalyse[CardGroup::Translate(mem)];//根据手牌构造待分析集合

	if (needanalyse.find(16) != needanalyse.end() &&
		needanalyse.find(17) != needanalyse.end())//存在王炸
		sum += 8;
	else if (needanalyse.find(16) != needanalyse.end())//一张小王
		sum += 3;
	else if (needanalyse.find(17) != needanalyse.end())//一张大王
		sum += 4;

	if (needanalyse.find(15) != needanalyse.end())//2的数量
		sum += 2 * needanalyse[15];

	for (auto mem : needanalyse){
		if (mem.second == 4)//炸弹
			sum += 6;
	}
	int result;
	if (sum >= 7)
		result = 3;
	else if (sum >= 5 && sum < 7)
		result = 2;
	else if (sum >= 3 && sum < 5)
		result = 1;
	else
		result = 0;
	return (result > nowscore ? result : 0);
}

//拆分手牌牌型并组成基本牌集合
void Robot::DivideIntoGroups(void)
{
	if (analyse.size())//牌型集合非空，返回
		return;

	set<int> cardscopy(cards);//手牌副本
	map<int, int> needanalyse;//方便分析的权值-数量集合

	for (auto mem : cardscopy)
		++needanalyse[CardGroup::Translate(mem)];//根据手牌构造待分析集合

	if (needanalyse.find(16) != needanalyse.end() &&
		needanalyse.find(17) != needanalyse.end()){//满足条件存在王炸
		CardGroup *c = new CardGroup(Bomb, 17);
		for (int i = 16; i < 18; ++i){
			c->AddNumber(ValueToNum(cardscopy, i));
			needanalyse.erase(i);
		}
		analyse.push_back(c);
	}

	for (auto mem : needanalyse){
		if (mem.second == 4){	//炸弹
			CardGroup *c = new CardGroup(Bomb, mem.first);
			for (int i = 0; i < 4; ++i){
				c->AddNumber(ValueToNum(cardscopy, mem.first));
			}
			analyse.push_back(c);
			needanalyse[mem.first] = 0;
		}
	}
	//删除分析堆中数量为零的元素
	FreshenMap(needanalyse);

	//提前处理2
	if (needanalyse.find(15) != needanalyse.end()){
		CardGroup *c = new CardGroup(Unkown, 15);
		int n = needanalyse[15];
		switch (n){
		case 3:
			c->type = Three;
			break;
		case 2:
			c->type = Double;
			break;
		case 1:
			c->type = Single;
			break;
		}
		for (int i = 0; i < n; ++i)
			c->AddNumber(ValueToNum(cardscopy, 15));
		needanalyse.erase(15);
		analyse.push_back(c);
	}
	//查找单顺
	int begin, n;
	bool exist = true;
	while (exist && needanalyse.size()){
		begin = n = 0;
		for (auto b = needanalyse.begin(); b != needanalyse.end(); ++b){
			if (b->second > 0){//跳过为零的元素
				if (!begin)
					begin = b->first;
				if (begin == b->first)
					++n;
				++begin;
			}
			if (n == 5){//满足组成单顺的数量
				auto p = b;
				int first = p->first - 4;//单顺的第一个
				CardGroup *c = new CardGroup(SingleSeq, p->first);
				for (first; first <= p->first; ++first){
					c->AddNumber(ValueToNum(cardscopy, first));
					--needanalyse[first];//减一
				}
				analyse.push_back(c);
				exist = true;
				break;//从开始重新查找
			}
			//连续牌面数量小于五个，重新计数；或者已到集合最后数量仍不满足
			auto end = needanalyse.end();
			if (begin - 1 != b->first || b == --end){
				if (b->second > 0){
					begin = b->first;
					++begin;
					n = 1;
				}
				else
					begin = n = 0;
				exist = false;
			}

		}
	}

	//删除分析堆中数量为零的元素
	FreshenMap(needanalyse);
	//如可能，继续往单顺中添加剩余牌
	for (auto mem : analyse){
		if (mem->type == SingleSeq){//针对每个单顺
			for (auto m : needanalyse){
				if (m.second > 0 && m.first == mem->value + 1){//剩余牌中还有比单顺最大大一的牌
					mem->AddNumber(ValueToNum(cardscopy, m.first));
					++mem->value;
					--needanalyse[m.first];

				}
			}
		}
	}
	//删除分析堆中数量为零的元素
	FreshenMap(needanalyse);

	//如现有单顺中有可以对接成更长的单顺；或两个单顺元素相同，组合成双顺
	for (auto mem1 : analyse){
		if (mem1->type == SingleSeq){//单顺1
			for (auto mem2 : analyse){
				if (mem2->type == SingleSeq && mem1 != mem2){//单顺2，且和单顺1不是同一个
					if (mem1->value < mem2->value){//mem1在前
						if (mem1->value == mem2->value - mem2->count){//可以拼接
							for (auto m : mem2->cards)
								mem1->AddNumber(m);
							mem1->value = mem2->value;
							mem2->type = Unkown;
						}
					}
					else if (mem1->value > mem2->value){//mem1在后
						if (mem2->value == mem1->value - mem1->count){
							for (auto m : mem1->cards)
								mem2->AddNumber(m);
							mem2->value = mem1->value;
							mem1->type = Unkown;
						}
					}
					else{//测试是否完全一样，可以合并成双顺
						if (mem1->count == mem2->count){
							for (auto m : mem2->cards)
								mem1->AddNumber(m);
							mem1->type = DoubleSeq;
							mem2->type = Unkown;
						}
					}
				}
			}
		}
	}
	if (needanalyse.empty()){//分析集合已空，返回
		DeleteUnkown();
		sort(analyse.begin(), analyse.end(), MyCompare);
		return;
	}

	//双顺，只查找数量大于等于2的连续牌，并且3个以上相连
	begin = n = 0;
	auto last = --needanalyse.end();
	for (auto b = needanalyse.begin(); b != needanalyse.end(); ++b){
		if (b->second >= 2){
			if (!begin)
				begin = b->first;
			if (begin == b->first)
				++n;
			++begin;
		}
		if (begin && begin - 1 != b->first || b == last){//出现与之前不连续的,或已到集合最后
			if (n >= 3){
				auto p = b;
				if (begin - 1 != b->first)
					--p;
				CardGroup *c = new CardGroup(DoubleSeq, p->first);
				for (int i = n; i > 0; --i, --p){
					for (int j = 0; j < 2; ++j){
						c->AddNumber(ValueToNum(cardscopy, p->first));
						--p->second;
					}
				}
				analyse.push_back(c);
			}
			if (b->second >= 2){
				n = 1;//当前分析牌是两张以上的
				begin = b->first;
				++begin;
			}
			else{
				n = 0;
				begin = 0;
			}
		}
	}

	//删除分析堆中数量为零的元素
	FreshenMap(needanalyse);

	//三顺
	//查找是否有重合的单顺和双顺组合成三顺
	for (auto mem1 : analyse){
		if (mem1->type == SingleSeq){
			for (auto mem2 : analyse){
				if (mem2->type == DoubleSeq){
					if (mem1->value == mem2->value && mem1->count * 2 == mem2->count){
						for (auto m : mem1->cards)
							mem2->AddNumber(m);
						mem2->type = ThreeSeq;
						mem1->type = Unkown;
					}
				}
			}
		}
	}

	if (needanalyse.empty()){
		DeleteUnkown();
		sort(analyse.begin(), analyse.end(), MyCompare);
		return;
	}
	//剩余牌中查找三顺
	begin = n = 0;
	last = --needanalyse.end();
	for (auto b = needanalyse.begin(); b != needanalyse.end(); ++b){
		if (b->second == 3){
			if (!begin)
				begin = b->first;
			if (begin == b->first)
				++n;
			++begin;
		}
		if (begin && begin - 1 != b->first || b == last){//出现与之前不连续的,或已到集合最后
			if (n >= 2){//存在2组及以上
				auto p = b;
				if (begin - 1 != b->first)
					--p;
				CardGroup *c = new CardGroup(ThreeSeq, p->first);
				for (int i = n; i > 0; --i, --p){
					for (int j = 0; j < 3; ++j){
						c->AddNumber(ValueToNum(cardscopy, p->first));
						--p->second;
					}
				}
				analyse.push_back(c);
				if (b->second == 3){//当前分析牌为3张，
					n = 1;
					begin = b->first;
					++begin;
				}
				else{
					n = 0;
					begin = 0;
				}
			}
		}
	}
	//三条
	for (auto mem : needanalyse){
		if (mem.second == 3){
			CardGroup *c = new CardGroup(Three, mem.first);
			for (int i = 0; i < 3; ++i)
				c->AddNumber(ValueToNum(cardscopy, mem.first));
			needanalyse[mem.first] = 0;
			analyse.push_back(c);
		}
	}

	//对子
	for (auto mem : needanalyse){
		if (mem.second == 2){
			CardGroup *c = new CardGroup(Double, mem.first);
			for (int i = 0; i < 2; ++i)
				c->AddNumber(ValueToNum(cardscopy, mem.first));
			needanalyse[mem.first] = 0;
			analyse.push_back(c);
		}
	}
	//删除分析堆中数量为零的元素
	FreshenMap(needanalyse);

	//单牌
	for (auto mem : needanalyse){
		if (mem.second != 1)
			throw runtime_error("Still has singleness card");
		CardGroup *c = new CardGroup(Single, mem.first);
		c->AddNumber(ValueToNum(cardscopy, mem.first));
		needanalyse[mem.first] = 0;
		analyse.push_back(c);
	}
	//删除分析堆中数量为零的元素
	FreshenMap(needanalyse);

	DeleteUnkown();
	sort(analyse.begin(), analyse.end(), MyCompare);

}

void Robot::ThreeplusAndAirplane()
{
	int n,
		doublecount = 0,//统计对子的数量，方便下面的整合
		singlecount = 0;//统计单张数量

	for (auto mem : analyse){
		if (mem->type == Single)
			++singlecount;
		else if (mem->type == Double)
			++doublecount;
	}

	for (auto mem : analyse){//完善飞机
		if (mem->type == ThreeSeq){
			n = mem->count / 3;
			if (singlecount >= n){
				for (auto temp : analyse){
					if (temp->type == Single){
						for (auto m : temp->cards)
							mem->AddNumber(m);
						temp->type = Unkown;
						--singlecount;
						--n;
					}
					if (!n){
						mem->type = Airplane;
						break;
					}
				}
			}
			else if (doublecount >= n){
				for (auto temp : analyse){
					if (temp->type == Double){
						for (auto m : temp->cards)
							mem->AddNumber(m);
						temp->type = Unkown;
						--doublecount;
						--n;
					}
					if (!n){
						mem->type = Airplane;
						break;
					}
				}
			}
		}
	}
	for (auto mem : analyse){//完善三带一
		if (mem->type == Three){
			if (singlecount){
				for (auto temp : analyse){
					if (temp->type == Single){
						for (auto m : temp->cards)
							mem->AddNumber(m);
						temp->type = Unkown;
						--singlecount;
						mem->type = ThreePlus;
						break;
					}
				}
			}
			else if (doublecount){
				for (auto temp : analyse){
					if (temp->type == Double){
						for (auto m : temp->cards)
							mem->AddNumber(m);
						temp->type = Unkown;
						--doublecount;
						mem->type = ThreePlus;
						break;
					}
				}
			}
		}
	}
}

//电脑选牌
void Robot::SelectCards(bool hint)
{
	if (analyse.empty())//是否需要重新分析手牌
		DivideIntoGroups();
	ThreeplusAndAirplane();
	DeleteUnkown();
	sort(analyse.begin(), analyse.end(), MyCompare);

	if (analyse.size() == 2){//手数为2，且有适合的炸弹直接出
		for (auto mem : analyse){
			if (mem->type == Bomb){
				if (game.lastone != nullptr &&//如果自己是接别人的牌
					game.lastone->discard.type == Bomb &&//别人最后出牌为炸弹，
					mem->value <= game.lastone->discard.value)//且自己的炸弹不大于对方时，
					continue;//不能选择改牌
				selection = *mem;
				return;
			}
		}
	}
	
	if (game.lastone == nullptr)
		Myself();//直接出牌
	else if (!hint && this != game.landlord && game.lastone != game.landlord)
		Friend();//跟友方牌：最后出牌的是友方,并且不是提示
	else
		Enemy(hint);//跟敌方的牌或提示
}

void Robot::Myself()
{
	if (analyse.size() == 1){//剩最后一手牌
		selection = *analyse[0];
		return;
	}

	if (analyse.size() == 2){//剩两手牌，出最大的那组
		//“查看”其它玩家手牌，只为分析剩余牌中的最大的
		int maxNum = 0;
		Player *p = game.ProPlayer();
		if (*p->cards.rbegin() > maxNum)
			maxNum = *p->cards.rbegin();
		p = game.NextPlayer();
		if (*p->cards.rbegin() > maxNum)
			maxNum = *p->cards.rbegin();
		for (auto mem : analyse){//如果手中有比剩余牌还大的一手牌，就先打出该牌
			if (mem->value > CardGroup::Translate(maxNum)){
				selection = *mem;
				return;
			}
		}
		//否则，打出牌类型最大的牌
		selection = *analyse[1];
		return;
	}
	if (game.NextPlayer()->cards.size() == 1){//下家手牌数为1
		if (this != game.landlord && game.lastone != game.landlord){//下家为友方
			//没试验过下家牌，就打出最小的一张；否则就正常出牌
			if (!test){
				if (analyse[0]->type == Single &&
					analyse[0]->value == CardGroup::Translate(*(cards.begin()))){
					selection = *analyse[0];
					return;
				}
				else{
					selection.AddNumber(*(cards.begin()));
					selection.type = Single;
					selection.value = selection.group.begin()->first;
					//拆牌了！要重新分析牌
					ClearAnalyse();
					return;
				}

			}

		}
		else{//下家为敌方剩1牌
			//待实现！！
			//思路是尽量不出单牌，只有单牌了的话就从大到小出
		}
	}
	//正常顺序出牌：(A以上的牌尽量不直接出、炸弹不直接出)
	//单牌→对子→双顺→单顺→三条、三带一、飞机
	for (auto mem : analyse){
		if ((mem->type == Single || mem->type == Double) &&
			mem->value >= 15 || mem->type == Bomb)
			continue;
		selection = *mem;
		return;
	}
	selection = *analyse[0];
	return;
}

void Robot::Friend()
{
	if (game.lastone != game.landlord && game.ProPlayer() == game.landlord){
		return;//上家为地主，但最后出牌方为友方，则不出牌
	}
	for (auto mem : analyse){//查找相应牌
		if (mem->type == game.lastone->discard.type &&
			mem->count == game.lastone->discard.count &&
			mem->value > game.lastone->discard.value){

			selection = *mem;
			break;
		}
	}
	if (analyse.size() > 2 && selection.value > 14)
		selection.Clear();//手牌手数大于2，并且所选牌权值大于14（A），则不出牌
	return;
}


void Robot::Enemy(bool hint)
{
	auto lastdiscard = game.lastone->discard;//敌方出牌

	//拆成基本牌
	ClearAnalyse();
	DivideIntoGroups();
	sort(analyse.begin(), analyse.end(), MyCompare);

	for (auto mem : analyse){//查看是否有相应牌，并且权值大
		if (mem->type == lastdiscard.type &&
			mem->count == lastdiscard.count &&
			mem->value > lastdiscard.value){

			selection = *mem;
			return;
		}
	}
	//需要拆牌
	switch (lastdiscard.type){
	case Single://敌方出的是单牌
		NeedSigle();
		break;
	case Double:
		NeedDouble();
		break;
	case SingleSeq:
		NeedSigleSeq();
		break;
	case Three:
		break;
	case ThreePlus://三带一
		NeedThreePlus();
		break;
	case Airplane://飞机，需要组合
		NeedAirplane();
		break;
	default:
		break;
	}
	if (selection.count)
		return;
	//敌方剩一张牌，或有适合的炸弹，就出炸弹
	if (hint || lastdiscard.count > 3 || lastdiscard.value > 14){
		for (auto mem : analyse){
			if (mem->type == Bomb){
				if (game.lastone->discard.type == Bomb &&//如果别人最后出牌为炸弹，
					mem->value <= game.lastone->discard.value)//且自己的炸弹不大于对方时，
					continue;//不能选择改牌
				selection = *mem;
				return;
			}
		}
	}
	return;
}

void Robot::NeedSigle()
{
	auto lastdiscard = game.lastone->discard;//敌方出牌

	for (auto mem : analyse){
		if (mem->type == SingleSeq && mem->count > 5){//首先,拆单顺数量大于5的
			if (mem->group.begin()->first > lastdiscard.value){
				selection.AddNumber(*mem->cards.begin());
				selection.value = mem->group.begin()->first;
				selection.type = Single;
				ClearAnalyse();//拆牌了，一定要清空
				return;
			}
			else if (mem->group.rbegin()->first > lastdiscard.value){
				selection.AddNumber(*(mem->cards.rbegin()));
				selection.value = mem->value;
				selection.type = Single;
				ClearAnalyse();//拆牌了，一定要清空
				return;
			}
		}
	}
	for (auto mem : analyse){
		if (mem->type == Three){//其次,拆三条
			if (mem->group.begin()->first > lastdiscard.value){
				selection.AddNumber(*mem->cards.begin());
				selection.value = mem->group.begin()->first;
				selection.type = Single;
				ClearAnalyse();//拆牌了，一定要清空
				return;
			}
		}
	}
	for (auto mem : analyse){
		if (mem->type == Double){//再者,拆对子
			if (mem->group.begin()->first > lastdiscard.value){
				selection.AddNumber(*mem->cards.begin());
				selection.value = mem->group.begin()->first;
				selection.type = Single;
				ClearAnalyse();//拆牌了，一定要清空
				return;
			}
		}
	}
}

void Robot::NeedDouble()
{
	auto lastdiscard = game.lastone->discard;//敌方出牌

	for (auto mem : analyse){
		if (mem->type == Three){//拆三条
			if (mem->group.begin()->first > lastdiscard.value){
				auto b = mem->cards.begin();
				for (int i = 0; i < 2; ++i)
					selection.AddNumber(*b++);
				selection.value = mem->group.begin()->first;
				selection.type = Double;
				ClearAnalyse();//拆牌了，一定要清空
				return;
			}
		}
	}
	for (auto mem : analyse){
		int i = 0, m = 0;
		if (mem->type == ThreeSeq){//拆三顺
			if (mem->group.begin()->first > lastdiscard.value){
				auto b = mem->cards.begin();
				for (int i = 0; i < 2; ++i)
					selection.AddNumber(*b++);
				selection.value = mem->group.begin()->first;
				selection.type = Double;
				ClearAnalyse();//拆牌了，一定要清空
				return;
			}
			else if (mem->group.rbegin()->first > lastdiscard.value){
				selection.AddNumber(*(mem->cards.rbegin()));
				selection.value = mem->value;
				selection.type = Double;
				ClearAnalyse();//拆牌了，一定要清空
				return;
			}
		}
	}
}

void Robot::NeedSigleSeq()
{
	auto lastdiscard = game.lastone->discard;//敌方出牌

	for (auto mem : analyse){
		if (mem->type == SingleSeq &&
			mem->value > lastdiscard.value &&
			mem->count > lastdiscard.count){//拆更长的单顺
			if (mem->count - (mem->value - lastdiscard.value) >= lastdiscard.count){
				//长单顺是从短单顺的开始的元素或更小的元素开始的
				for (int i = lastdiscard.value - lastdiscard.count + 2, j = 0;
					j < lastdiscard.count; ++j)
					selection.AddNumber(ValueToNum(mem->cards, i + j));
				selection.value = lastdiscard.value + 1;
				selection.type = SingleSeq;
				ClearAnalyse();//拆牌了，一定要清空
				return;
			}
			else{//长单顺的开始元素比短单顺的开始元素大
				int i = 0;
				auto b = mem->cards.begin();
				for (; i < lastdiscard.count; ++i, ++b)
					selection.AddNumber(*b);
				selection.value = CardGroup::Translate(*--b);
				selection.type = SingleSeq;
				ClearAnalyse();//拆牌了，一定要清空
				return;
			}
		}
	}
}

void Robot::NeedThreePlus()
{
	auto b = analyse.begin();
	for (; b != analyse.end(); ++b){//查找比对方三张相同牌的牌面大的三条
		if ((*b)->type == Three && (*b)->value > game.lastone->discard.value){
			break;
		}
	}
	if (b == analyse.end())//如果没有
		return;//跳出
	if (game.lastone->discard.count == 4){//最后出牌为三带一张
		if (analyse[0]->type == Single){//有单牌
			for (auto m : analyse[0]->cards)
				(*b)->AddNumber(m);
			(*b)->type = ThreePlus;
			analyse[0]->type = Unkown;
			selection = **b;
			return;
		}
		else{//需要拆牌
			for (auto mem : analyse){
				if (mem->type == SingleSeq && mem->count > 5){//首先,拆单顺数量大于5的
					selection = **b;
					selection.AddNumber(*mem->cards.begin());
					selection.type = ThreePlus;
					ClearAnalyse();//拆牌了，一定要清空
					return;
				}
			}
			for (auto mem : analyse){
				if (mem->type == Three && mem != *b && mem->value < 14){//其次,拆三条
					selection = **b;
					selection.AddNumber(*mem->cards.begin());
					selection.type = ThreePlus;
					ClearAnalyse();//拆牌了，一定要清空
					return;
				}
			}
			for (auto mem : analyse){
				if (mem->type == Double && mem->value < 14){//再者,拆对子
					selection = **b;
					selection.AddNumber(*mem->cards.begin());
					selection.type = ThreePlus;
					ClearAnalyse();//拆牌了，一定要清空
					return;
				}
			}
		}
	}
	else{//三带一对
		for (auto mem : analyse){//先找对子
			if (mem->type == Double && mem->value < 14){
				for (auto m : mem->cards)
					(*b)->AddNumber(m);
				(*b)->type = ThreePlus;
				mem->type = Unkown;
				selection = **b;
				return;
			}
		}
		for (auto mem : analyse){
			if (mem->type == Three && mem != *b && mem->value < 14){//其次,拆三条
				selection = **b;
				for (int i = 0; i < 3; ++i)
					selection.AddNumber(*mem->cards.begin());
				selection.type = ThreePlus;
				ClearAnalyse();//拆牌了，一定要清空
				return;
			}
		}
	}
}

void Robot::NeedAirplane()
{
	ClearAnalyse();
	DivideIntoGroups();
	sort(analyse.begin(), analyse.end(), MyCompare);

	int wing = 0,//翅膀类型
		n = 0;//单顺中三张牌的个数
	for (auto mem : game.lastone->discard.group){
		if (mem.second == 3)
			++n;
	}
	if (game.lastone->discard.count == 5 * n)//飞机翅膀为对子
		wing = 2;
	else{//飞机翅膀为单张
		while (game.lastone->discard.count != 4 * n)
			--n;
		wing = 1;
	}
	auto b = analyse.begin();
	for (; b != analyse.end(); ++b){
		if ((*b)->type == ThreeSeq &&
			(*b)->count == 3 * n &&
			(*b)->value > game.lastone->discard.value)
			break;
	}
	if (b == analyse.end())
		return;
	int count = 0;
	for (auto mem : analyse){
		if (mem->type == (wing == 1 ? Single : Double))
			++count;
	}
	if (count < n)
		return;
	for (auto mem : analyse){
		if (mem->type == (wing == 1 ? Single : Double)){
			for (auto m : mem->cards)
				(*b)->AddNumber(m);
			mem->type = Unkown;
			--n;
		}
		if (!n)
			break;
	}
	(*b)->type = Airplane;
	selection = **b;
	return;
}

//电脑出牌
bool Robot::Discard(void)
{
    SelectCards();
	if (selection.count == 0){//电脑选牌区为空，说明不出
		nodiscard = true;
		return false;
	}
	//否则正常打出
	return DiscardAndClear();
}