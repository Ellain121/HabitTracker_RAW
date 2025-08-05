#include "HabitDao.hpp"

#include "Constants.hpp"
#include "DatabaseManager.hpp"
#include "Utility.hpp"

#include <QMap>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QVariant>

//
#include <QDebug>

// std
#include <array>
#include <bitset>
#include <regex>
#include <string>

/***
 * habits_id:
1) name (TEXT)- имя

2) startDate (DATE) - дата в формате yyyy-mm-dd

3) habitType (INT) - 0 или 1, пока что 2 типа, на будущее стоит INT и может быть больше
   Onetime = 0
   Quantative = 1

4) habitUnits(TEXT) - any text, for example 5 minutes

5) dailyGoal (INT) - 1 Means goal is 1 or it's true for a Onetime type of habit

6) repeatFrequence (INT) - повтор паттерна, чаще всего настраивается именно патерн,
поэтому чаще всего значение 1, но не обязательно, допустим каждые 2 недели паттерн в ПНД,
СР, ПЯТНИЦА. Т.е. паттерн 3 дня недели, а повтор этого паттерна - 2

7) repeatDateUnit (INT) - тип паттерна, может быть днем, неделью, месяцем, годом, колвом
дней. колво дней имеется ввиду патерн на любое количество дней. Тоже самое если выбрать
просто месяц, только доп функция в виде не колво дней месяца, а колво дней свое.

8) repeatPattern (INT) - побитовое отображение паттерна 1 день - 1 бит. Максимальный
кастомный размер паттерна = 30 дней, максимальный размер паттерна = 31 день(когда в месяце
31 день)

9) repeatPatternSize (INT) - Значение которое имеет смысл только когда выбирается
RepeanNDaysPattern, в остальных случаях это значение идет INT_NULL_VALUE
***/
namespace
{
const QString TABLE_NAME_ID = "habits_id";
const QString TABLE_NAME_DATA = "habits_data";
const QString createIdTableQuery = QString{"CREATE TABLE IF NOT EXISTS "	//
										   + TABLE_NAME_ID +
										   " (id INTEGER PRIMARY KEY AUTOINCREMENT,"
										   "name TEXT NOT NULL,"
										   "startDate DATE NOT NULL,"
										   "habitType INT NOT NULL,"
										   "habitUnits TEXT NOT NULL,"
										   "dailyGoal INT NOT NULL,"
										   "repeatFrequence INT NOT NULL,"
										   "repeatDateUnit INT NOT NULL,"
										   "repeatPattern INT,"
										   "repeatEnds TEXT,"
										   "repeatPatternSize INT"
										   ");"};
std::pair<QString, QString> getDataTableCreationQuery()
{
	return std::make_pair(QString{"CREATE TABLE IF NOT EXISTS " + TABLE_NAME_DATA +
								  " (id INTEGER PRIMARY KEY AUTOINCREMENT,"
								  "  date DATE UNIQUE NOT NULL,"
								  "  dayofweek INT NOT NULL,"
								  "  weekday TEXT NOT NULL,"
								  "  quarter INT NOT NULL,"
								  "  year INT NOT NULL,"
								  "  month INT NOT NULL,"
								  "  day INT NOT NULL"
								  ");"},
		QString{"INSERT"
				"  OR ignore INTO " +
				TABLE_NAME_DATA +
				" (date, dayofweek, weekday, quarter, year, month, day)"
				"SELECT *"
				"FROM ("
				"  WITH RECURSIVE dates(date) AS ("
				"    VALUES('2023-01-01')"
				"    UNION ALL"
				"    SELECT date(date, '+1 day')"
				"    FROM dates"
				"    LIMIT 5000"
				"  )"
				"  SELECT date,"
				"    (CAST(strftime('%w', date) AS INT) + 6) % 7 AS dayofweek,"
				"    CASE"
				"      (CAST(strftime('%w', date) AS INT) + 6) % 7"
				"      WHEN 0 THEN 'Monday'"
				"      WHEN 1 THEN 'Tuesday'"
				"      WHEN 2 THEN 'Wednesday'"
				"      WHEN 3 THEN 'Thursday'"
				"      WHEN 4 THEN 'Friday'"
				"      WHEN 5 THEN 'Saturday'"
				"      ELSE 'Sunday'"
				"    END AS weekday,"
				"    CASE"
				"      WHEN CAST(strftime('%m',date) AS INT) BETWEEN 1 AND 3 THEN 1"
				"      WHEN CAST(strftime('%m',date) AS INT) BETWEEN 4 AND 6 THEN 2"
				"      WHEN CAST(strftime('%m',date) AS INT) BETWEEN 7 AND 9 THEN 3"
				"      ELSE 4"
				"    END AS quarter,"
				"    CAST(strftime('%Y',date) AS INT) AS year,"
				"    CAST(strftime('%m',date) AS INT) AS month,"
				"    CAST(strftime('%d',date) AS INT) AS day"
				"  FROM dates"
				") ORDER BY date;"});	 //
}

std::array<std::string, 6> removeHabitColumnQueries(
	std::string schema, const std::string& exclude)
{
	std::regex	re{",[ ]*" + exclude + "[ ]*INT"};
	std::string replacement{""};
	schema = regex_replace(schema, re, replacement);

	size_t		i = schema.find(" TABLE");
	std::string schema_temp = schema.substr(0, i) + " TEMPORARY" + schema.substr(i);

	re = std::regex{TABLE_NAME_DATA.toStdString()};
	replacement = std::string{"temp_copy"};
	std::string query_1 = regex_replace(schema_temp, re, replacement) + ";";
	//	std::cerr << "===============================" << std::endl;
	//	std::cerr << query_1 << std::endl;

	re = std::regex{"[\\(|,] *([\\w]+)"};
	replacement = std::string{"$1, "};
	std::string columns =
		regex_replace(schema, re, replacement, std::regex_constants::format_no_copy);
	columns.pop_back();
	columns.pop_back();

	std::string query_2{"INSERT INTO temp_copy SELECT " + columns + " FROM " +
						TABLE_NAME_DATA.toStdString() + " ORDER BY id;"};
	//	std::cerr << "===============================" << std::endl;
	//	std::cerr << query_2 << std::endl;

	std::string query_3{"DROP TABLE " + TABLE_NAME_DATA.toStdString() + ";"};
	//	std::cerr << "===============================" << std::endl;
	//	std::cerr << query_3 << std::endl;

	std::string query_4 = schema + ";";
	//	std::cerr << "===============================" << std::endl;
	//	std::cerr << query_4 << std::endl;

	std::string query_5{"INSERT INTO " + TABLE_NAME_DATA.toStdString() + " SELECT " +
						columns + " FROM temp_copy ORDER BY id;"};
	//	std::cerr << "===============================" << std::endl;
	//	std::cerr << query_5 << std::endl;

	std::string query_6 = {"DROP TABLE temp_copy;"};
	//	std::cerr << "===============================" << std::endl;
	//	std::cerr << query_6 << std::endl;

	return std::array<std::string, 6>{
		query_1, query_2, query_3, query_4, query_5, query_6};
}

}	 // namespace

