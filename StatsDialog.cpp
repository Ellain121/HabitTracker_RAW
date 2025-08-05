#include "StatsDialog.hpp"

#include "ui_StatsDialog.h"

namespace
{

enum class StatsWidgetType
{
	ScoreGraphWT = 0,
	ResultBarGraphWT = 1,
	RelativeResultGraphWT = 2
};

}

StatsDialog::StatsDialog(Habit&& habit, QWidget* parent)
	: QDialog{parent}
	, ui{new Ui::StatsDialog}
	, mHabit{std::move(habit)}
	, mCurrentIndx{0}
{
	init();
	setWindowTitle("Habis statistics");

	ui->scorePage->init(&mHabit);
	ui->resultBarPage->init(&mHabit);
	ui->relativeResultPage->init(&mHabit);

	ui->graphStackedWidget->setCurrentIndex(mCurrentIndx);

	connect(ui->prevGraphButton, &QPushButton::clicked, this,
		[=, this]()
		{
			int size = ui->graphStackedWidget->count();
			mCurrentIndx = (mCurrentIndx + size - 1) % size;
			ui->graphStackedWidget->setCurrentIndex(mCurrentIndx);
			ui->graphLabel->setText(
				ui->graphStackedWidget->currentWidget()->accessibleName());
		});
	connect(ui->nextGraphButton, &QPushButton::clicked, this,
		[=, this]()
		{
			int size = ui->graphStackedWidget->count();
			mCurrentIndx = (mCurrentIndx + 1) % size;
			ui->graphStackedWidget->setCurrentIndex(mCurrentIndx);
			ui->graphLabel->setText(
				ui->graphStackedWidget->currentWidget()->accessibleName());
		});
	connect(ui->graphSettings, &QPushButton::clicked, this,
		[this]() {	  //
			int indx = ui->graphStackedWidget->currentIndex();
			openStatsWidgetSettings(indx);
		});
}

StatsDialog::~StatsDialog()
{
	delete ui;
}

void StatsDialog::init()
{
	ui->setupUi(this);

	// do not change order or anything
	mStatsWidgets[static_cast<int>(StatsWidgetType::ScoreGraphWT)] = ui->scorePage;
	mStatsWidgets[static_cast<int>(StatsWidgetType::ResultBarGraphWT)] =
		ui->resultBarPage;
	mStatsWidgets[static_cast<int>(StatsWidgetType::RelativeResultGraphWT)] =
		ui->relativeResultPage;
}

void StatsDialog::openStatsWidgetSettings(int indx)
{
	QWidget* statsWT = mStatsWidgets[indx];
	// Seems like it's impossible to use .ui from QWidget subclass instead only QWidget
	// should be use. At least as far as I know
	// So it's kind of a crutch, but I don't know how to do it better
	switch (static_cast<StatsWidgetType>(indx))
	{
		using enum StatsWidgetType;

		case ScoreGraphWT:
		{
			ScoreGraph* scoreGraph = dynamic_cast<ScoreGraph*>(statsWT);
			assert(scoreGraph);
			scoreGraph->openSettings();
		}
		break;

		case ResultBarGraphWT:
		{
			ResultsBarGraph* resultsBarGraph = dynamic_cast<ResultsBarGraph*>(statsWT);
			assert(resultsBarGraph);
		}
		break;

		case RelativeResultGraphWT:
		{
			RelativeResultGraph* relativeResultGraph =
				dynamic_cast<RelativeResultGraph*>(statsWT);
			assert(relativeResultGraph);
		}
		break;
	}
	//	ScoreGraph;
}

// 147
//  417
