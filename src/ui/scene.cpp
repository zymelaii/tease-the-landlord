#include "scene.h"
#include <QPainter>
#include <QMouseEvent>

namespace ui {

QString Scene::get_card_name(Card card) {
    if (card.suit == Card::JOKER) {
        return card.rank == Card::BIG_JOKER ? "Joker-R" : "Joker-B";
    }
    static QMap<Card::Rank, QString> RANK_TABLE{
        {Card::KING,  "K" },
        {Card::QUEEN, "Q" },
        {Card::JACK,  "J" },
        {Card::TEN,   "10"},
        {Card::NINE,  "9" },
        {Card::EIGHT, "8" },
        {Card::SEVEN, "7" },
        {Card::SIX,   "6" },
        {Card::FIVE,  "5" },
        {Card::FOUR,  "4" },
        {Card::THREE, "3" },
        {Card::TWO,   "2" },
        {Card::ACE,   "A" },
    };
    static QMap<Card::Suit, int> SUIT_TABLE{
        {Card::CLUBS,    1},
        {Card::DIAMONDS, 2},
        {Card::HEARTS,   3},
        {Card::SPADES,   4},
    };
    Q_ASSERT(RANK_TABLE.contains(card.rank) && SUIT_TABLE.contains(card.suit));
    return QString("%1-%2")
        .arg(RANK_TABLE.value(card.rank))
        .arg(SUIT_TABLE.value(card.suit));
}

Scene::Scene(QWidget* parent)
    : QWidget(parent) {
    const QPixmap sample_card(":/card/back");
    const int     sample_card_width   = sample_card.width();
    const int     sample_card_height  = sample_card.height();
    const int     expected_card_width = 100;
    const double  ratio = expected_card_width * 1.0 / sample_card_width;
    const int     expecetd_card_height = ratio * sample_card_height;
    card_size_ = QSize(expected_card_width, expecetd_card_height);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QSize Scene::minimumSizeHint() const {
    return QRect(QPoint(0, 0), QSize(1200, 800))
        .marginsAdded(contentsMargins())
        .size();
}

QSize Scene::sizeHint() const {
    return minimumSizeHint();
}

void Scene::paintEvent(QPaintEvent* event) {
    if (cards.empty()) { return; }

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int  margin          = 32;
    const int  selected_offset = card_size_.height() / 8;
    const int  spacing         = card_size_.width() / 5;
    const auto bb              = contentsRect();
    const int  cards_width = card_size_.width() + (cards.size() - 1) * spacing;
    int        pos_x       = bb.left() + (bb.width() - cards_width) / 2;
    int        pos_y       = bb.bottom() - margin - card_size_.height();
    const int  human_player_reserved = bb.bottom() - pos_y;

    for (int i = 0; i < cards.size(); ++i) {
        const auto& card = cards[i];
        QPixmap     card_image(QString(":/card/%1").arg(get_card_name(card)));
        Q_ASSERT(!card_image.isNull());
        p.drawPixmap(
            QPoint(
                pos_x,
                selected_cards_.count(i) ? pos_y - selected_offset : pos_y),
            card_image.scaled(
                card_size_, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        pos_x += spacing;
    }

    auto back_image =
        QPixmap(":/card/back")
            .scaled(card_size_, Qt::KeepAspectRatio, Qt::SmoothTransformation)
            .transformed(QTransform().rotate(90));

    pos_x = bb.left() + margin;
    pos_y = bb.top()
          + (bb.height() - human_player_reserved
             - (spacing * (player_left_total_cards - 1) + card_size_.width()))
                / 2;
    for (int i = 0; i < player_left_total_cards; ++i) {
        p.drawPixmap(QPoint(pos_x, pos_y), back_image);
        pos_y += spacing;
    }

    pos_x = bb.right() - margin - card_size_.height();
    pos_y = bb.top()
          + (bb.height() - human_player_reserved
             - (spacing * (player_right_total_cards - 1) + card_size_.width()))
                / 2;
    back_image = back_image.transformed(QTransform().scale(-1, 1));
    for (int i = 0; i < player_right_total_cards; ++i) {
        p.drawPixmap(QPoint(pos_x, pos_y), back_image);
        pos_y += spacing;
    }
}

void Scene::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) { return; }

    const int  margin          = 32;
    const int  selected_offset = card_size_.height() / 8;
    const int  spacing         = card_size_.width() / 5;
    const auto bb              = contentsRect();
    const int  cards_width = card_size_.width() + (cards.size() - 1) * spacing;
    int        pos_x       = bb.left() + (bb.width() - cards_width) / 2;
    int        pos_y       = bb.bottom() - margin - card_size_.height();

    const QRect cards_bb(
        pos_x,
        pos_y - selected_offset,
        cards_width,
        card_size_.height() + selected_offset);

    if (!cards_bb.contains(event->pos())) { return; }
    if (cards.empty()) { return; }

    const int index =
        qBound<int>(0, (event->pos().x() - pos_x) / spacing, cards.size() - 1);

    bool hit_test_result = false;

    if (event->pos().y() < pos_y) {
        hit_test_result = selected_cards_.count(index);
    } else if (
        event->pos().y() > pos_y - selected_offset + card_size_.height()) {
        hit_test_result = !!selected_cards_.count(index);
    } else {
        hit_test_result = true;
    }

    if (hit_test_result) {
        if (selected_cards_.contains(index)) {
            selected_cards_.erase(index);
        } else {
            selected_cards_.insert(index);
        }
        update();
    }
}

} // namespace ui
