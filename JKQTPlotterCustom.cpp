#include "JKQTPlotterCustom.hpp"

JKQTPlotterCustom::JKQTPlotterCustom(QWidget* parent)
	: JKQTPlotter{parent}
{
}
// temp
#include <QDateTime>
void JKQTPlotterCustom::wheelEvent(QWheelEvent* event)
{
	double xMin = getXMin();
	double xMax = getXMax();
	double yMin = getYMin();
	double yMax = getYMax();
	double viewWidth = xMax - xMin;

	//	double zoomStep = (xMax - xMin) / 10;
	double sign = (-1.0) * event->angleDelta().y() / 120;
	double day = 86400000;
	double mult = 2.0;

	double absMaxX = getXAxis()->getAbsoluteMax();
	double absMinX = getXAxis()->getAbsoluteMin();

	double zoomXMin = xMin + day * sign * mult;
	double zoomXMax = xMax + day * sign * mult;
	if (zoomXMin < absMinX)
	{
		zoomXMin = absMinX;
		zoomXMax = absMinX + viewWidth;
	}
	if (zoomXMax > absMaxX)
	{
		zoomXMin = absMaxX - viewWidth;
		zoomXMax = absMaxX;
	}

	setXY(zoomXMin, zoomXMax, yMin, yMax);
	qDebug() << "wheelEvent: " << event->angleDelta()
			 << " absX: " << QDateTime::fromMSecsSinceEpoch(absMinX)
			 << " zoomXMin: " << QDateTime::fromMSecsSinceEpoch(zoomXMin);
}
