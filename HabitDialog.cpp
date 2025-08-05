#include "HabitDialog.hpp"

#include "ReccurenceDialog.hpp"
#include "Utility.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <numeric>

//
#include <QDebug>

namespace
{
enum class RepeatOption
{
	Daily = 0,
	Custom
};

const QString RepOptToStr[2] = {"Daily", "Custom"};

}	 // namespace

HabitDialog::HabitDialog(const QString& header, Mode mode, const QString& okButtonText,
	const QString& cancelButtonText, const QString& nameField, const QDate& date,
	Habit::Type habitType, const QString& units, int dailyGoal, QWidget* parent)
	: QDialog{parent}
	, mMode{mode}
	, mFormLayout{new QFormLayout{this}}
	, nameLineEdit{new QLineEdit{nameField, this}}
	, startDateEdit{new QDateEdit{date, this}}
	, habitRepeatComboBox{new QComboBox{this}}
	, habitTypeComboBox{new QComboBox{this}}
	, repeatInfo{Habit::RepeatInfo{RI_Settings::Daily}}	   // daily
{
	/***************** [Row elements settings] *******************/
	habitRepeatComboBox->addItem(RepOptToStr[static_cast<int>(RepeatOption::Daily)]);
	habitRepeatComboBox->addItem(RepOptToStr[static_cast<int>(RepeatOption::Custom)]);
	habitRepeatComboBox->setCurrentIndex(0);
	habitRepeatComboBox->setEnabled(mMode == Mode::Add);

	habitTypeComboBox->addItem(Habit::typeToString(Habit::Type::OneTime));
	habitTypeComboBox->addItem(Habit::typeToString(Habit::Type::Quantitative));
	habitTypeComboBox->setCurrentIndex(static_cast<int>(habitType));

	/*************** [FormLayout setup] ************************/
	QLabel* label = new QLabel{header};
	label->setAlignment(Qt::AlignCenter);
	mFormLayout->addRow(label);
	mFormLayout->addRow(tr("Name: "), nameLineEdit);
	mFormLayout->addRow(tr("Start date: "), startDateEdit);
	mFormLayout->addRow(tr("Repeat: "), habitRepeatComboBox);
	mFormLayout->addRow(tr("Habit type: "), habitTypeComboBox);
	if (habitType == Habit::Type::Quantitative)
	{
		addRowsForQuanOption(units, dailyGoal, mFormLayout->rowCount());
	}

	// Add some standard buttons (Cancel/Ok) at the bottom of the dialog
	QDialogButtonBox* buttonBox = new QDialogButtonBox(
		QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	buttonBox->button(QDialogButtonBox::Ok)->setText(okButtonText);
	buttonBox->button(QDialogButtonBox::Cancel)->setText(cancelButtonText);
	mFormLayout->addRow(buttonBox);

	/*************** [Connections] ************************/
	QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	connect(habitRepeatComboBox,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
		[this](int indx)
		{
			if (static_cast<RepeatOption>(indx) == RepeatOption::Custom)
			{
				// Create a ReccurenceDialog, asking for habit custom repeat info
				ReccurenceDialog recDialog{};
				if (recDialog.exec() == QDialog::Accepted)
				{
					repeatInfo = recDialog.getRepeatInfo();
				}
				else
				{
					// if we cancel, set repeat option to default(daily)
					habitRepeatComboBox->setCurrentIndex(
						static_cast<int>(RepeatOption::Daily));
					//                    repeatInfo =
				}
			}
		});

	connect(habitTypeComboBox,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
		[this, units, dailyGoal](int indx)
		{
			if (static_cast<Habit::Type>(indx) == Habit::Type::Quantitative)
			{
				addRowsForQuanOption(units, dailyGoal, mFormLayout->rowCount() - 1);
			}
			else if (static_cast<Habit::Type>(indx) == Habit::Type::OneTime)
			{
				removeRowsForQuanOption();
			}
		});
}

QString HabitDialog::getName() const
{
	return nameLineEdit->text();
}

QDate HabitDialog::getStartDate() const
{
	return startDateEdit->date();
}

Habit::Type HabitDialog::getType() const
{
	return Habit::stringToType(habitTypeComboBox->currentText());
}

QString HabitDialog::getUnits() const
{
	if (habitUnitsEdit == nullptr)
	{
		return QString{};
	}
	return habitUnitsEdit->text();
}

int HabitDialog::getDailyGoal() const
{
	if (dailyGoalEdit == nullptr)
	{
		// for Onetime type habits goal is 1(true)
		return static_cast<int>(true);
	}
	return toInt(dailyGoalEdit->text());
}

Habit::RepeatInfo HabitDialog::getRepeatInfo() const
{
	return repeatInfo;
}

void HabitDialog::closeEvent(QCloseEvent* e)
{
	// when closing window, emit signal number 5
	// 5 is undefined signal so no effect will happen
	// maybe this solution isn't quite right
	done(5);
	QDialog::closeEvent(e);
}

void HabitDialog::addRowsForQuanOption(
	const QString& defaultUnits, const int& defaultDailyGoal, int rowPos)
{
	habitUnitsEdit = new QLineEdit{defaultUnits, this};
	dailyGoalEdit = new QLineEdit{QString::number(defaultDailyGoal), this};
	dailyGoalEdit->setValidator(
		new QIntValidator{0, std::numeric_limits<int>::max(), this});

	mFormLayout->insertRow(rowPos, tr("Units: "), habitUnitsEdit);
	mFormLayout->insertRow(rowPos, tr("Daily goal: "), dailyGoalEdit);
}

void HabitDialog::removeRowsForQuanOption()
{
	mFormLayout->removeRow(habitUnitsEdit);
	mFormLayout->removeRow(dailyGoalEdit);
	habitUnitsEdit = nullptr;
	dailyGoalEdit = nullptr;
	adjustSize();
}
