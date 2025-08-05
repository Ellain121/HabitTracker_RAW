#pragma once

class QSqlDatabase;
class QSqlQuery;

#include "Constants.hpp"
#include "HabitDao.hpp"

#include <QString>
#include <memory>

class DatabaseManager
{
public:
	static DatabaseManager& instance();
	~DatabaseManager();

	static void debugQuery(const QSqlQuery& query);

private:
	DatabaseManager(const QString& path = DATABASE_FILENAME);
	DatabaseManager& operator=(const DatabaseManager& rhs) = delete;

private:
	std::unique_ptr<QSqlDatabase> mDatabase;

public:
	const HabitDao habitDao;
};
