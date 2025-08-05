#include "SettingsDialog.hpp"

#include "Settings.hpp"
#include "Utility.hpp"
#include "ui_SettingsDialog.h"

namespace
{
const int newDayOffset = 3;
}

SettingsDialog::SettingsDialog(QWidget* parent)
	: QDialog{parent}
	, ui{new Ui::SettingsDialog}
{
	ui->setupUi(this);
	setWindowTitle("Settings");

	// setup newDayOffsetSetting
	if (toInt(Settings::instance().getSetting("global", "new_day_offset")) ==
		newDayOffset)
	{
		ui->newDayOffsetCheckBox->setChecked(true);
	}

	connect(ui->newDayOffsetCheckBox, &QCheckBox::stateChanged, this,
		[this](int checkStateInt)
		{
			Qt::CheckState checkState = static_cast<Qt::CheckState>(checkStateInt);
			int			   offset = checkState == Qt::Unchecked ? 0 : newDayOffset;
			Settings::instance().setSetting("global", "new_day_offset", QVariant{offset});
			qDebug() << "emit SIGNAL_optionChanged();";
			emit SIGNAL_optionChanged();
		});
	// end setup newDayOffsetSetting
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::closeEvent(QCloseEvent*)
{
	this->hide();
}

void SettingsDialog::hideEvent(QHideEvent*)
{
	emit SIGNAL_visibilityChanged(false);
}

void SettingsDialog::showEvent(QShowEvent*)
{
	emit SIGNAL_visibilityChanged(true);
}
