#include "ReccurenceDialog.hpp"

#include "Constants.hpp"
#include "ListViewCustom.hpp"
#include "Utility.hpp"

#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QGraphicsEffect>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVBoxLayout>

namespace
{
const QString weekNameByIndx[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

const int patternSizeStartIndx = 2;
const int patternSizeMaxIndx = 30;
}	 // namespace

ReccurenceDialog::ReccurenceDialog()
	: mFormLayout{new QFormLayout{this}}
	, mFrequencyLineEdit{new QLineEdit{QString::number(1), this}}
	, mPatternSizeComboBox{new QComboBox{this}}
	, mTimeUnitComboBox{new QComboBox{this}}
	, mRepeatLayoutIndx_1{-1}
	, mRepeatLayoutIndx_2{-1}
	, mOptionsListView{nullptr}
	, mEndsOptionCheckBox_1{new QCheckBox{"Never", this}}
	, mEndsOptionCheckBox_2{new QCheckBox{"On    ", this}}
	, mEndsOptionDateEdit_2{new QDateEdit{currentDate(), this}}
	, mEndsOptionCheckBox_3{new QCheckBox{"After", this}}
	, mEndsOptionLineEdit_3{new QLineEdit{QString::number(10), this}}
{
	/**************** [First row settings] ******************/
	QCheckBox* repeatTimeUnitCheckBox = new QCheckBox{"Repeat every", this};

	mFrequencyLineEdit->setValidator(new QIntValidator{1, 99, this});
	mFrequencyLineEdit->setFixedWidth(30);
	mFrequencyLineEdit->setAlignment(Qt::AlignCenter);

	mTimeUnitComboBox->addItem("day");
	mTimeUnitComboBox->addItem("week");
	mTimeUnitComboBox->addItem("month");
	mTimeUnitComboBox->addItem("year");
	mTimeUnitComboBox->setCurrentIndex(0);

	QHBoxLayout* repeatHBoxLayout_1 = new QHBoxLayout{this};
	repeatHBoxLayout_1->addWidget(repeatTimeUnitCheckBox);
	repeatHBoxLayout_1->addWidget(mFrequencyLineEdit);
	repeatHBoxLayout_1->addWidget(mTimeUnitComboBox);

	/*************** [Second row settings] *******************/
	QCheckBox* repeatPatternCheckBox = new QCheckBox{"Repeat ", this};
	QLabel*	   patternSizeLabel = new QLabel{"days pattern", this};

	for (int i = patternSizeStartIndx; i <= patternSizeMaxIndx; ++i)
	{
		mPatternSizeComboBox->addItem(QString::number(i));
	}
	mPatternSizeComboBox->setFixedWidth(50);
	mPatternSizeComboBox->setStyleSheet("combobox-popup: 0;");

	QHBoxLayout* repeatHBoxLayout_2 = new QHBoxLayout{this};
	repeatHBoxLayout_2->addWidget(repeatPatternCheckBox);
	repeatHBoxLayout_2->addWidget(mPatternSizeComboBox);
	repeatHBoxLayout_2->addWidget(patternSizeLabel);

	mPatternSizeComboBox->setEnabled(false);
	patternSizeLabel->setEnabled(false);

	/*********** [Exclusive checkboxes for 1,2 rows] *******/
	QButtonGroup* repeatOptionButtonGroup = new QButtonGroup{this};
	repeatOptionButtonGroup->addButton(repeatTimeUnitCheckBox);
	repeatOptionButtonGroup->addButton(repeatPatternCheckBox);
	repeatTimeUnitCheckBox->setCheckState(Qt::Checked);

	/*********** [Ends Options] **********************/
	mEndsOptionLineEdit_3->setValidator(new QIntValidator{1, 10000, this});
	mEndsOptionLineEdit_3->setFixedWidth(60);
	mEndsOptionLineEdit_3->setAlignment(Qt::AlignCenter);

	mEndsOptionDateEdit_2->setEnabled(false);
	mEndsOptionLineEdit_3->setEnabled(false);

	QButtonGroup* endsOptionButtonGroup = new QButtonGroup{this};
	endsOptionButtonGroup->addButton(mEndsOptionCheckBox_1);
	endsOptionButtonGroup->addButton(mEndsOptionCheckBox_2);
	endsOptionButtonGroup->addButton(mEndsOptionCheckBox_3);
	mEndsOptionCheckBox_1->setCheckState(Qt::Checked);

	QHBoxLayout* endsOptionLayout_2 = new QHBoxLayout{this};
	endsOptionLayout_2->addWidget(mEndsOptionCheckBox_2);
	endsOptionLayout_2->addWidget(mEndsOptionDateEdit_2);
	endsOptionLayout_2->addStretch();

	QHBoxLayout* endsOptionLayout_3 = new QHBoxLayout{this};
	endsOptionLayout_3->addWidget(mEndsOptionCheckBox_3);
	endsOptionLayout_3->addWidget(mEndsOptionLineEdit_3);
	endsOptionLayout_3->addWidget(new QLabel{"days", this});
	endsOptionLayout_3->addStretch();

	QLabel* label = new QLabel{"Custom occurence", this};
	label->setAlignment(Qt::AlignCenter);
	QLabel* label_2 = new QLabel{"Ends", this};
	label_2->setAlignment(Qt::AlignLeft);

	/***************** [FormLayout setup] *********************/
	mFormLayout->addRow(label);
	mFormLayout->addRow(repeatHBoxLayout_1);
	mRepeatLayoutIndx_1 = mFormLayout->count();
	mFormLayout->addRow(repeatHBoxLayout_2);
	mRepeatLayoutIndx_2 = mFormLayout->count();
	mFormLayout->addRow(label_2);
	mFormLayout->addRow(mEndsOptionCheckBox_1);
	mFormLayout->addRow(endsOptionLayout_2);
	mFormLayout->addRow(endsOptionLayout_3);

	// Add some standard buttons (Cancel/Ok) at the bottom of the dialog
	QDialogButtonBox* buttonBox = new QDialogButtonBox(
		QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	buttonBox->button(QDialogButtonBox::Ok)->setText("Done");
	buttonBox->button(QDialogButtonBox::Cancel)->setText("Cancel");
	mFormLayout->addRow(buttonBox);

	/*************** Connections ************************************/
	QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	connect(repeatTimeUnitCheckBox, &QCheckBox::stateChanged, this,
		[this](int checkState)
		{
			bool isChecked = static_cast<Qt::CheckState>(checkState) == Qt::Checked;
			// Activate/Deactivate row
			mTimeUnitComboBox->setCurrentIndex(0);
			mFrequencyLineEdit->setEnabled(isChecked);
			mTimeUnitComboBox->setEnabled(isChecked);
		});

	connect(repeatPatternCheckBox, &QCheckBox::stateChanged, this,
		[this, patternSizeLabel](int checkState)
		{
			bool isChecked = static_cast<Qt::CheckState>(checkState) == Qt::Checked;
			// Activate/Deactivate row
			if (isChecked)
			{
				addRowOptionsListView(mRepeatLayoutIndx_2, Habit::TimeUnit::NDays,
					mPatternSizeComboBox->currentIndex() + patternSizeStartIndx);
			}
			else
			{
				removeRowOptionsListView();
			}
			mPatternSizeComboBox->setEnabled(isChecked);
			patternSizeLabel->setEnabled(isChecked);
		});

	connect(mTimeUnitComboBox,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
		[this](int indx)
		{
			if (static_cast<Habit::TimeUnit>(indx) == Habit::TimeUnit::Week)
			{
				addRowOptionsListView(mRepeatLayoutIndx_1, Habit::TimeUnit::Week);
			}
			else if (static_cast<Habit::TimeUnit>(indx) == Habit::TimeUnit::Month)
			{
				addRowOptionsListView(mRepeatLayoutIndx_1, Habit::TimeUnit::Month);
			}
			else
			{
				removeRowOptionsListView();
			}
		});

	connect(mPatternSizeComboBox,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
		[this](int indx)
		{
			addRowOptionsListView(
				mRepeatLayoutIndx_2, Habit::TimeUnit::NDays, indx + patternSizeStartIndx);
		});

	connect(mEndsOptionCheckBox_2, &QCheckBox::stateChanged, this,
		[this](int checkState)
		{
			bool isChecked = static_cast<Qt::CheckState>(checkState) == Qt::Checked;
			mEndsOptionDateEdit_2->setEnabled(isChecked);
		});
	connect(mEndsOptionCheckBox_3, &QCheckBox::stateChanged, this,
		[this](int checkState)
		{
			bool isChecked = static_cast<Qt::CheckState>(checkState) == Qt::Checked;
			mEndsOptionLineEdit_3->setEnabled(isChecked);
		});
}

Habit::RepeatInfo ReccurenceDialog::getRepeatInfo() const
{
	Habit::RepeatInfo repeatInfo;

	// RepeatInfo::frequence
	repeatInfo.frequence = toInt(mFrequencyLineEdit->text());

	// RepeatInfo::dateUnit
	// RepeatInfo::patternSize
	if (mTimeUnitComboBox->isEnabled())
	{
		repeatInfo.dateUnit =
			static_cast<Habit::TimeUnit>(mTimeUnitComboBox->currentIndex());
		repeatInfo.patternSize = INT_NULL_VALUE;
	}
	else
	{
		repeatInfo.dateUnit = Habit::TimeUnit::NDays;
		repeatInfo.patternSize =
			mPatternSizeComboBox->currentIndex() + patternSizeStartIndx;
	}

	// RepeatInfo::pattern
	if (mOptionsListView != nullptr)
	{
		int pattern = 0;
		for (int i = 0; i < mOptionsListView->model()->rowCount(); ++i)
		{
			QModelIndex indx = mOptionsListView->model()->index(i, 0);
			int			checked = toInt(indx.data(Qt::CheckStateRole));
			int			bit = static_cast<Qt::CheckState>(checked) == Qt::Checked;

			pattern |= (bit << i);
		}
		repeatInfo.pattern = pattern;
	}
	else
	{
		repeatInfo.pattern = 0;
	}

	// RepeatInfo::ends
	if (mEndsOptionCheckBox_1->checkState() == Qt::Checked)
	{
		repeatInfo.ends = "Never";
	}
	else if (mEndsOptionCheckBox_2->checkState() == Qt::Checked)
	{
		repeatInfo.ends = mEndsOptionDateEdit_2->date().toString(Qt::ISODate);
	}
	else if (mEndsOptionCheckBox_3->checkState() == Qt::Checked)
	{
		toInt(mEndsOptionLineEdit_3->text());

		repeatInfo.ends = mEndsOptionLineEdit_3->text();
	}
	return repeatInfo;
}

void ReccurenceDialog::addRowOptionsListView(int rowIndx, Habit::TimeUnit timeUnit, int n)
{
	// delete & clear mOptionsListView if it exist
	removeRowOptionsListView();

	mOptionsListView = new ListViewCustom{this};
	QStandardItemModel* model = new QStandardItemModel{mOptionsListView};

	if (timeUnit == Habit::TimeUnit::Week)
	{
		for (size_t i = 0; i < std::size(weekNameByIndx); ++i)
		{
			QStandardItem* checkBox = new QStandardItem;
			checkBox->setText(weekNameByIndx[i]);
			checkBox->setCheckable(true);
			checkBox->setData(Qt::Unchecked, Qt::CheckStateRole);
			checkBox->setSizeHint(QSize{60, 18});
			model->setItem(i, checkBox);
		}
	}
	else if (timeUnit == Habit::TimeUnit::Month)
	{
		for (int i = 0; i < 31; ++i)
		{
			QStandardItem* checkBox = new QStandardItem;
			checkBox->setText(
				(i + 1 < 10 ? QString{"0"} : QString{""}) + QString::number(i + 1));
			checkBox->setCheckable(true);
			checkBox->setData(Qt::Unchecked, Qt::CheckStateRole);
			model->setItem(i, checkBox);
		}
	}
	else if (timeUnit == Habit::TimeUnit::NDays)
	{
		for (int i = 0; i < n; ++i)
		{
			QStandardItem* checkBox = new QStandardItem;
			checkBox->setText(
				(i + 1 < 10 ? QString{"0"} : QString{""}) + QString::number(i + 1));
			checkBox->setCheckable(true);
			checkBox->setData(Qt::Unchecked, Qt::CheckStateRole);
			model->setItem(i, checkBox);
		}
	}
	mOptionsListView->setResizeMode(QListView::Adjust);
	mOptionsListView->setFlow(QListView::LeftToRight);
	mOptionsListView->setWrapping(true);
	mOptionsListView->setModel(model);
	mOptionsListView->setSelectionMode(QAbstractItemView::NoSelection);
	mOptionsListView->setFocusPolicy(Qt::NoFocus);
	mOptionsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

	auto sp = mOptionsListView->sizePolicy();
	sp.setVerticalPolicy(QSizePolicy::Fixed);
	mOptionsListView->setSizePolicy(sp);

	mFormLayout->insertRow(rowIndx, mOptionsListView);
	adjustSize();
}

void ReccurenceDialog::removeRowOptionsListView()
{
	if (mOptionsListView != nullptr)
	{
		mFormLayout->removeRow(mOptionsListView);
		mOptionsListView = nullptr;
	}
}
