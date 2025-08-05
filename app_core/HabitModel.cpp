#include "HabitModel.hpp"

#include "DatabaseManager.hpp"
#include "Utility.hpp"

#include <QRandomGenerator>

namespace
{
const int COLUMN_SIZE = 7;

QDate getColumnDate(int column)
{
	return currentDate().addDays(1 - column);
}

}	 // namespace

HabitModel::HabitModel(bool ignoreDB, QObject* parent)
	: QAbstractTableModel{parent}
	, mDB{DatabaseManager::instance()}
	, mHabits{}
	, mIgnoreDBFlag{ignoreDB}
{
	if (!mIgnoreDBFlag)
	{
		mHabits = mDB.habitDao.habits();
	}
}

QModelIndex HabitModel::addHabit(const Habit& habit)
{
	int rowIndex = rowCount();

	beginInsertRows(QModelIndex{}, rowIndex, rowIndex);

	Habit::Ptr newHabit{std::make_unique<Habit>(habit)};
	if (!mIgnoreDBFlag)
	{
		mDB.habitDao.addHabit(*newHabit);
	}
	mHabits.push_back(std::move(newHabit));

	endInsertRows();

	return index(rowIndex, 0);
}

int HabitModel::rowCount(const QModelIndex&) const
{
	return mHabits.size();
}

int HabitModel::columnCount(const QModelIndex&) const
{
	return COLUMN_SIZE;
}

QVariant HabitModel::data(const QModelIndex& index, int role) const
{
	if (!isIndexValid(index))
	{
		return QVariant{};
	}

	const Habit& habit = *mHabits.at(index.row());
	switch (role)
	{
		case Qt::TextAlignmentRole:
		{
			if (index.column() > 0)
				return Qt::AlignCenter;
			else
				return QVariant{};
		}

		case Roles::IdRole:
		case Qt::UserRole:
			return habit.getId();

		case Roles::NameRole:
			return habit.getName();

		case Roles::StartDateRole:
			return habit.getStartDate();

		case Roles::TypeRole:
			return static_cast<int>(habit.getType());

		case Roles::UnitsRole:
			return habit.getUnits();

		case Roles::DailyGoalRole:
			return habit.getDailyGoal();

		case Qt::DisplayRole:
			//        case Qt::DecorationRole:
			{
				switch (index.column())
				{
					case 0:
						return habit.getName();
					default:
						return QVariant{};
				};
				break;
			}
			//            Qt::ItemDataRole::
		case Roles::HabitStatusRole:
		{
			if (index.column() > 0)
			{
				return habit.getDateStatus(getColumnDate(index.column()));
			}
			break;
		}

		case Roles::HabitAllDatesStausRole:
			return QVariant::fromValue(habit.getDatesStatus());

		case Roles::HabitFullDataRole:
			return QVariant::fromValue(habit);

		default:
			return QVariant{};
	}
	return QVariant{};
}

QVariant HabitModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section == 0)
	{
		return QVariant{};
	}

	QDate date = getColumnDate(section);

	return date.toString("ddd") + ": " + QString::number(date.day());
}

bool HabitModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!isIndexValid(index))
	{
		return false;
	}

	QDate  date = getColumnDate(index.column());
	Habit& habit = *mHabits.at(index.row());
	switch (role)
	{
		case Roles::HabitStatusRole:
		{
			if (habit.getType() == Habit::Type::OneTime)
			{
				habit.toggleDateStatus(date);
			}
			else
			{
				habit.setDateStatus(date, toInt(value));
			}
			if (!mIgnoreDBFlag)
			{
				mDB.habitDao.updateHabitDateStatus(date, habit);
			}
			break;
		}
		case Roles::NameRole:
		{
			habit.setName(toQString(value));
			if (!mIgnoreDBFlag)
			{
				mDB.habitDao.updateHabitBasicInfo(habit);
			}
			break;
		}
		case Roles::StartDateRole:
		{
			habit.setStartDate(toQDate(value));
			if (!mIgnoreDBFlag)
			{
				mDB.habitDao.updateHabitBasicInfo(habit);
			}
			break;
		}
		case Roles::TypeRole:
		{
			habit.setType(static_cast<Habit::Type>(toInt(value)));
			if (!mIgnoreDBFlag)
			{
				mDB.habitDao.updateHabitBasicInfo(habit);
			}
			break;
		}
		case Roles::UnitsRole:
		{
			habit.setUnits(toQString(value));
			if (!mIgnoreDBFlag)
			{
				mDB.habitDao.updateHabitBasicInfo(habit);
			}
			break;
		}
		case Roles::DailyGoalRole:
		{
			habit.setDailyGoal(toInt(value));
			if (!mIgnoreDBFlag)
			{
				mDB.habitDao.updateHabitBasicInfo(habit);
			}
			break;
		}
		default:
		{
			return false;
		}
	};

	emit dataChanged(index, index);
	return true;
}

bool HabitModel::removeRows(int row, int count, const QModelIndex& parent)
{
	if (row < 0 || row >= rowCount() || count <= 0 || (row + count) > rowCount())
	{
		return false;
	}

	beginRemoveRows(parent, row, row + count - 1);
	int countLeft = count;
	while (countLeft--)
	{
		const Habit& habit = *mHabits.at(row + countLeft);
		mDB.habitDao.removeHabit(habit);
	}
	mHabits.erase(mHabits.begin() + row, mHabits.begin() + row + count);
	endRemoveRows();

	return true;
}

const Habit& HabitModel::getHabit(const QModelIndex& index) const
{
	assert((isIndexValid(index)) && "Index invalid!");

	return *mHabits[index.row()];
}

void HabitModel::updateCachedData()
{
	mHabits.clear();
	mHabits = mDB.habitDao.habits();
}

bool HabitModel::isIndexValid(const QModelIndex& index) const
{
	return index.isValid() && index.row() < rowCount() && index.column() < columnCount();
}
