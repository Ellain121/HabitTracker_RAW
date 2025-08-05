#pragma once

#include "Habit.hpp"

#include <QDialog>
#include <array>

namespace Ui
{
class StatsDialog;
}

class StatsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit StatsDialog(Habit&& habit, QWidget* parent = nullptr);
	~StatsDialog();

private:
	void init();
	void openStatsWidgetSettings(int indx);

private:
	Ui::StatsDialog* ui;
	Habit			 mHabit;

	std::array<QWidget*, 3> mStatsWidgets;
	int						mCurrentIndx;
};