HabitDao::HabitDao(QSqlDatabase& database)
	: mDatabase{database}
{
}

void HabitDao::init() const
{
	if (!mDatabase.tables().contains(TABLE_NAME_ID))
	{
		// table not exist, create one
		QSqlQuery query{mDatabase};
		query.exec(createIdTableQuery);
		DatabaseManager::debugQuery(query);
	}

	if (!mDatabase.tables().contains(TABLE_NAME_DATA))
	{
		// table not exist, create one
		QSqlQuery query{mDatabase};
		auto	  createDataTableQueries = getDataTableCreationQuery();
		query.exec(createDataTableQueries.first);
		DatabaseManager::debugQuery(query);
		query.exec(createDataTableQueries.second);
		DatabaseManager::debugQuery(query);
	}
}

void HabitDao::addHabit(Habit& habit) const
{
	/** Insert basic habit info to TABLE_NAME_ID **/
	QSqlQuery query_1{mDatabase};
	query_1.prepare(
		"INSERT INTO " + TABLE_NAME_ID +
		" (name, startDate, habitType, habitUnits, dailyGoal, repeatFrequence, "
		"repeatDateUnit, repeatPattern, repeatEnds, repeatPatternSize) VALUES (:name, "
		":start_date, :habit_type, :habit_units, :daily_goal, :repeat_frequence, "
		":repeat_date_unit, :repeat_pattern, :repeat_ends, :repeat_pattern_size)");
	query_1.bindValue(":name", habit.getName());
	query_1.bindValue(":start_date", habit.getStartDate().toString(Qt::ISODate));
	query_1.bindValue(":habit_type", static_cast<int>(habit.getType()));
	query_1.bindValue(":habit_units", habit.getUnits().isEmpty() ? "" : habit.getUnits());
	query_1.bindValue(":daily_goal", habit.getDailyGoal());
	query_1.bindValue(":repeat_frequence", habit.getRepeatFrequence());
	query_1.bindValue(":repeat_date_unit", static_cast<int>(habit.getRepeatDateUnit()));
	query_1.bindValue(":repeat_pattern", habit.getRepeatPattern());
	query_1.bindValue(":repeat_ends", habit.getRepeatEnds());
	query_1.bindValue(":repeat_pattern_size", habit.getRepeatPatternSize());
	query_1.exec();
	DatabaseManager::debugQuery(query_1);
	habit.setId(toInt(query_1.lastInsertId()));
	/***********************************************/

	/** Add new Column named id_(:habitIDName) to TABLE_NAME_DATA **/
	QSqlQuery query_2{mDatabase};
	query_2.exec(
		"ALTER TABLE " + TABLE_NAME_DATA + " ADD " + habit.getIDName() + " INT;");
	DatabaseManager::debugQuery(query_2);
	/***************************************************************/

	// if habit is new, we have 0 info about datesStatus
	if (habit.getDatesStatus().empty())
	{
		/** Set all habit column values in TABLE_NAME_DATA from NULL to
		 * INT_FALSE_VALUE WHERE date >= habitStartDate); */
		// if Daily, then fill each date since startDate
		std::array<QString, 4> strQueries = getNullToFalseQueries(habit);
		for (QString& strQuery : strQueries)
		{
			if (strQuery.isEmpty())
				continue;

			QSqlQuery query{mDatabase};
			query.exec(strQuery);
			DatabaseManager::debugQuery(query);

			assert((query.lastError().type() == QSqlError::NoError) && "4 QUERY ERROR");
		}
	}
	else	// if habit load from CSV, we should load datesStatus to the table as well
	{
		const QMap<QDate, int>& datesStatus = habit.getDatesStatus();
		for (auto iter = datesStatus.constBegin(); iter != datesStatus.constEnd(); ++iter)
		{
			updateHabitDateStatus(iter.key(), habit);
		}
	}
	// load habitInfo from previously changed table
	loadHabitInfoFromDataTable(&habit);
}

