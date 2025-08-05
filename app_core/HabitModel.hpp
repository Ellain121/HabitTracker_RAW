#pragma once

#include "Habit.hpp"
#include "app_core_global.hpp"

#include <QAbstractTableModel>
#include <vector>

class DatabaseManager;

class APP_CORE_EXPORT HabitModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	enum Roles
	{
		IdRole = Qt::UserRole + 1,
		NameRole,
		StartDateRole,
		TypeRole,
		UnitsRole,
		DailyGoalRole,
		HabitStatusRole,
		HabitAllDatesStausRole,
		HabitFullDataRole
	};

public:
	explicit HabitModel(bool ignoreDB = false, QObject* parent = nullptr);

	QModelIndex addHabit(const Habit& habit);

	int		 rowCount(const QModelIndex& index = QModelIndex{}) const override;
	int		 columnCount(const QModelIndex& index = QModelIndex{}) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const override;

	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	bool removeRows(
		int row, int count, const QModelIndex& parent = QModelIndex{}) override;

	const Habit& getHabit(const QModelIndex& index) const;

	void updateCachedData();

private:
	bool isIndexValid(const QModelIndex& index) const;

private:
	DatabaseManager&		mDB;
	std::vector<Habit::Ptr> mHabits;
	bool					mIgnoreDBFlag;
};
