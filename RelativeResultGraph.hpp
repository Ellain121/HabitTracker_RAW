#pragma once

#include "Habit.hpp"

#include <QWidget>

namespace Ui
{
class RelativeResultGraph;
}

class RelativeResultGraph : public QWidget
{
	Q_OBJECT

public:
	enum class TimePeriod
	{
		Day = 0,
		Week,
		Month,
		Year,
		N_Days,
		Size
	};

	struct LocalSettings
	{
		TimePeriod timePeriod;
		int		   nDays;
	};

public:
	explicit RelativeResultGraph(QWidget* parent = nullptr);
	~RelativeResultGraph();

	void init(const Habit* habit);

private:
	LocalSettings getLocalSettings();
	bool		  addGraph();

	void setPlotterSettings();
	void setPlotterAbsoluteX();
	void setPlotterZoom();

	void initGraphInfoWidgets();
	void initXTickGroupBox();

	//	void setGraph_1(const Habit habit);

private:
	Ui::RelativeResultGraph* ui;
	LocalSettings			 mSettings;
	const Habit*			 mHabit = nullptr;
};
