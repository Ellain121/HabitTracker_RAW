#pragma once

#include <QPen>
#include <QStyledItemDelegate>
#include <QtMultimedia/QMediaPlayer>
#include <memory>

class QTableView;
class QTimer;


class HabitDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit HabitDelegate(QTableView* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void startAnimation(const QModelIndex& animationIndx);
    void stopAnimation();

private:
    QPen		mPen;
    QTableView* mTableView;
    qreal		mPValue;
    QTimer*		mAnimationTimer;
    QModelIndex mAnimatedCellIndx;
    int			mAnimatedCellStatus;
};
