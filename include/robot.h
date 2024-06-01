#include<vector>
#include"player.h"
#include"handtype.h"
class Robot : public Player {
public:
    Robot(const std::string& name) : Player(name) {}

    // 重写playCard方法，实现自动出牌逻辑
    HandType playCard(const HandType currentHand);
};