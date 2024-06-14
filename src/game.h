#pragma once

#include <memory>
#include <array>
#include <vector>
#include <assert.h>
#include "cardgroup.h"

class Player;
class Deck;
class Arbiter;

enum PlayAction {
    Play,
    Skip,
};

class Game {
public:
    //! NOTE: 记录具体牌组对于机器玩家的优势过于巨大
    //! TODO: 用牌面分值代替记录牌组会缺失实际的出牌，玩家将无法通
    //! 过遍历牌组反推对手的出牌，关于是否使用实际出牌待定
    struct CardPlayRecordRef {
        const Player& player;
        int           card_score;
        //! FIXME: 临时修一下
        CardGroup card_group;
    };

public:
    Game(Deck* deck, Arbiter* arbiter);
    virtual ~Game() = default;

    int  total_players() const;
    bool add_player(Player* player);

    bool is_playing() const {
        return on_play_;
    }

    void start();

    int total_turns() const {
        return turn_stack_.size();
    }

    const CardPlayRecordRef& last_play_record() const {
        assert(total_turns() > 0);
        return turn_stack_.back();
    }

    std::vector<CardPlayRecordRef>& play_records() {
        return turn_stack_;
    }

    void accept(Player* player, CardGroup card_group) {
        assert(std::count(players_.begin(), players_.end(), player) == 1);
        turn_stack_.push_back(
            {*player, CardGroup::score(card_group), card_group});
        next_turn();
    }

    void skip() {
        next_turn();
    }

    bool try_handle_current_turn();

    void finish();

    //! NOTE: -1 为非法 uid
    int last_winner() const {
        return last_winner_;
    }

protected:
    void next_turn();

private:
    std::unique_ptr<Deck>                  deck_;
    std::unique_ptr<Arbiter>               arbiter_;
    std::array<std::unique_ptr<Player>, 4> players_;
    std::vector<CardPlayRecordRef>         turn_stack_;
    bool                                   on_play_;
    int                                    player_index_;
    int                                    current_total_players_;
    int                                    last_winner_;
};
