#include "ImportDialog.hpp"

#include "HabitDialog.hpp"
#include "HabitModel.hpp"
#include "ImportLoader.hpp"
#include "Utility.hpp"

#include <QDebug>
#include <QDialogButtonBox>
#include <QListView>
#include <QPushButton>

ImportDialog::ImportDialog(const QString& importDBFilename, const QString& okButtonText,
    const QString& cancelButtonText, QWidget* parent)
    : QDialog{parent}
    , mFormLayout{new QFormLayout{this}}
    , mHabitsListView{new QListView{this}}
    , mNewHabits{}
{
    mNewHabits = loadImportedHabits(importDBFilename);

    mFormLayout->addRow(mHabitsListView);

    HabitModel* habitModel = new HabitModel{true, this};
    mHabitsListView->setModel(habitModel);

    for (auto& habit : mNewHabits)
    {
        habitModel->addHabit(habit);
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

    connect(mHabitsListView, &QListView::doubleClicked, this,
        [habitModel, this](const QModelIndex& index)
        {
            Habit::Type habitType =
                static_cast<Habit::Type>(toInt(index.data(HabitModel::TypeRole)));
            QString habitName = toQString(index.data(HabitModel::Roles::NameRole));
            QDate	habitDate = toQDate(index.data(HabitModel::Roles::StartDateRole));
            QString habitUnits = toQString(index.data(HabitModel::Roles::UnitsRole));
            int		habitDailyGoal = toInt(index.data(HabitModel::Roles::DailyGoalRole));
            HabitDialog habitDialog{"Edit habit", HabitDialog::Mode::Edit, "Accept",
                "Delete", habitName, habitDate, habitType, habitUnits, habitDailyGoal};

            decltype(auto) result = habitDialog.exec();
            if (result == QDialog::Accepted)
            {
                QString		name = habitDialog.getName();
                QDate		date = habitDialog.getStartDate();
                Habit::Type type = habitDialog.getType();
                QString		units = habitDialog.getUnits();
                int			dailyGoal = habitDialog.getDailyGoal();
                assert((index.row() >= 0 &&
                           index.row() < static_cast<int>(mNewHabits.size())) &&
                       "Error");
                Habit& habit = mNewHabits[index.row()];

                habitModel->setData(index, name, HabitModel::Roles::NameRole);
                habitModel->setData(index, date, HabitModel::Roles::StartDateRole);
                habitModel->setData(
                    index, static_cast<int>(type), HabitModel::Roles::TypeRole);
                habitModel->setData(index, units, HabitModel::Roles::UnitsRole);
                habitModel->setData(index, dailyGoal, HabitModel::Roles::DailyGoalRole);

                habit = habitModel->getHabit(index);
            }
            else if (result == QDialog::Rejected)
            {
                qDebug() << "Rejected!";
            }
        });
}

const std::vector<Habit>& ImportDialog::getNewHabits() const
{
    return mNewHabits;
}
