#pragma once

#include <jkqtplotter/jkqtplotter.h>

class JKQTPlotterCustom : public JKQTPlotter
{
public:
	JKQTPlotterCustom(QWidget* parent = nullptr);

protected:
	void wheelEvent(QWheelEvent* event) override;
};
