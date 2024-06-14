#include <map>
#include <set>
#include "cardgroup.h"

CardGroup::CardGroup()
    : type(Unkown)
    , value(0)
    , count(0) {}

CardGroup::CardGroup(Type t, int v)
    : type(t)
    , value(v)
    , count(0) {}

CardGroup &CardGroup::operator=(CardGroup &cg) {
    this->group = cg.group;
    this->cards = cg.cards;
    this->type  = cg.type;
    this->value = cg.value;
    this->count = cg.count;
    return *this;
}

// 重置牌型
void CardGroup::Clear(void) {
    group.clear();
    cards.clear();
    type  = Unkown;
    value = 0;
    count = 0;
    return;
}

// 添加0-53表示的牌
void CardGroup::AddNumber(int num) {
    ++count;
    cards.insert(num);
    ++group[Translate(num)];
}

// 去掉一张牌
void CardGroup::DeleteNumber(int num) {
    if (cards.find(num) == cards.end()) // 确定要去掉的牌在结构内
        return;
    --count;
    cards.erase(num);
    if (--group[Translate(num)] == 0) group.erase(Translate(num));
}

// 把0-53转换成3-17权值，其中A（14）、2（15）、小王（16）、大王（17）
int CardGroup::Translate(int num) {
    if (num < 52)
        return num / 4 + 3;
    else
        return num - 36;
}