void HabitDao::updateHabitDateStatus(const QDate& date, const Habit& habit) const
{
	QSqlQuery query{mDatabase};
	query.prepare("UPDATE " + TABLE_NAME_DATA + " SET " + habit.getIDName() +
				  "=(:habit_date_status) WHERE date=(:habit_start_date)");
	query.bindValue(":habit_date_status", habit.getDateStatus(date));
	query.bindValue(":habit_start_date", date.toString(Qt::ISODate));
	query.exec();
	DatabaseManager::debugQuery(query);
}

// Basic info includes name, startDate and habitType
void HabitDao::updateHabitBasicInfo(const Habit& habit) const
{
	QSqlQuery query{mDatabase};
	query.prepare("UPDATE " + TABLE_NAME_ID +
				  " SET name=(:habit_name), startDate=(:habit_start_date), "
				  "habitType=(:habit_type), habitUnits=(:habit_units), "
				  "dailyGoal=(:daily_goal) WHERE id=(:habid_id)");
	query.bindValue(":habit_name", habit.getName());
	query.bindValue(":habit_start_date", habit.getStartDate().toString(Qt::ISODate));
	query.bindValue(":habit_type", static_cast<int>(habit.getType()));
	query.bindValue(":habit_units", habit.getUnits().isEmpty() ? "" : habit.getUnits());
	query.bindValue(":habid_id", habit.getId());
	query.bindValue(":daily_goal", habit.getDailyGoal());
	query.exec();
	DatabaseManager::debugQuery(query);

	_debug_printTable();
}

