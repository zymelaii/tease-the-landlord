/**************************************************************\
模块：
	类Player -> 斗地主.exe
文件：
	player.cpp
功能：
	用作玩家类，包含：
	1.玩家必须的结构及变量，如：手牌集、最后出牌、分数等；
	2.人工智能相关函数，如：分析手牌、将手牌按基本牌类型拆分及组合、
	  判断叫地主分数、应当出什么牌的判断等。
作者：
	宋保明
修改历史：
	修改人	修改时间	修改内容
	-------	-----------	-------------------------------
	宋保明	2014.12.5	创建
\**************************************************************/

// #include <Windows.h>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include "cardgroup.h"
#include "game.h"
#include "player.h"

using namespace std;

Player::Player(Game &game)
: game(game)
, test(false)
, nodiscard(false)
, score(1000)
{

}
//开始新的一局，做一些初始化集合等的操作
void Player::NewGame()
{
	test = false;
	nodiscard = false;
	cards.clear();
	ClearAnalyse();
	selection.Clear();
	discard.Clear();
}
//清空分析牌集合
void Player::ClearAnalyse()
{
	if (analyse.empty())
		return;

	for (auto mem : analyse)
		delete mem;
	analyse.clear();

	return;
}
//回调函数
bool Player::MyCompare(CardGroup *c1, CardGroup *c2)
{
	if (c1->type != c2->type)
		return c1->type < c2->type;
	else
		return c1->value < c2->value;
}

//分析选牌是否符合规定
bool Player::IsValid()
{
	if (game.lastone && game.lastone->discard.count != selection.count &&
		selection.count != 4 && selection.count != 2)//跟牌，但数量不符且不可能为炸弹
		return false;

	selection.type = Unkown;
	AnalyseSelection();//分析所选牌的类型及权值

	if (selection.type == Unkown)//所选牌不符合规定
		return false;

	if (game.lastone){
		if (selection.type == Bomb &&
			(game.lastone->discard.type != Bomb ||
			selection.value > game.lastone->discard.value))
			return true;
		if (selection.type != game.lastone->discard.type ||
			selection.count != game.lastone->discard.count)//类型不符或数量不符
			return false;
		if (selection.value <= game.lastone->discard.value)//选牌不大于上家牌
			return false;
	}
	return true;
}
//对选牌进行分析
void Player::AnalyseSelection()
{
	int NumMax = 0,//同牌面的最大数量
		ValueMax = 0;//最大数量的最大权值

	//判断是否为王炸
	if (selection.count == 2 &&
		selection.group.find(16) != selection.group.end() &&
		selection.group.find(17) != selection.group.end()){
		selection.type = Bomb;
		selection.value = 17;
		return;
	}
	//找出相同牌面的最大数量，和最大权值
	for (auto mem : selection.group){
		if (mem.second >= NumMax && mem.first > ValueMax){
			NumMax = mem.second;
			ValueMax = mem.first;
		}
	}
	//根据牌面相同的最大数量判断类型
	switch (NumMax){
	case 4:
		if (selection.count == 4){//炸弹
			selection.type = Bomb;
			selection.value = ValueMax;
			return;
		}
		if (selection.count == 6){//四带两张
			selection.type = FourSeq;
			selection.value = ValueMax;
			return;
		}
		if (selection.count == 8){//四带两对
			for (auto mem : selection.group){
				if (mem.second != 2 && mem.second != 4)//牌面不合规
					return;
			}
			selection.type = FourSeq;
			selection.value = ValueMax;
			return;
		}
		return;//牌面不合规
	case 3:
	{
			  if (selection.count == 3){//三条
				  selection.type = Three;
				  selection.value = ValueMax;
				  return;
			  }
			  if (selection.count == 4){//三带一张
				  selection.type = ThreePlus;
				  selection.value = ValueMax;
				  return;
			  }
			  if (selection.count == 5){//三带两张
				  for (auto mem : selection.group){
					  if (mem.second != 3 && mem.second != 2)
						  return;
				  }
				  selection.type = ThreePlus;
				  selection.value = ValueMax;
				  return;
			  }
			  int begin = 0, n = 0;
			  for (auto mem : selection.group){//判断连续的3张牌面的最大数量
				  if (mem.second == 3){
					  if (!begin || begin == mem.first)
						  ++n;
					  if (!begin)
						  begin = mem.first;
					  if (begin != mem.first && n == 1){
						  n = 1;
						  begin = mem.first;
					  }
					  ++begin;
				  }
			  }
			  if (selection.count == 3 * n){//三顺
				  selection.type = ThreeSeq;
				  selection.value = ValueMax;
				  return;
			  }
			  if (selection.count == 4 * n){//飞机带单张的翅膀
				  selection.type = Airplane;
				  selection.value = ValueMax;
				  return;
			  }
			  if (selection.count == 5 * n){//飞机带对子翅膀
				  for (auto mem : selection.group){
					  if (mem.second != 2 && mem.second != 3)//牌不合规
						  return;
				  }
				  selection.type = Airplane;
				  selection.value = ValueMax;
				  return;
			  }
			  return;//牌不合规
	}
	case 2:
		if (selection.count == 2){//一对
			selection.type = Double;
			selection.value = ValueMax;
			return;
		}
		if (selection.count >= 6 && !(selection.count % 2)){//连对
			int begin = 0;
			for (auto mem : selection.group){//确定牌是连续的，并且都是成对的
				if (!begin)
					begin = mem.first;
				if (begin++ != mem.first || mem.second != 2)//牌不符合规定
					return;
			}
			selection.type = DoubleSeq;
			selection.value = ValueMax;
			return;
		}
		return;//牌不符合规定
	case 1:
		if (selection.count == 1){//单张
			selection.type = Single;
			selection.value = ValueMax;
			return;
		}
		else if (selection.count >= 5){//判断是否为顺子
			int begin = 0;
			for (auto mem : selection.group){
				if (!begin)
					begin = mem.first;
				if (begin++ != mem.first || mem.first >= 15)//牌不是连续的或者带了2及以上的牌
					return;
			}
			selection.type = SingleSeq;//单顺
			selection.value = ValueMax;
			return;
		}
	default://下落，不符合规定
		return;
	}

}

