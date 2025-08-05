#pragma once

#include "Habit.hpp"
#include "ScoreGraphSettingsDialog.hpp"

#include <QHash>
#include <QWidget>

namespace Ui
{
class ScoreGraph;
}

class ScoreGraph : public QWidget
{
	Q_OBJECT

public:
	explicit ScoreGraph(QWidget* parent = nullptr);
	~ScoreGraph();

	void init(const Habit* habit);
	void openSettings();

private:
	void initGraphs();

	void setPlotterSettings();
	void addTheoreticalGraph(const QMap<QDate, int>& origData, int dailyGoal);
	void addGraph(const QMap<QDate, int>& data, int dailyGoal, const QString& xName,
		const QString& yName, bool setWidgetsStats = false);

	void setLastNDaysStats();

private:
	Ui::ScoreGraph*			  ui;
	const Habit*			  mHabit = nullptr;
	ScoreGraphSettingsDialog* mSettingsDialog = nullptr;
	//	const QMap<QDate, int>& mHabitData;
};
