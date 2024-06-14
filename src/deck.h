#include <vector>
#include <algorithm>
#include <random>
#include <assert.h>
#include "card.h"

class Deck {
public:
    virtual ~Deck() = default;

    /*!
     * \brief 初始化牌堆
     *
     * 由牌局调用
     */
    virtual void init() = 0;

    void clear() {
        cards_.clear();
    }

    bool empty() const {
        return cards_.empty();
    }

    [[nodiscard]] Card next_card() {
        assert(!cards_.empty());
        const auto card = cards_.back();
        cards_.pop_back();
        return card;
    }

    //! FIXME: 不必要的功能，考虑加入到 init 中
    void shuffle() {
        auto rng = std::default_random_engine(std::random_device{}());
        std::shuffle(cards_.begin(), cards_.end(), rng);
    }

protected:
    void put_card(Card card) {
        cards_.push_back(card);
    }

private:
    std::vector<Card> cards_;
};
