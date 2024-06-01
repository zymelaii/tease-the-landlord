#pragma once

#include "cardgroup.h"

//! TODO: 增加特效判定
class Arbiter {
public:
    virtual bool validate(const CardGroup &last_cards, const CardGroup &cards) {
        return cards > last_cards;
    }
};
