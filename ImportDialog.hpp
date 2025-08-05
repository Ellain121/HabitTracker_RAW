#pragma once

#include "Habit.hpp"

#include <QDialog>
#include <QFormLayout>
#include <vector>

class QListView;

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    ImportDialog(const QString& importDBFilename, const QString& okButtonText = "OK",
        const QString& cancelButtonText = "Cancel", QWidget* parent = nullptr);

    const std::vector<Habit>& getNewHabits() const;

private:
    QFormLayout*	   mFormLayout;
    QListView*		   mHabitsListView;
    std::vector<Habit> mNewHabits;
};
