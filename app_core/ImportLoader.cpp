#include "ImportLoader.hpp"

#include "Constants.hpp"
#include "DatabaseManager.hpp"
#include "Utility.hpp"

#include <QDate>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

// std
#include <chrono>
#include <unordered_map>

namespace ch = std::chrono;

namespace
{
const QString HabitsTableName = "Habits";
const QString DatesTableName = "Repetitions";
}	 // namespace

std::vector<Habit> loadImportedHabits(const QString& path)
{
	/********** [Connect database] *************/
	QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE", "import_connection");
	database.setDatabaseName(path);
	bool openStatus = database.open();
	assert(("mDatabase->open() failed!") && openStatus);

	assert((database.tables().contains(HabitsTableName)) &&
		   "no table named ???, probably .db file corrupted");
	assert((database.tables().contains(DatesTableName)) &&
		   "no table named ???, probably .db file corrupted");

	/********* [Load Habits Basic Info] *********************/
	QSqlQuery query_1{"SELECT id, name, type, target_value, unit from " +
						  HabitsTableName + " ORDER BY id;",
		database};
	query_1.exec();
	DatabaseManager::debugQuery(query_1);
	//    std::vector<Habit> habits;
	std::unordered_map<int, Habit> habits;
	while (query_1.next())
	{
		Habit habit;
		habit.setId(toInt(query_1.value("id")));
		habit.setName(toQString(query_1.value("name")));
		//        habit.setStartDate(toQDate(query_1.value("startDate")));
		habit.setType(static_cast<Habit::Type>(toInt(query_1.value("type"))));
		habit.setUnits(toQString(query_1.value("unit")));
		habit.setDailyGoal(toInt(query_1.value("target_value")));

		habits.insert(std::make_pair(habit.getId(), std::move(habit)));
	}
	/************** [Load Dates Status Info] *************/
	QSqlQuery query_2{"SELECT * from " + DatesTableName + " ORDER BY id;", database};
	query_2.exec();
	DatabaseManager::debugQuery(query_2);
	std::unordered_map<int, QDate> habitsEarliestDate;
	for (auto& habit : habits)
	{
		habitsEarliestDate.insert(std::make_pair(habit.first, currentDate()));
	}
	while (query_2.next())
	{
		int		habitId = toInt(query_2.value("habit"));
		int64_t timestampMilli = toInt64(query_2.value("timestamp"));
		int		timestamp = toInt32(timestampMilli / 1000);
		int		value = toInt(query_2.value("value"));

		auto iter = habits.find(habitId);
		assert((iter != habits.end()) && "HabitId not found!");
		auto& habit = iter->second;

		ch::time_point<ch::system_clock> chronoTimestamp{ch::seconds{timestamp}};
		ch::year_month_day				 ymd{ch::floor<ch::days>(chronoTimestamp)};

		int status = value;
		if (status == 2)
		{
			status = INT_TRUE_VALUE;
		}
		else if (status > 1000 && status % 1000 == 0)
		{
			status /= 1000;
		}
		QDate date = toQDate(ymd);
		assert((habitsEarliestDate.contains(habitId)) &&
			   "habitEarliestDate no such habitId");
		const QDate& earliestDate = habitsEarliestDate[habitId];
		if (date < earliestDate)
		{
			habitsEarliestDate[habitId] = date;
		}

		habit.addDateStatus(toQDate(ymd), status);
	}
	/****** [Create and return result vector] *****/
	std::vector<Habit> resultHabits(habits.size());
	size_t			   i = 0;
	for (auto& habitPair : habits)
	{
		auto& habit = habitPair.second;
		assert((habitsEarliestDate.contains(habit.getId())) &&
			   "habitEarliestDate no such habitId");
		habit.setStartDate(habitsEarliestDate[habit.getId()]);
		resultHabits[i++] = std::move(habit);
	}
	return resultHabits;
}
