#include "MainWindow.hpp"

#include "Constants.hpp"
#include "HabTableView.hpp"
#include "HabitDelegate.hpp"
#include "HabitDialog.hpp"
#include "HabitModel.hpp"
#include "ImportDialog.hpp"
#include "RegexInputDialog.hpp"
#include "StatusProxyModel.hpp"
#include "Utility.hpp"
#include "config.h"
#include "ui_MainWindow.h"

#include <QDockWidget>
#include <QFileDialog>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QVariant>
#include <string>

////
// #include <QTimer>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow{parent}
	, ui{new Ui::MainWindow}
	, mSettingsDialog{new SettingsDialog{this}}
{
	ui->setupUi(this);
	setWindowTitle(" ");

	qDebug() << APPLICATION_NAME;

	HabitModel*		  habitModel = new HabitModel{false, this};
	StatusProxyModel* statusModel = new StatusProxyModel{this};
	statusModel->setSourceModel(habitModel);

	QVBoxLayout*  mainLayout = new QVBoxLayout{this};
	HabTableView* tableView = new HabTableView{this};
	QPushButton*  addHabitButton = new QPushButton{"Add habit", this};
	QPushButton*  importButton = new QPushButton{"Import habits from .db file", this};
	QPushButton*  syncButton = new QPushButton{"Synchronize (local)", this};
	QPushButton*  syncButtonRemote = new QPushButton{"Synchronize (remote)", this};

	tableView->setModel(statusModel);
	tableView->setHabitDelegate(new HabitDelegate{tableView});
	tableView->setSelectionMode(QAbstractItemView::NoSelection);
	tableView->setFocusPolicy(Qt::NoFocus);
	tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableView->setShowGrid(false);
	tableView->verticalHeader()->setVisible(false);
	tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	connect(addHabitButton, &QPushButton::clicked, this,
		[habitModel]()
		{
			HabitDialog habitDialog{
				"New habit", HabitDialog::Mode::Add, "Create", "Discard"};
			if (habitDialog.exec() == QDialog::Accepted)
			{
				QString			  name = habitDialog.getName();
				QDate			  date = habitDialog.getStartDate();
				Habit::Type		  type = habitDialog.getType();
				QString			  units = habitDialog.getUnits();
				int				  dailyGoal = habitDialog.getDailyGoal();
				Habit::RepeatInfo repeatInfo = habitDialog.getRepeatInfo();

				habitModel->addHabit(
					Habit{name, date, type, units, dailyGoal, repeatInfo});
			}
		});

	connect(importButton, &QPushButton::clicked, this,
		[this, habitModel]()
		{
			QString filename = QFileDialog::getOpenFileName(this,
				tr("Select import .db File"), "/home/jonathan/", tr("Databases (*.db)"));

			if (filename.isEmpty())
			{
				return;
			}
			//
			ImportDialog impDialog{filename, "Approve"};
			if (impDialog.exec() == QDialog::Accepted)
			{
				const std::vector<Habit>& habits = impDialog.getNewHabits();
				for (auto& habit : habits)
				{
					habitModel->addHabit(habit);
				}
			}
		});

	connect(syncButton, &QPushButton::clicked, this,
		[this]()
		{
			std::string cmd =
				"scp " + DATABASE_FILENAME.toStdString() +
				" local_server:/home/ellain/Apps/jonathan/habit_daily_bot/data/";
			bool success = exec(cmd.c_str()) == 0;
			if (success)
			{
				QMessageBox::information(
					this, tr("Synchronization"), tr("Synchronized successfully"));
			}
			else
			{
				QMessageBox::warning(
					this, tr("Synchronization"), tr("Synchronization failed!"));
			}
		});

	connect(syncButtonRemote, &QPushButton::clicked, this,
		[this]()
		{
			bool	ok;
			QString ip = RegexInputDialog::getText(this, tr("remote server"),
				tr("remote server ip:"), tr("192.168.0.1"),
				QRegularExpression{
					R"(^(?:(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])(\.(?!$)|$)){4}$)"},
				&ok);
			if (ok && !ip.isEmpty())
			{
				std::string cmd = "scp " + DATABASE_FILENAME.toStdString() + " " +
								  ip.toStdString() +
								  ":/home/ellain/Apps/jonathan/habit_daily_bot/data/";
				bool success = exec(cmd.c_str()) == 0;
				if (success)
				{
					QMessageBox::information(this, tr("Synchronization"),
						tr("Synchronized successfully") + "\n(" + ip + ")");
				}
				else
				{
					QMessageBox::warning(this, tr("Synchronization"),
						tr("Synchronization failed: ") + "\n(" + ip + ")");
				}
			}
		});

	connect(mSettingsDialog, &SettingsDialog::SIGNAL_visibilityChanged, this,
		[this](bool visible) {	  //
			this->centralWidget()->setEnabled(!visible);
		});
	connect(mSettingsDialog, &SettingsDialog::SIGNAL_optionChanged, this,
		[this, habitModel]() {	  //
			habitModel->updateCachedData();
		});

	mainLayout->addWidget(tableView);
	mainLayout->addWidget(addHabitButton);
	mainLayout->addWidget(importButton);
	mainLayout->addWidget(syncButton);
	mainLayout->addWidget(syncButtonRemote);
	centralWidget()->setLayout(mainLayout);

	setupMenuBar();

	//	QTimer::singleShot(2500, [this]() { centralWidget()->setEnabled(false); });
	//	Settings* settings = new Settings{this};
	//	settings->show();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::setupMenuBar()
{
	QMenuBar* menuBar = new QMenuBar{this};
	//	QMenuBar* menuBar = ui->menubar;

	QMenu* optionsMenu = new QMenu{menuBar};
	QMenu* helpMenu = new QMenu{menuBar};

	QAction* settingsAction = new QAction{this};
	QAction* nopAction = new QAction{this};

	menuBar->addAction(optionsMenu->menuAction());
	menuBar->addAction(helpMenu->menuAction());

	optionsMenu->addAction(settingsAction);
	optionsMenu->setTitle("Options");
	settingsAction->setText("Settings...");

	helpMenu->addAction(nopAction);
	helpMenu->setTitle("Help");
	nopAction->setText("nop");

	this->setMenuBar(menuBar);
	this->setStatusBar(new QStatusBar(this));
	connect(settingsAction, &QAction::triggered, mSettingsDialog, &QDialog::show);

	menuBar->setGeometry(0, 0, this->width(), 100);
}
