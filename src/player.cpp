#include "player.h"
#include <algorithm>
#include <utility>

void Player::notify_decision_accpted(CardGroup &cards) {
    for (const auto &card : cards.cards()) {
        if (auto it = std::find(hand_cards_.begin(), hand_cards_.end(), card);
            it != hand_cards_.end()) {
            hand_cards_.erase(it);
        } else {
            std::unreachable();
        }
    }
}
