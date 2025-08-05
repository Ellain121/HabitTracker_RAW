#include "ResultsBarGraph.hpp"

#include "ui_ResultsBarGraph.h"

#include <jkqtplotter/graphs/jkqtpbarchart.h>
#include <jkqtplotter/graphs/jkqtpfilledcurve.h>
#include <jkqtplotter/graphs/jkqtpgraphlabels.h>
#include <jkqtplotter/jkqtplotter.h>

#include <QDateTime>

namespace
{

const double DAY_IN_MSEC = 86400000;

enum class Color
{
	Pink = 0,
	Green
};

int maxValue(const QMap<QDate, int>& habitData)
{
	int maxVal = 0;
	for (auto it = habitData.begin(); it != habitData.end(); ++it)
	{
		int val = it.value();
		if (val > maxVal)
		{
			maxVal = val;
		}
	}
	return maxVal;
}
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

ResultsBarGraph::ResultsBarGraph(QWidget* parent)
	: QWidget{parent}
	, ui{new Ui::ResultsBarGraph}
{
}

ResultsBarGraph::~ResultsBarGraph()
{
	delete ui;
}

void ResultsBarGraph::init(const Habit* habit)
{
	// I use habitData.begin() later, change this assert later
	assert(habit->getDatesStatus().size() > 0);
	ui->setupUi(this);
	mHabit = habit;

	setTheme(*ui->resultBarPlot, Color::Green);

	addGraph();
	setPlotterSettings();
	setPlotterAbsoluteX();
	ui->resultBarPlot->show();
}

void ResultsBarGraph::addGraph()
{
	const QMap<QDate, int>& habitData = mHabit->getDatesStatus();
	int						habitDailyGoal = mHabit->getDailyGoal();

	JKQTPDatastore* ds = ui->resultBarPlot->getDatastore();
	const int		columnXSize = habitData.size();

	size_t columnX = ds->addColumn(columnXSize, "days");
	size_t columnY = ds->addColumn(columnXSize, "relative_score");

	int i = 0;
	for (auto it = habitData.begin(); it != habitData.end(); ++it, ++i)
	{
		QDateTime date{it.key(), QTime{}};
		double	  dayResult = static_cast<double>(it.value());

		ds->set(columnX, i, date.toMSecsSinceEpoch());
		ds->set(columnY, i, dayResult);
		date = date.addDays(1);
	}

	JKQTPBarGraphBase* graph = new JKQTPBarVerticalGraph{ui->resultBarPlot};

	graph->setXColumn(columnX);
	graph->setYColumn(columnY);
	graph->setFillMode(JKQTPBarGraphBase::FillMode::TwoColorFilling);
	graph->fillStyleBelow().setFillColor(QColor("red"));
	ui->resultBarPlot->addGraph(graph);

	graph->setFillMode(JKQTPBarGraphBase::FillMode::FunctorFilling);
	graph->setFillBrushFunctor(
		[habitDailyGoal](double key, double value)
		{
			double hValue = value / static_cast<double>(habitDailyGoal * 4) * 0.35;
			hValue = hValue > 0.35 ? 0.35 : hValue;
			hValue = hValue < 0.08 ? 0.08 : hValue;
			return QBrush{QColor::fromHsvF(hValue, 1.0, 1.0)};
		});

	graph->setWidth(0.5);
	JKQTPXYGraphLabels* graphLabels =
		new JKQTPXYGraphLabels{JKQTPXYGraphLabels::XValueLabel, ui->resultBarPlot};
	graphLabels->setXColumn(graph->getXColumn());
	graphLabels->setYColumn(graph->getYColumn());

	graphLabels->setLabelPosition(JKQTPGLabelAwayFromXAxis);
	graphLabels->setTextFontSize(10);
	graphLabels->setCustomLabelGenerator([](double xVal, double yVal, int b) {	  //
		QString line_1 = QString::number(static_cast<int>(yVal));
		//		QString line_2 = QDateTime::fromMSecsSinceEpoch(xVal).toString("dd");
		return line_1;	  // + "\\\\" + line_2;
	});
	graphLabels->setDrawLabelBoxFrame(true);
	ui->resultBarPlot->addGraph(graphLabels);
}

void ResultsBarGraph::setPlotterSettings()
{
	ui->resultBarPlot->getXAxis()->setMinorTicks(0);
	ui->resultBarPlot->getYAxis()->setMinorTicks(0);
	ui->resultBarPlot->getXAxis()->setTickWidth(0.0);
	ui->resultBarPlot->getYAxis()->setTickWidth(0.0);
	//	ui->resultBarPlot->deregisterMouseWheelAction(Qt::NoModifier);
	//	ui->resultBarPlot->registerMouseWheelAction(
	//		Qt::NoModifier, JKQTPMouseWheelActions::jkqtpmwaPanByWheel);
	//	ui->relativeResultPlot->registerMouseDragAction(
	//		Qt::LeftButton, Qt::ControlModifier,
	// static_cast<JKQTPMouseDragActions>(0));

	//	ui->resultBarPlot->getYAxis()->setTickPrintfFormat("po\nta\nto");

	ui->resultBarPlot->setToolbarEnabled(false);

	ui->resultBarPlot->setGrid(false);
	ui->resultBarPlot->setMousePositionShown(false);
	//	ui->resultBarPlot->getXAxis()->setAxisLabel("Days");
	ui->resultBarPlot->getXAxis()->setTickLabelType(JKQTPCALTdatetime);
	ui->resultBarPlot->getXAxis()->setTickDateTimeFormat("dd\\\\MMM");
	//	ui->resultBarPlot->getYAxis()->setAxisLabel("Result");
	ui->resultBarPlot->getYAxis()->setTickLabelFontSize(13);
	//	ui->resultBarPlot->getXAxis()->setTickLabelAngle(-60);
	ui->resultBarPlot->getXAxis()->setTickLabelColor(QColor::fromRgb(255, 255, 0));
	ui->resultBarPlot->getXAxis()->setTickLabelFontSize(11);
	ui->resultBarPlot->getYAxis()->setAutoAxisSpacing(false);

	int tickSpacing = 5;
	ui->resultBarPlot->getYAxis()->setUserTickSpacing(tickSpacing);
	ui->resultBarPlot->getXAxis()->setAutoAxisSpacing(false);
	ui->resultBarPlot->getXAxis()->setUserTickSpacing(DAY_IN_MSEC);

	ui->resultBarPlot->zoomToFit();

	QDateTime curDate{currentDate(), QTime{}};
	QDateTime stDate{curDate.addDays(-20)};
	ui->resultBarPlot->zoom(stDate.toMSecsSinceEpoch() - DAY_IN_MSEC / 1.5,
		curDate.toMSecsSinceEpoch() + DAY_IN_MSEC / 1.5, 0,
		maxValue(mHabit->getDatesStatus()) + 2 * tickSpacing - 1);

	ui->resultBarPlot->resize(400, 300);
}

void ResultsBarGraph::setPlotterAbsoluteX()
{
	const QMap<QDate, int>& habitData = mHabit->getDatesStatus();
	QDateTime				startDate{habitData.begin().key(), QTime{}};
	QDateTime				endDate{std::prev(habitData.end()).key(), QTime{}};
	ui->resultBarPlot->setAbsoluteX(startDate.toMSecsSinceEpoch() - DAY_IN_MSEC * 2.5,
		endDate.toMSecsSinceEpoch() + DAY_IN_MSEC * 2.5);
}
