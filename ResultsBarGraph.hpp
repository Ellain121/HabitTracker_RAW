#pragma once

#include "Habit.hpp"

#include <QWidget>

namespace Ui
{
class ResultsBarGraph;
}

class ResultsBarGraph : public QWidget
{
	Q_OBJECT

public:
	explicit ResultsBarGraph(QWidget* parent = nullptr);
	~ResultsBarGraph();

	void init(const Habit* habit);

private:
	void addGraph();
	void setPlotterSettings();
	// required for correct zooming in JKQTPlotterCustom
	void setPlotterAbsoluteX();

private:
	Ui::ResultsBarGraph* ui;
	const Habit*		 mHabit = nullptr;
};
