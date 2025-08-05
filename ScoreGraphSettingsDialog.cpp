#include "ScoreGraphSettingsDialog.hpp"

#include "Settings.hpp"
#include "Utility.hpp"
#include "ui_ScoreGraphSettingsDialog.h"

ScoreGraphSettingsDialog::ScoreGraphSettingsDialog(QWidget* parent)
	: QDialog{parent}
	, ui{new Ui::ScoreGraphSettingsDialog}
{
}

ScoreGraphSettingsDialog::~ScoreGraphSettingsDialog()
{
	delete ui;
}

void ScoreGraphSettingsDialog::init(const Habit* habit)
{
	ui->setupUi(this);
	mHabit = habit;

	bool theoGraphChecked = toBool(
		Settings::instance().getSetting(mHabit->getName(), "enable_theo_graph_sg"));
	ui->theoreticalGraphCheckbox->setChecked(theoGraphChecked);

	int theoGraphDailyGoal = toInt(
		Settings::instance().getSetting(mHabit->getName(), "theo_graph_daily_result_sg"));
	ui->dailyResultSpinBox->setValue(theoGraphDailyGoal);

	connect(ui->theoreticalGraphCheckbox, &QCheckBox::stateChanged, this,
		[this](int checkStateInt)
		{
			Qt::CheckState checkState = static_cast<Qt::CheckState>(checkStateInt);
			bool		   status = !(checkState == Qt::CheckState::Unchecked);
			Settings::instance().setSetting(
				mHabit->getName(), "enable_theo_graph_sg", QVariant{status});
			qDebug() << "emit ScoreGraphSettingsDialog::SIGNAL_optionChanged();";
			emit SIGNAL_optionChanged();
		});
	connect(ui->dailyResultSpinBox, &QSpinBox::valueChanged, this,
		[this](int newValue)
		{
			Settings::instance().setSetting(
				mHabit->getName(), "theo_graph_daily_result_sg", QVariant{newValue});
			qDebug() << "emit ScoreGraphSettingsDialog::SIGNAL_optionChanged();";
			emit SIGNAL_optionChanged();
		});
}
