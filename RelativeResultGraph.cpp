#include "RelativeResultGraph.hpp"

#include "CircleProgressBar.hpp"
#include "Settings.hpp"
#include "ui_RelativeResultGraph.h"

#include <jkqtplotter/graphs/jkqtpbarchart.h>
#include <jkqtplotter/graphs/jkqtpfilledcurve.h>
#include <jkqtplotter/graphs/jkqtpgraphlabels.h>
#include <jkqtplotter/jkqtplotter.h>

#include <QDateTime>
#include <QHash>
#include <cmath>

namespace
{

const double DAY_IN_MSEC = 86400000;
const double WEEK_IN_MSEC = DAY_IN_MSEC * 7.0;
const double MONTH_IN_MSEC = DAY_IN_MSEC * 30.0;
const double YEAR_IN_MSEC = DAY_IN_MSEC * 365;

enum class Color
{
	Pink = 0,
	Green
};

using TimePeriod = RelativeResultGraph::TimePeriod;
// doubtful
QMap<TimePeriod, QString> TPtoQString{
	{TimePeriod::Day, "day"},		  //
	{TimePeriod::Week, "week"},		  //
	{TimePeriod::Month, "month"},	  //
	{TimePeriod::Year, "year"},		  //
	{TimePeriod::N_Days, "n_days"}	  //
};
// doubtful
QMap<QString, TimePeriod> QStringtoTP{
	{"day", TimePeriod::Day},		  //
	{"week", TimePeriod::Week},		  //
	{"month", TimePeriod::Month},	  //
	{"year", TimePeriod::Year},		  //
	{"n_days", TimePeriod::N_Days}	  //
};

QString toQString(TimePeriod timePeriod)
{
	auto found = TPtoQString.find(timePeriod);
	assert(found != TPtoQString.end());

	return found.value();
}

TimePeriod toTP(const QString timePeriod)
{
	auto found = QStringtoTP.find(timePeriod);
	qDebug() << timePeriod;
	assert(found != QStringtoTP.end());

	return found.value();
}

double getTimePeriodInMSec(TimePeriod timePeriod, int nDaysPeriodSize = -1)
{
	switch (timePeriod)
	{
		using enum RelativeResultGraph::TimePeriod;
		case Day:
			return DAY_IN_MSEC;
		case Week:
			return WEEK_IN_MSEC;
		case Month:
			return MONTH_IN_MSEC;
		case Year:
			return YEAR_IN_MSEC;
		case N_Days:
		{
			assert(nDaysPeriodSize > 0);
			return DAY_IN_MSEC * nDaysPeriodSize;
		}
		default:
			return -1;
	}
}

int getTimePeriodNumber(const QDate& date, TimePeriod timePeriod)
{
	switch (timePeriod)
	{
		using enum RelativeResultGraph::TimePeriod;
		case Day:
			return date.day();
		case Week:
			return date.weekNumber();
		case Month:
			return date.month();
		case Year:
			return date.year();
		default:
			return -1;
	}
}

int getDayNumberInTimePeriod(const QDate& date, TimePeriod timePeriod)
{
	switch (timePeriod)
	{
		using enum RelativeResultGraph::TimePeriod;
		case Day:
			return 1;
		case Week:
			return date.dayOfWeek();
		case Month:
			return date.day();
		case Year:
			return date.dayOfYear();
		default:
			return -1;
	}
}

QDate getFirstDateOfPeriod(const QDate& date, TimePeriod timePeriod)
{
	int timePeriodNumber = getDayNumberInTimePeriod(date, timePeriod);
	return date.addDays(1 - timePeriodNumber);
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

using TP = TimePeriod;

RelativeResultGraph::RelativeResultGraph(QWidget* parent)
	: QWidget{parent}
	, ui{new Ui::RelativeResultGraph}
{
}

RelativeResultGraph::~RelativeResultGraph()
{
	delete ui;
}

void RelativeResultGraph::init(const Habit* habit)
{
	// I use habitData.begin() later, change this assert later
	assert(habit->getDatesStatus().size() > 0);
	ui->setupUi(this);
	mHabit = habit;
	mSettings = getLocalSettings();

	setTheme(*ui->relativeResultPlot, Color::Green);
	initGraphInfoWidgets();

	bool	 success = addGraph();
	QWidget* widgetToShow = success ? ui->relativeResultPlot : ui->noDataWidget;
	setPlotterSettings();
	setPlotterZoom();
	ui->relativeResultPlot->show();
}

RelativeResultGraph::LocalSettings RelativeResultGraph::getLocalSettings()
{
	int nDays =
		toInt(Settings::instance().getSetting(mHabit->getName(), "n_days_spin_box_rrg"));
	int tickCBIndx = toInt(
		Settings::instance().getSetting(mHabit->getName(), "x_tick_combo_box_indx_rrg"));
	qDebug() << tickCBIndx << " " << ui->xTickComboBox->count();
	TimePeriod tp = toTP(ui->xTickComboBox->itemText(tickCBIndx));

	return LocalSettings{tp, nDays};
}

[[nodiscard("If false, no graph will be added")]] //
bool RelativeResultGraph::addGraph()
{
	const QMap<QDate, int>& habitData = mHabit->getDatesStatus();
	TimePeriod				timePeriod = mSettings.timePeriod;
	int						nDaysPeriodSize = mSettings.nDays;

	JKQTPDatastore* ds = ui->relativeResultPlot->getDatastore();

	size_t columnX = ds->addColumn("days");
	size_t columnY = ds->addColumn("relative_score");

	if (timePeriod != TimePeriod::N_Days)
	{
		double prevPeriodResult = 0.0;
		double curPeriodResult = 0.0;
		int	   i = 0;
		int	   iterCnt = 0;
		QDate  date{habitData.begin().key()};
		//		int	   curMonthNumber = date.month();
		int curPeriodNumber = getTimePeriodNumber(date, timePeriod);
		//		int	  periodSize = 0;
		for (auto it = habitData.begin(); it != habitData.end(); ++it)
		{
			//			int	   nextDayWeekNumber = date.addDays(1).weekNumber();
			//			int	   nextDayMonthNumber = date.addDays(1).month();
			int	   nextDayPeriodNumber = getTimePeriodNumber(date.addDays(1), timePeriod);
			double dayResult = static_cast<double>(it.value());
			curPeriodResult += dayResult;
			//			periodSize += 1;

			// if current day is Sunday (day before Monday)
			if (nextDayPeriodNumber != curPeriodNumber ||
				std::next(it) == habitData.end())
			{
				double score = 0.0;
				if (prevPeriodResult == 0)
				{
					if (curPeriodResult > 0)
						score = 100;
					else
						score = 0;
				}
				else
				{
					score = (curPeriodResult / prevPeriodResult - 1) * 100;
					score = score > 300 ? 300 : score;
				}
				ds->appendToColumn(
					columnX, QDateTime{getFirstDateOfPeriod(date, timePeriod), QTime{}}
								 .toMSecsSinceEpoch());
				ds->appendToColumn(columnY, static_cast<int>(score));
				qDebug() << i << " "
						 << QDateTime{getFirstDateOfPeriod(date, timePeriod), QTime{}}
								.toString(Qt::ISODate)
						 << " " << score;

				i += 1;
				curPeriodNumber = nextDayPeriodNumber;
				prevPeriodResult = curPeriodResult;
				curPeriodResult = 0.0;
			}
			date = date.addDays(1);
		}
		if (i < 2)
		{
			// insufficient data
			return false;
		}
	}
	else
	{
		int		  startPart = habitData.size() % nDaysPeriodSize;
		bool	  firstTime = true;
		double	  prevPeriodResult = 0.0;
		double	  curPeriodResult = 0.0;
		int		  i = 0;
		int		  iterCnt = 0;
		QDateTime date{habitData.begin().key(), QTime{}};
		for (auto it = habitData.begin(); it != habitData.end(); ++it, ++iterCnt)
		{
			double dayResult = static_cast<double>(it.value());
			curPeriodResult += dayResult;
			qDebug() << "date: " << date << " " << iterCnt;

			// first period (current element included)
			// possible to be not used(if size%3 == 0)
			if (firstTime && (iterCnt + 1) == startPart)
			{
				double score = 0.0;
				if (prevPeriodResult == 0)
				{
					if (curPeriodResult > 0)
						score = 100;
					else
						score = 0;
				}
				else
				{
					score = (curPeriodResult / prevPeriodResult - 1) * 100;
					score = score > 300 ? 300 : score;
				}

				ds->appendToColumn(columnX, date.toMSecsSinceEpoch());
				ds->appendToColumn(columnY, score);
				qDebug() << i << ": " << date.toString(Qt::ISODate) << " " << score;

				i += 1;
				prevPeriodResult = curPeriodResult;
				curPeriodResult = 0.0;
				date = date.addDays(startPart);
				firstTime = false;
			}
			// current element included
			else if ((iterCnt - startPart + 1) % nDaysPeriodSize == 0)
			{
				double score = 0.0;
				if (prevPeriodResult == 0)
				{
					if (curPeriodResult > 0)
						score = 100;
					else
						score = 0;
				}
				else
				{
					score = (curPeriodResult / prevPeriodResult - 1) * 100;
					score = score > 300 ? 300 : score;
				}

				ds->appendToColumn(columnX, date.toMSecsSinceEpoch());
				ds->appendToColumn(columnY, score);
				qDebug() << i << ": " << date.toString(Qt::ISODate) << " " << score;

				i += 1;
				prevPeriodResult = curPeriodResult;
				curPeriodResult = 0.0;
				date = date.addDays(nDaysPeriodSize);
				firstTime = false;
			}
		}
		if (i < 2)
		{
			// insufficient data
			return false;
		}
	}

	JKQTPBarGraphBase* graph = new JKQTPBarVerticalGraph{ui->relativeResultPlot};
	graph->setXColumn(columnX);
	graph->setYColumn(columnY);
	graph->setFillMode(JKQTPBarGraphBase::FillMode::TwoColorFilling);
	graph->fillStyleBelow().setFillColor(QColor("red"));

	graph->setWidth(0.4);

	JKQTPXYGraphLabels* graphLabels =
		new JKQTPXYGraphLabels{JKQTPXYGraphLabels::XValueLabel, ui->relativeResultPlot};
	graphLabels->setXColumn(graph->getXColumn());
	graphLabels->setYColumn(graph->getYColumn());

	graphLabels->setLabelPosition(JKQTPGLabelAwayFromXAxis);
	graphLabels->setTextFontSize(10);
	graphLabels->setCustomLabelGenerator([](double xVal, double yVal, int b) {	  //
		QString line_1 =
			QString{yVal >= 0 ? "+" : ""} + QString::number(static_cast<int>(yVal)) + "%";
		QString line_2 = QDateTime::fromMSecsSinceEpoch(xVal).toString("dd.MM");
		return "\\begin{align}" + line_1 + "\\\\" + line_2 + "\\end{align}";
	});
	graphLabels->setDrawLabelBoxFrame(true);

	ui->relativeResultPlot->addGraph(graph);
	ui->relativeResultPlot->addGraph(graphLabels);

	setPlotterAbsoluteX();
	return true;
}

void RelativeResultGraph::setPlotterSettings()
{
	TimePeriod timePeriod = mSettings.timePeriod;
	int		   nDaysPeriodSize = mSettings.nDays;

	// bad
	ui->relativeResultPlot->getXAxis()->setMinorTicks(0);
	ui->relativeResultPlot->getYAxis()->setMinorTicks(0);
	ui->relativeResultPlot->getXAxis()->setTickWidth(0.0);
	ui->relativeResultPlot->getYAxis()->setTickWidth(0.0);

	ui->relativeResultPlot->setGrid(false);
	ui->relativeResultPlot->setMousePositionShown(false);
	//	ui->relativeResultPlot->getXAxis()->setAxisLabel("Days");
	ui->relativeResultPlot->getXAxis()->setTickLabelType(JKQTPCALTdatetime);
	// bad
	ui->relativeResultPlot->getXAxis()->setTickDateTimeFormat("dd\\\\MMM");
	//	ui->relativeResultPlot->getYAxis()->setAxisLabel("Relative Score (%)");
	//	ui->relativeResultPlot->getYAxis()->setTickPrintfFormat("y=25%f");
	ui->relativeResultPlot->getYAxis()->setTickLabelFontSize(13);
	ui->relativeResultPlot->getYAxis()->setTickUnitName("%");
	//	ui->relativeResultPlot->getXAxis()->setTickColor(QColor::fromRgb(0, 255,
	// 255)); 	ui->relativeResultPlot->getXAxis()->setTickInsideLength(50);
	//	ui->relativeResultPlot->getXAxis()->setTickLabelAngle(-60);
	//	ui->relativeResultPlot->getXAxis()->setWid
	ui->relativeResultPlot->getXAxis()->setTickLabelColor(QColor::fromRgb(255, 255, 0));
	//	ui->relativeResultPlot->getXAxis()->setTickLabelDistance(50);
	ui->relativeResultPlot->getXAxis()->setTickLabelFontSize(11);
	//	ui->relativeResultPlot->getXAxis()->setTickMode(1);
	//	ui->relativeResultPlot->getXAxis()->setTickOutsideLength(50);
	//	ui->relativeResultPlot->getXAxis()->setTickSpacing(86400000);
	//	ui->relativeResultPlot->getXAxis()->calcrelativeResultPlotScaling(true);
	//	ui->relativeResultPlot->getXAxis()->setTickWidth(50);
	ui->relativeResultPlot->getYAxis()->setAutoAxisSpacing(false);
	ui->relativeResultPlot->getYAxis()->setUserTickSpacing(50);
	ui->relativeResultPlot->getXAxis()->setAutoAxisSpacing(false);
	ui->relativeResultPlot->getXAxis()->setUserTickSpacing(
		getTimePeriodInMSec(timePeriod, nDaysPeriodSize));
	//	ui->relativeResultPlot->getXAxis()->setUserTickSpacing((DAY_IN_MSEC));
	//	ui->relativeResultPlot->zoomToFit();

	//	ui->relativeResultPlot->zoomToFit();

	// 5. show ui->relativeResultPlotter and make it a decent size
	//	ui->relativeResultPlot->resize(400, 300);
}

void RelativeResultGraph::setPlotterAbsoluteX()
{
	TimePeriod timePeriod = mSettings.timePeriod;
	int		   nDaysPeriodSize = mSettings.nDays;

	auto*  ds = ui->relativeResultPlot->getDatastore();
	size_t columnX = ds->getColumnNum("days");
	double startDateInMSec = ds->get(columnX, 0);

	const QMap<QDate, int>& habitData = mHabit->getDatesStatus();
	assert(habitData.size() > 0);

	QDateTime startDate{QDateTime::fromMSecsSinceEpoch(startDateInMSec)};
	QDateTime endDate{std::prev(habitData.end()).key(), QTime{}};
	double	  periodInMSec = getTimePeriodInMSec(timePeriod, nDaysPeriodSize);
	//	ui->relativeResultPlot->setAbsoluteX(
	//		startDate.toMSecsSinceEpoch() - DAY_IN_MSEC - periodInMSec * 1.5,
	//		endDate.toMSecsSinceEpoch() + periodInMSec * 1.5);
	ui->relativeResultPlot->setAbsoluteX(startDate.toMSecsSinceEpoch() - periodInMSec,
		endDate.toMSecsSinceEpoch() + periodInMSec);

	qDebug() << "-> startDate: " << startDate.toString(Qt::ISODate)
			 << " endDate: " << endDate.toString(Qt::ISODate);
}

void RelativeResultGraph::setPlotterZoom()
{
	TimePeriod timePeriod = mSettings.timePeriod;
	int		   nDaysPeriodSize = mSettings.nDays;

	double TPMsec = getTimePeriodInMSec(timePeriod, nDaysPeriodSize);
	//	const QMap<QDate, int>& habitData = mHabit->getDatesStatus();
	//	assert(habitData.size() > 0);
	//	QDateTime newestDate{std::prev(habitData.end()).key(), QTime{}};

	QDateTime curDate{currentDate(), QTime{}};
	//	QDateTime stDate{curDate.addDays(-15 * 1)};
	QDateTime stDate{curDate.addMSecs(static_cast<qint64>(-15 * TPMsec))};

	ui->relativeResultPlot->setXY(stDate.toMSecsSinceEpoch() - TPMsec,
		curDate.toMSecsSinceEpoch() + TPMsec, -145, 345);
	//	ui->relativeResultPlot->setXY(newestDate.toMSecsSinceEpoch() - MONTH_IN_MSEC,
	//		static_cast<double>(newestDate.toMSecsSinceEpoch()), -145, 345);
}

void RelativeResultGraph::initGraphInfoWidgets()
{
	initXTickGroupBox();
}

void RelativeResultGraph::initXTickGroupBox()
{
	for (const QString& tpItem : TPtoQString)
	{
		ui->xTickComboBox->addItem(tpItem);
	}

	bool isNDays = (ui->xTickComboBox->currentText() == toQString(TP::N_Days));
	ui->nDaysSpinBox->setEnabled(isNDays);

	/*** [START: Connections] ****/
	connect(ui->xTickComboBox, &QComboBox::currentIndexChanged, this,
		[this](int currentIndx)
		{
			bool isNDays =
				(ui->xTickComboBox->itemText(currentIndx) == toQString(TP::N_Days));
			ui->nDaysSpinBox->setEnabled(isNDays);
		});

	connect(ui->tickApplyButton, &QPushButton::clicked, this,
		[this]()
		{
			ui->relativeResultPlot->clearGraphs(true);
			QString	   timePeriodQS = ui->xTickComboBox->currentText();
			TimePeriod timePeriod = QStringtoTP[timePeriodQS];
			int		   nDaysPeriod =
				   timePeriod == TimePeriod::N_Days ? ui->nDaysSpinBox->value() : -1;
			mSettings = LocalSettings{timePeriod, nDaysPeriod};

			bool	 success = addGraph();
			QWidget* widgetToShow = success ? ui->relativeResultPlot : ui->noDataWidget;
			ui->stackedWidget->setCurrentWidget(widgetToShow);

			setPlotterSettings();
			setPlotterAbsoluteX();
			setPlotterZoom();

			Settings::instance().setSetting(mHabit->getName(),
				"x_tick_combo_box_indx_rrg", ui->xTickComboBox->currentIndex());
			Settings::instance().setSetting(
				mHabit->getName(), "n_days_spin_box_rrg", ui->nDaysSpinBox->value());
		});
	/*** [END: Connections] ****/

	int nDays =
		toInt(Settings::instance().getSetting(mHabit->getName(), "n_days_spin_box_rrg"));
	int tickCBIndx = toInt(
		Settings::instance().getSetting(mHabit->getName(), "x_tick_combo_box_indx_rrg"));
	ui->nDaysSpinBox->setValue(nDays);
	ui->xTickComboBox->setCurrentIndex(tickCBIndx);
}
