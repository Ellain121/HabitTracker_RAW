#include "CircleProgressBar.hpp"

#include <QColor>
#include <QPainter>
#include <QPainterPath>

CircleProgressBar::CircleProgressBar(QWidget* parent)
	: QWidget{parent}
	, p{}
{
	setMinimumSize(80, 80);
	setMaximumSize(80, 80);
}

void CircleProgressBar::upd(double pp)
{
	if (p == pp)
		return;

	p = pp;
	update();
}

void CircleProgressBar::paintEvent(QPaintEvent*)
{
	double	 pd = p * 360;
	double	 rd = 360 - pd;
	QPainter p(this);
	double	 side = size().width();
	double	 width = side / 5;

	p.setRenderHint(QPainter::Antialiasing);

	QPen pen, pen2;
	pen.setCapStyle(Qt::FlatCap);
	pen.setColor(QColor("#6dd187"));
	pen.setWidth(width);

	pen2.setWidth(width);
	pen2.setColor(QColor("#909191"));
	pen2.setCapStyle(Qt::FlatCap);

	QPainterPath path, path2;
	path.moveTo(side / 2, width / 2);
	path.arcTo(QRectF{width / 2, width / 2, side - width, side - width}, 90, -pd);
	path2.moveTo(side / 2, width / 2);
	path2.arcTo(QRectF{width / 2, width / 2, side - width, side - width}, 90, rd);

	p.strokePath(path, pen);
	p.strokePath(path2, pen2);
}

int CircleProgressBar::heightForWidth(int width) const
{
	return width;
}
bool CircleProgressBar::hasHeightForWidth() const
{
	return true;
}
