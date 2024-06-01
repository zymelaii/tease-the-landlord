#pragma once

#include <vector>
#include <optional>
#include "game.h"
#include "cardgroup.h"

class Player {
    friend class App;

protected:
    Player(int uid)
        : game_(nullptr)
        , uid_(uid) {}

public:
    Player() = delete;

    //! TODO: 由 app 实现 uid 分配
    //! WARNING: player 应该由 app 构造
    int uid() const {
        return uid_;
    }

    bool join(Game *game) {
        game_ = game;
        return game->add_player(this);
    }

    void accept(Card card) {
        hand_cards_.push_back(card);
    }

    bool is_card_all_out() const {
        return hand_cards_.empty();
    }

    //! FIXME: 无法验证出牌是否合法
    virtual std::optional<CardGroup> make_decision() = 0;
    void                             notify_decision_accpted(CardGroup &cards);

private:
    const Game       *game_;
    std::vector<Card> hand_cards_;
    const int         uid_;
};
