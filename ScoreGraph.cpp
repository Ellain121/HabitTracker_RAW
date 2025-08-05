#include "ScoreGraph.hpp"

#include "CircleProgressBar.hpp"
#include "Settings.hpp"
#include "ui_ScoreGraph.h"

#include <jkqtplotter/graphs/jkqtpbarchart.h>
#include <jkqtplotter/graphs/jkqtpfilledcurve.h>
#include <jkqtplotter/jkqtplotter.h>

#include <QDateTime>
#include <cmath>

namespace
{
enum class Color
{
	Pink = 0,
	Green
};

struct Point
{
	double x;
	int	   y;
};

}	 // namespace

void setTheme(JKQTPlotter& plot, Color color)
{
	QString	  configPath{color == Color::Pink ? "/ssd_home/assets/dark_pink.ini"
											  : "/ssd_home/assets/dark_green.ini"};
	QSettings settings{configPath, QSettings::IniFormat};
	plot.loadCurrentPlotterStyle(settings);

	plot.setPlotUpdateEnabled(true);
	plot.redrawPlot();
}

ScoreGraph::ScoreGraph(QWidget* parent)
	: QWidget{parent}
	, ui{new Ui::ScoreGraph}
	, mSettingsDialog{new ScoreGraphSettingsDialog{this}}
{
}

ScoreGraph::~ScoreGraph()
{
	delete ui;
}

void ScoreGraph::init(const Habit* habit)
{
	// I use habitData.begin() later, change this assert later
	assert(habit->getDatesStatus().size() > 0);

	ui->setupUi(this);
	mHabit = habit;
	mSettingsDialog->init(habit);

	setTheme(*ui->scorePlot, Color::Pink);

	initGraphs();
	setLastNDaysStats();

	setPlotterSettings();
	ui->scorePlot->show();

	connect(mSettingsDialog, &ScoreGraphSettingsDialog::SIGNAL_optionChanged, this,
		[this]()
		{
			ui->scorePlot->clearGraphs(true);
			initGraphs();
			ui->scorePlot->zoomToFit();
		});
}

void ScoreGraph::openSettings()
{
	mSettingsDialog->show();
}

void ScoreGraph::initGraphs()
{
	const QMap<QDate, int>& habitData = mHabit->getDatesStatus();
	int						habitDailyGoal = mHabit->getDailyGoal();

	bool isTheoGraphEnabled = toBool(
		Settings::instance().getSetting(mHabit->getName(), "enable_theo_graph_sg"));
	if (isTheoGraphEnabled)
	{
		addTheoreticalGraph(habitData, habitDailyGoal);
	}

	addGraph(habitData, habitDailyGoal, "days", "score", true);
}

void ScoreGraph::setPlotterSettings()
{
	ui->scorePlot->getXAxis()->setMinorTicks(0);
	ui->scorePlot->getYAxis()->setMinorTicks(0);
	//	ui->scorePlot->getXAxis()->setTickWidth(0.0);
	//	ui->scorePlot->getYAxis()->setTickWidth(0.0);

	ui->scorePlot->setToolbarEnabled(false);

	ui->scorePlot->setGrid(false);
	ui->scorePlot->setMousePositionShown(false);
	//	ui->scorePlot->getXAxis()->setAxisLabel("Days");
	//	ui->scorePlot->getYAxis()->setAxisLabel("Score (%)");
	ui->scorePlot->getXAxis()->setTickLabelType(JKQTPCALTdatetime);
	ui->scorePlot->getXAxis()->setTickDateTimeFormat("dd\\\\MMM");
	//	ui->scorePlot->getXAxis()->setTickLabelAngle(-60);

	ui->scorePlot->getXAxis()->setTickLabelColor(QColor::fromRgb(255, 255, 0));

	ui->scorePlot->getXAxis()->setTickLabelFontSize(10);
	ui->scorePlot->getXAxis()->setAutoAxisSpacing(false);
	ui->scorePlot->getXAxis()->setUserTickSpacing(432000000);
	ui->scorePlot->getYAxis()->setTickLabelFontSize(13);
	ui->scorePlot->getYAxis()->setTickUnitName("%");

	ui->scorePlot->zoomToFit();
	ui->scorePlot->resize(400, 300);
}