void HabitDao::removeHabit(const Habit& habit) const
{
	/**** [ Delete habit by id from TABLE_NAME_ID ] *****/
	QSqlQuery query_1{mDatabase};
	query_1.prepare("DELETE FROM " + TABLE_NAME_ID + " WHERE id=(:habit_id)");
	query_1.bindValue(":habit_id", habit.getId());
	query_1.exec();
	DatabaseManager::debugQuery(query_1);

	/**** [ Delete habit column(id_n) from TABLE_NAME_DATA ] *****/
	QSqlQuery query_2{mDatabase};
	query_2.exec("SELECT sql FROM sqlite_master WHERE name='" + TABLE_NAME_DATA + "';");
	DatabaseManager::debugQuery(query_2);
	query_2.next();
	QString tableSchema = toQString(query_2.value("sql"));
	query_2.finish();

	QSqlQuery dropQuery{mDatabase};
	dropQuery.exec("DROP TABLE IF EXISTS temp_copy");
	DatabaseManager::debugQuery(dropQuery);

	std::array<std::string, 6> queries = removeHabitColumnQueries(
		tableSchema.toStdString(), habit.getIDName().toStdString());
	for (auto& queryStdStr : queries)
	{
		QSqlQuery query{mDatabase};
		query.exec(QString::fromStdString(queryStdStr));
		DatabaseManager::debugQuery(query);
	}
}

std::vector<Habit::Ptr> HabitDao::habits() const
{
	/** Get existing habits from TABLE_NAME_ID (for now no info from TABLE_NAME_DATA
	 * **/
	QSqlQuery query{"SELECT * FROM " + TABLE_NAME_ID + " ORDER BY id;", mDatabase};
	query.exec();
	DatabaseManager::debugQuery(query);
	std::vector<Habit::Ptr> habits;
	while (query.next())
	{
		Habit::Ptr habit{std::make_unique<Habit>()};

		habit->setId(toInt(query.value("id")));
		habit->setName(toQString(query.value("name")));
		habit->setStartDate(toQDate(query.value("startDate")));
		habit->setType(static_cast<Habit::Type>(toInt(query.value("habitType"))));
		habit->setUnits(toQString(query.value("habitUnits")));
		habit->setDailyGoal(toInt(query.value("dailyGoal")));
		habits.push_back(std::move(habit));
	}

	// Get additional data about habits from TABLE_NAME_DATA, specifically
	// data about dates statuses, between start of the habit and
	// Utility.hpp::currentDate()
	for (auto& habit : habits)
	{
		loadHabitInfoFromDataTable(habit.get());
	}

	return habits;
}

void HabitDao::loadHabitInfoFromDataTable(Habit* habit) const
{
	QDate	  startDate = habit->getStartDate();
	QString	  habitColumn = habit->getIDName();
	QSqlQuery query{mDatabase};

	//	query.exec("SELECT date," + habitColumn + " FROM " + TABLE_NAME_DATA +
	//			   " WHERE date>='" + startDate.toString(Qt::ISODate) +
	//			   "' AND date<=date('now', 'localtime')  ORDER BY id;");
	query.exec("SELECT date," + habitColumn + " FROM " + TABLE_NAME_DATA +
			   " WHERE date>='" + startDate.toString(Qt::ISODate) + "' AND date<='" +
			   currentDate().toString(Qt::ISODate) + "'  ORDER BY id;");
	DatabaseManager::debugQuery(query);
	QMap<QDate, int> habitStatusByDate;
	while (query.next())
	{
		QDate	 date = toQDate(query.value("date"));
		QVariant statusPure = query.value(habitColumn);
		int		 status = statusPure.isNull() ? INT_NULL_VALUE : toInt(statusPure);
		habitStatusByDate.insert(date, status);
	}
	habit->loadDatesInfo(std::move(habitStatusByDate));
}

