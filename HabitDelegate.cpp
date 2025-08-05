#include "HabitDelegate.hpp"

#include "Constants.hpp"
#include "HabitModel.hpp"
#include "SoundPlayer.hpp"
#include "Utility.hpp"

#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QTableView>
#include <QTimer>
#include <QUrl>

namespace
{
const qreal cricleRectSize = 17;
const qreal pStartValue = 0.0;
const qreal pMaxValue = 1.0;
const qreal pIncreaseStep = 0.02;
}	 // namespace

HabitDelegate::HabitDelegate(QTableView* parent)
	: QStyledItemDelegate{parent}
	, mPValue{pStartValue}
	, mAnimationTimer{new QTimer{this}}
	//    , mSoundPlayer{std::make_unique<QMediaPlayer>()}
	, mAnimatedCellIndx{QModelIndex{}}
	, mAnimatedCellStatus{-1}
{
	int gridHint = parent->style()->styleHint(
		QStyle::SH_Table_GridLineColor, new QStyleOptionViewItem{});
	QColor gridColor = static_cast<QRgb>(gridHint);
	mPen = QPen{gridColor, 0, parent->gridStyle()};

	mTableView = parent;

	connect(mAnimationTimer, &QTimer::timeout, this,
		[this]()
		{
			if (mPValue >= pMaxValue)
			{
				if (mAnimatedCellStatus == 0)
				{
					SoundPlayer::playSound(SoundPlayer::Type::Success);
				}
				mTableView->model()->setData(
					mAnimatedCellIndx, QVariant{}, HabitModel::Roles::HabitStatusRole);
				stopAnimation();
				return;
			}
			mPValue += pIncreaseStep;
			mTableView->viewport()->repaint();
		});
}

void HabitDelegate::paint(
	QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	assert(index.isValid());

	/*** Draw Horizontal Lines ***/
	QPen oldPen = painter->pen();
	painter->setPen(mPen);
	// draw horizontalLine
	painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
	// above code, line have breakpoint, the following code can solve it well
	painter->setPen(oldPen);
	/************************************/

	// If first column, then we just draw a name, without possibility of an animation
	if (index.column() == 0)
	{
		QStyledItemDelegate::paint(painter, option, index);
		return;
	}

	painter->save();
	const QRect	 r = option.rect;
	QPixmap		 pix = index.data(Qt::DecorationRole).value<QPixmap>();
	const QPoint midPoint =
		QPoint((r.width() - pix.width()) / 2, (r.height() - pix.height()) / 2);

	/*** If mouse pressed on current index, draw an animation of progress ***/
	if (mAnimatedCellIndx != QModelIndex{} && index == mAnimatedCellIndx)
	{
		qreal  p = mPValue;
		qreal  pd = p * 360;
		qreal  rd = 360 - pd;
		QColor progressColor =
			index.model()->data(index, HabitModel::HabitStatusRole) == 0
				? QColor{"#4CBB17"}
				: QColor{"#CE084E"};
		QPoint pos = r.topLeft() + midPoint +
					 QPoint{static_cast<int>(cricleRectSize / 2.5 - 2),
						 static_cast<int>(cricleRectSize / 2.5 - 1)};

		painter->translate(pos.x(), pos.y());
		painter->setRenderHint(QPainter::Antialiasing);

		QPainterPath path1, path2;
		path1.moveTo(cricleRectSize / 2, 0);
		path1.arcTo(QRectF(0, 0, cricleRectSize, cricleRectSize), 90, -pd);

		path2.moveTo(cricleRectSize / 2, 0);
		path2.arcTo(QRectF(0, 0, cricleRectSize, cricleRectSize), 90, rd);

		QPen pen1, pen2;
		pen1.setCapStyle(Qt::FlatCap);
		pen1.setColor(progressColor);
		pen1.setWidth(static_cast<int>(cricleRectSize / 2.5));
		painter->strokePath(path1, pen1);

		pen2.setWidth(static_cast<int>(cricleRectSize / 2.5));
		pen2.setColor(QColor{"#d7d7d7"});
		pen2.setCapStyle(Qt::FlatCap);
		painter->strokePath(path2, pen2);
	}
	else	// set Icons at the center of the cell
	{
		Habit::Type habitType =
			static_cast<Habit::Type>(toInt(index.data(HabitModel::Roles::TypeRole)));
		int		unitsAmount = toInt(index.data(HabitModel::Roles::HabitStatusRole));
		QString units = toQString(index.data(HabitModel::Roles::UnitsRole));

		// For one time habits (or for quantative ones, but only when minus sign required)
		if (habitType == Habit::Type::OneTime || unitsAmount == INT_NULL_VALUE)
		{
			QStyleOptionViewItem opt = option;
			initStyleOption(&opt, index);
			// disable default icon
			opt.icon = QIcon{};
			// draw default item
			QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, 0);

			// draw pixmap at center of item
			painter->drawPixmap(r.topLeft() + midPoint, pix);
		}
		else	// for the quantative habits
		{
			int		dailyGoal = toInt(index.data(HabitModel::Roles::DailyGoalRole));
			QString unitsAmountStr = QString::number(unitsAmount);
			// previous code only draws blue ellipses with white background
			QColor secondColor(240, 0, 0);

			QPen colorPen = painter->pen();
			colorPen.setColor(
				unitsAmount >= dailyGoal ? QColor{"#4CBB17"} : QColor{"#CE084E"});
			painter->setPen(colorPen);

			painter->setBrush(secondColor);

			QFont font = painter->font();
			font.setPointSize(11);
			painter->setFont(font);

			QFontMetrics fm{painter->font()};
			int			 width_1 = fm.horizontalAdvance(unitsAmountStr);
			int			 width_2 = fm.horizontalAdvance(units);
			int			 height = fm.height();
			QPoint		 mid = QPoint{r.x() + r.width() / 2, r.y() + r.height() / 2};
			painter->drawText(QPoint{mid.x() - width_1 / 2, mid.y() - 2}, unitsAmountStr);
			painter->drawText(
				QPoint{mid.x() - width_2 / 2, mid.y() + height / 2 + 1}, units);
		}
	}

	// =====================================================
	painter->restore();
}

void HabitDelegate::startAnimation(const QModelIndex& animationIndx)
{
	mAnimatedCellIndx = animationIndx;
	mAnimatedCellStatus = toInt(animationIndx.data(HabitModel::HabitStatusRole));
	mAnimationTimer->start(mAnimatedCellStatus > 0 ? 7 : 20);
	mTableView->viewport()->repaint();
}

void HabitDelegate::stopAnimation()
{
	mAnimationTimer->stop();
	mAnimatedCellIndx = QModelIndex{};
	mPValue = pStartValue;
	mTableView->viewport()->repaint();
}
