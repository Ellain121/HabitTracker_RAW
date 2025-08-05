#pragma once

#include "Habit.hpp"

#include <array>
#include <vector>

class QSqlDatabase;

class HabitDao
{
public:
	HabitDao(QSqlDatabase& database);
	void init() const;

	void addHabit(Habit& habit) const;
	void updateHabitDateStatus(const QDate& date, const Habit& habit) const;
	void updateHabitBasicInfo(const Habit& habit) const;
	void removeHabit(const Habit& habit) const;

	std::vector<Habit::Ptr> habits() const;

private:
	inline void loadHabitInfoFromDataTable(Habit* habit) const;

	std::array<QString, 4> getNullToFalseQueries(const Habit& habit) const;

	void _debug_printTable() const;

private:
	QSqlDatabase& mDatabase;
};