std::array<QString, 4> HabitDao::getNullToFalseQueries(const Habit& habit) const
{
	// WARNING!
	// DO NOT FORGET TO ORDER BY date (or ROWID) before doing those queries
	QString		 habID = habit.getIDName();
	auto&		 r = habit.getRepeatInfo();
	const QDate& startDate = habit.getStartDate();

	// query_1 and query_2 will be assigned depends on r
	QString query_1;
	QString query_2;
	QString query_3 =
		QString{
			"update habits_data SET %1=(SELECT temp_table.%1 FROM temp_table WHERE "
			"temp_table.date = habits_data.date  ORDER BY id);"}
			.arg(habID);
	QString query_4{"DROP TABLE temp_table;"};

	if (r.dateUnit == Habit::TimeUnit::Day)
	{
		// Create temp copy table started with habit creation date
		// update each habit work date in original table
		// It's convinient to add another table
		query_1 =
			QString{
				"CREATE TEMPORARY TABLE temp_table AS SELECT * from habits_data "
				"WHERE "
				"date>='%1'"}
				.arg(startDate.toString(Qt::ISODate));
		query_2 = QString{"update temp_table SET %1=%2 WHERE (ROWID-1) % %3 = 0"}
					  .arg(habID)
					  .arg(INT_FALSE_VALUE)
					  .arg(r.frequence);
		/*************************************************************************/

		if (r.ends == "Never")
		{
			// Close queries, we're good
			query_1 += " ORDER BY id;";
			query_2 += ";";
		}
		else if (QDate::fromString(r.ends, Qt::ISODate).isValid())
		{
			// limit changes to specified date(r.ends)
			query_1 += QString{" AND date <='%1' ORDER BY id;"}.arg(r.ends);
			query_2 += ";";
		}
		else if (toInt(r.ends))
		{
			// limit to n specified changes(r.ends)
			query_1 += " ORDER BY id;";
			query_2 += QString{" LIMIT %2;"}.arg(r.ends);
		}
		else
		{
			qDebug() << r.ends;
			assert((false) && "Unable to determine r.ends type");
		}
	}
	if (r.dateUnit == Habit::TimeUnit::Week)
	{
		assert(r.pattern > 0);
		/*************************************************************************/
		query_1 =
			QString{
				"CREATE TEMPORARY TABLE temp_table AS SELECT * from habits_data "
				"WHERE "
				"date "
				">= '%1' AND ("}
				.arg(startDate.toString(Qt::ISODate));

		bool first = true;
		int	 startWeekDay = startDate.dayOfWeek() - 1;
		int	 tempTableWeekDayOffset = 0;
		for (int i = 0; i < 7; ++i)
		{
			if (r.pattern & (0b1 << i))
			{
				if (i > startWeekDay)
				{
					tempTableWeekDayOffset++;
				}
				if (!first)
				{
					query_1 += QString{" OR "};
				}
				query_1 += QString{"dayofweek="} + QString::number(i);
				first = false;
			}
		}
		query_1 += ")";
		int weekdays = std::bitset<32>(r.pattern).count();
		query_2 = QString{"update temp_table SET %1=%2 WHERE (ROWID-1+%3) % %4 < %5"}
					  .arg(habID)
					  .arg(INT_FALSE_VALUE)
					  .arg(tempTableWeekDayOffset)
					  .arg(weekdays * r.frequence)
					  .arg(weekdays);	 // 4 <= 1;"};
		/*************************************************************************/

		if (r.ends == "Never")
		{
			// Close queries, we're good
			query_1 += " ORDER BY id;";
			query_2 += ";";
		}
		else if (QDate::fromString(r.ends, Qt::ISODate).isValid())
		{
			// limit changes to specified date(r.ends)
			query_1 += QString{" AND date <='%1' ORDER BY id;"}.arg(r.ends);
			query_2 += ";";
		}
		else if (toInt(r.ends))
		{
			// limit to n specified changes(r.ends)
			query_1 += " ORDER BY id;";
			query_2 += QString{" LIMIT %2;"}.arg(r.ends);
		}
		else
		{
			assert((false) && "Unable to determine r.ends type");
		}
	}
	else if (r.dateUnit == Habit::TimeUnit::Month)
	{
		assert(r.pattern > 0);
		/*************************************************************************/
		query_1 =
			QString{
				"CREATE TEMPORARY TABLE temp_table AS SELECT * from habits_data "
				"WHERE "
				"date "
				">= '%1' AND ("}
				.arg(startDate.toString(Qt::ISODate));
		bool first = true;
		for (int i = 0; i < 31; ++i)
		{
			if (r.pattern & (0b1 << i))
			{
				if (!first)
				{
					query_1 += QString{" OR "};
				}
				query_1 += QString{"day="} + QString::number(i + 1);
				first = false;
			}
		}
		query_1 += ")";
		int days = std::bitset<32>(r.pattern).count();
		query_2 = QString{"update temp_table SET %1=2 WHERE (ROWID-1) % %3 < %4"}
					  .arg(habID)
					  .arg(INT_FALSE_VALUE)
					  .arg(days * r.frequence)
					  .arg(days);	 // 4 <= 1;"};
		/*************************************************************************/
		if (r.ends == "Never")
		{
			// Close queries, we're good
			query_1 += "  ORDER BY id;";
			query_2 += ";";
		}
		else if (QDate::fromString(r.ends, Qt::ISODate).isValid())
		{
			// limit changes to specified date(r.ends)
			query_1 += QString{" AND date <='%1' ORDER BY id;"}.arg(r.ends);
			query_2 += ";";
		}
		else if (toInt(r.ends))
		{
			// limit to n specified changes(r.ends)
			query_1 += " ORDER BY id;";
			query_2 += QString{" LIMIT %2;"}.arg(r.ends);
		}
		else
		{
			assert((false) && "Unable to determine r.ends type");
		}
	}
	else if (r.dateUnit == Habit::TimeUnit::NDays)
	{
		assert(r.pattern > 0);
		/*************************************************************************/
		query_1 =
			QString{
				"UPDATE habits_data SET %1=%2 WHERE "
				"date>='%3' AND ( (%4 & (1<<((rowid-1) % %5))) > 0)"}
				.arg(habID)
				.arg(INT_FALSE_VALUE)
				.arg(startDate.toString(Qt::ISODate))
				.arg(r.pattern)
				.arg(r.patternSize);
		query_2.clear();
		query_3.clear();
		query_4.clear();

		/*************************************************************************/
		if (r.ends == "Never")
		{
			// Close query, we're good
			query_1 += ";";
		}
		else if (QDate::fromString(r.ends, Qt::ISODate).isValid())
		{
			// limit changes to specified date(r.ends)
			query_1 += QString{" AND date <='%1';"}.arg(r.ends);
		}
		else if (toInt(r.ends))
		{
			// limit to n specified changes(r.ends)
			query_1 += QString{" LIMIT %2;"}.arg(r.ends);
		}
		else
		{
			assert((false) && "Unable to determine r.ends type");
		}
	}
	else if (r.dateUnit == Habit::TimeUnit::Year)
	{
		/*************************************************************************/
		query_1 =
			QString{"UPDATE habits_data SET %1=%2 WHERE month=%3 AND day=%4 AND year>=%5"}
				.arg(habID)
				.arg(INT_FALSE_VALUE)
				.arg(startDate.month())
				.arg(startDate.day())
				.arg(startDate.year());
		query_2.clear();
		query_3.clear();
		query_4.clear();
		/*************************************************************************/

		if (r.ends == "Never")
		{
			// Close query, we're good
			query_1 += ";";
		}
		else if (QDate::fromString(r.ends, Qt::ISODate).isValid())
		{
			// limit changes to specified date(r.ends)
			query_1 += QString{" AND date <='%1';"}.arg(r.ends);
		}
		else if (toInt(r.ends))
		{
			// limit to n specified changes(r.ends)
			query_1 += QString{" LIMIT %2;"}.arg(r.ends);
		}
		else
		{
			assert((false) && "Unable to determine r.ends type");
		}
	}
	qDebug() << "query_1|" << query_1 << "|";
	qDebug() << "query_2|" << query_2 << "|";
	qDebug() << "query_3|" << query_3 << "|";
	qDebug() << "query_4|" << query_4 << "|";

	return std::array<QString, 4>{query_1, query_2, query_3, query_4};
}

void HabitDao::_debug_printTable() const
{
	/** Get existing habits from TABLE_NAME_ID (for now no info from TABLE_NAME_DATA
	 * **/
	QSqlQuery query{"SELECT * FROM " + TABLE_NAME_ID + "  ORDER BY id;", mDatabase};
	query.exec();
	DatabaseManager::debugQuery(query);
	while (query.next())
	{
		Habit::Ptr habit{std::make_unique<Habit>()};

		habit->setId(toInt(query.value("id")));
		habit->setName(toQString(query.value("name")));
		habit->setStartDate(toQDate(query.value("startDate")));
		habit->setType(static_cast<Habit::Type>(toInt(query.value("habitType"))));
		habit->setUnits(toQString(query.value("habitUnits")));
		habit->setDailyGoal(toInt(query.value("dailyGoal")));

		qDebug() << "id:=" << habit->getId() << " name:=" << habit->getName()
				 << " startDate:=" << habit->getStartDate().toString(Qt::ISODate)
				 << " type:=" << static_cast<int>(habit->getType())
				 << " units:=" << habit->getUnits()
				 << " dailyGoal:=" << habit->getDailyGoal();
	}
}
