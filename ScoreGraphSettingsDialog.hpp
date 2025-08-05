#pragma once

#include <Habit.hpp>
#include <QDialog>

namespace Ui
{
class ScoreGraphSettingsDialog;
}

class ScoreGraphSettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ScoreGraphSettingsDialog(QWidget* parent = nullptr);
	~ScoreGraphSettingsDialog();

	void init(const Habit* habit);

signals:
	void SIGNAL_optionChanged();

private:
	Ui::ScoreGraphSettingsDialog* ui;
	const Habit*				  mHabit = nullptr;
};
