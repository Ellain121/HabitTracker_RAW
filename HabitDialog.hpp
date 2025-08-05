#pragma once

#include "Habit.hpp"
#include "Utility.hpp"

#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPointer>

class HabitDialog : public QDialog
{
	Q_OBJECT

public:
	enum class Mode
	{
		Add = 0,
		Edit
	};

public:
	HabitDialog(const QString& header, Mode mode = Mode::Add,
		const QString& okButtonText = "OK", const QString& cancelButtonText = "Cancel",
		const QString& nameField = "name", const QDate& date = currentDate(),
		Habit::Type	   habitType = Habit::Type::OneTime,
		const QString& units = "e.g. minutes", int dailyGoal = 1,
		QWidget* parent = nullptr);

	QString			  getName() const;
	QDate			  getStartDate() const;
	Habit::Type		  getType() const;
	QString			  getUnits() const;
	int				  getDailyGoal() const;
	Habit::RepeatInfo getRepeatInfo() const;

protected:
	virtual void closeEvent(QCloseEvent* e) override;

private:
	void addRowsForQuanOption(
		const QString& defaultUnits, const int& defaultDailyGoal, int rowPos);
	void removeRowsForQuanOption();

private:
	Mode		 mMode;
	QFormLayout* mFormLayout = nullptr;

	QLineEdit* nameLineEdit = nullptr;
	QDateEdit* startDateEdit = nullptr;
	QComboBox* habitRepeatComboBox = nullptr;
	QComboBox* habitTypeComboBox = nullptr;
	QLineEdit* habitUnitsEdit = nullptr;
	QLineEdit* dailyGoalEdit = nullptr;

	Habit::RepeatInfo repeatInfo;
};