void ScoreGraph::addTheoreticalGraph(const QMap<QDate, int>& origData, int dailyGoal)
{
	QMap<QDate, int> theoreticalData;
	int				 dailyResult = toInt(
		 Settings::instance().getSetting(mHabit->getName(), "theo_graph_daily_result_sg"));
	for (auto it = origData.begin(); it != origData.end(); ++it)
	{
		theoreticalData.insert(it.key(), dailyResult);
	}
	addGraph(theoreticalData, dailyGoal, "theoreticalX", "theoreticalY", false);
}

void ScoreGraph::addGraph(const QMap<QDate, int>& data, int dailyGoal,
	const QString& xName, const QString& yName, bool setWidgetsStats)
{
	JKQTPDatastore* ds = ui->scorePlot->getDatastore();
	const int		columnXSize = data.size();

	size_t columnX = ds->addColumn(columnXSize, xName);
	size_t columnY = ds->addColumn(columnXSize, yName);

	double xFormula{}, yFormula{}, coef{7.0};
	int	   bestStreak = 0;
	int	   currentStreak = 0;
	int	   i = 0;
	for (auto it = data.begin(); it != data.end(); ++it, ++i)
	{
		QDateTime date{it.key(), QTime{}};
		double	  dayResult = static_cast<double>(it.value()) / dailyGoal;
		double	  xDiff = 0.0;
		if (dayResult >= 1.0)
		{
			currentStreak++;
			// if dayResult = success -> add 1 to xFormula
			xFormula += dayResult;
		}
		else
		{
			currentStreak = 0;
			// if dayResult = failure subtract -5% from y(Formula) (yFormula >= 0)
			// calculate resulted x(Formula)
			yFormula = (yFormula - 5.0 > 0.0) ? yFormula - 5.0 : 0.0;
			xFormula = yFormula * yFormula / 49.0;
		}

		bestStreak = currentStreak > bestStreak ? currentStreak : bestStreak;

		yFormula = std::sqrt(xFormula + xDiff) * coef;
		ds->set(columnX, i, date.toMSecsSinceEpoch());
		ds->set(columnY, i, yFormula);
		date = date.addDays(1);
	}

	JKQTPFilledCurveXGraph* graph = new JKQTPFilledCurveXGraph(ui->scorePlot);
	graph->setXColumn(columnX);
	graph->setYColumn(columnY);
	ui->scorePlot->addGraph(graph);

	if (setWidgetsStats)
	{
		ui->circleProgressBar->upd(yFormula / 100);
		ui->scoreLabel->setText(QString::number(static_cast<int>(yFormula)) + tr("%"));
		ui->currentStreakLabel->setText(QString::number(currentStreak));
		ui->bestStreakLabel->setText(QString::number(bestStreak));
	}
}

void ScoreGraph::setLastNDaysStats()
{
	const QMap<QDate, int>& data = mHabit->getDatesStatus();

	int	 hSize = static_cast<int>(data.size());
	int	 last14DayResult = 0;
	auto it = data.end();
	for (int i = hSize - 1; i >= std::max(hSize - 14, 0); --i)
	{
		int dayResult = (--it).value();
		last14DayResult += dayResult;
	}
	ui->last14ResultLabel->setText(
		QString::number(static_cast<double>(last14DayResult) / 12, 'f', 1) + " hrs");

	int todayResult = data.size() > 0 ? std::prev(data.end()).value() : 0;
	ui->todayResultLabel->setText(
		QString::number(static_cast<double>(todayResult) / 12, 'f', 1) + " hrs");
}
