#pragma once

#include "Settings.hpp"

#include <QDialog>

namespace Ui
{
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget* parent = nullptr);
	~SettingsDialog();

signals:
	void SIGNAL_optionChanged(/*Setting setting*/);
	void SIGNAL_visibilityChanged(const bool visible);

protected:
	void closeEvent(QCloseEvent* e) override;
	void hideEvent(QHideEvent* e) override;
	void showEvent(QShowEvent* e) override;

private:
	Ui::SettingsDialog* ui;
};
