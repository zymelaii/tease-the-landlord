#include <stdexcept>
#include <random>
#include <time.h>
#include "cards.h"

using namespace std;

Cards::Cards() {
    for (int i = 0; i < 54; ++i) cards[i] = i; // 初始化为0-53
    RandCards(); // 洗牌一次并重置牌堆剩余牌数
}

// 洗牌
void Cards::RandCards(void) {
    default_random_engine              e(time(nullptr)); // 随机数引擎
    uniform_int_distribution<unsigned> u(0, 53);         // 整型分布

    for (int i = 0; i < 54; i++) {
        int rand = u(e);
        swap(cards[i], cards[rand]);
    }
    remain = 54;

    return;
}

// 抹牌
int Cards::GetCard(void) {
    if (!remain) throw runtime_error("No more card in cardset!");
    return cards[--remain];
}
