#pragma once

#include "Habit.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>

class ListViewCustom;

class ReccurenceDialog : public QDialog
{
    Q_OBJECT

public:
    ReccurenceDialog();

    Habit::RepeatInfo getRepeatInfo() const;

private:
    void addRowOptionsListView(int rowIndx, Habit::TimeUnit timeUnit, int n = 0);
    void removeRowOptionsListView();

private:
    QFormLayout* mFormLayout;

    QLineEdit*		mFrequencyLineEdit;
    QComboBox*		mPatternSizeComboBox;
    QComboBox*		mTimeUnitComboBox;
    int				mRepeatLayoutIndx_1;
    int				mRepeatLayoutIndx_2;
    ListViewCustom* mOptionsListView;

    QCheckBox* mEndsOptionCheckBox_1;
    QCheckBox* mEndsOptionCheckBox_2;
    QDateEdit* mEndsOptionDateEdit_2;
    QCheckBox* mEndsOptionCheckBox_3;
    QLineEdit* mEndsOptionLineEdit_3;
};