//给定权值，从集合中查找相应0-53数字，然后从集合中删除并返回该数字；不存在或无效返回-1
int Player::ValueToNum(set<int> &cardscopy, int value)
{
	if (value<3 || value>17 || cardscopy.empty())
		throw runtime_error("Value not in set!");

	if (value == 16 && cardscopy.find(52) != cardscopy.end()){
		cardscopy.erase(52);
		return 52;
	}
	else if (value == 17 && cardscopy.find(53) != cardscopy.end()){
		cardscopy.erase(53);
		return 53;
	}
	else{
		for (int i = (value - 3) * 4, j = 0; j < 4; ++j){
			if (cardscopy.find(i + j) != cardscopy.end()){
				cardscopy.erase(i + j);
				return i + j;
			}

		}
		throw runtime_error("Value not in set!");
	}
}

//删除分析堆中数量为零的元素
void Player::FreshenMap(map<int, int> &m)
{
	bool notcomplete = true;
	while (notcomplete){
		notcomplete = false;
		auto b = m.begin();
		for (; b != m.end(); ++b){
			if (b->second == 0){
				m.erase(b);
				notcomplete = true;
				break;
			}
		}
	}
}

//出牌并重置分析集合
bool Player::DiscardAndClear()
{
	discard = selection;//把选牌放入出牌区：打出选牌
	bool needclear = true;//本次出牌是否为拆牌，需要更新分析牌堆
	for (auto b = analyse.begin(); b != analyse.end(); ++b){
		if ((*b)->type == selection.type &&
			(*b)->value == selection.value &&
			(*b)->count == selection.count){//不是拆牌
			delete (*b);
			analyse.erase(b);
			needclear = false;//不需要清空
			break;
		}
	}
	if (needclear)//需要清空，下次出牌要重新分析
		ClearAnalyse();

	for (auto mem : selection.cards){
		cards.erase(mem);//从手牌中删除打出牌
	}
	selection.Clear();//清空选牌区
	return true;
}


void Player::Pass(void)
{
	nodiscard = true;
	selection.Clear();
	return;
}