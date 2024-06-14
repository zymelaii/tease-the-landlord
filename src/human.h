#pragma once
#include "player.h"

class Human : public Player {
public:
    Human(Game &game) : Player(game) {}

    int GetBaseScore(int questioned, int nowscore) override;

    bool Discard() override;

    bool IsHuman() const override {
        return true;
    }
};
