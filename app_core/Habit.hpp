#pragma once

#include "Utility.hpp"
#include "app_core_global.hpp"

#include <QDate>
#include <QMap>
#include <QString>
#include <memory>

const QString endsNeverOption = "Never";
enum class RI_Settings
{
	None = 0,
	Daily = 1
};

class APP_CORE_EXPORT Habit
{
public:
	using Ptr = std::unique_ptr<Habit>;

	enum class Type
	{
		OneTime = 0,
		Quantitative
	};

	enum class TimeUnit
	{
		Day = 0,
		Week = 1,
		Month = 2,
		Year = 3,
		NDays = 4
	};

	struct RepeatInfo
	{
		RepeatInfo(RI_Settings settings = RI_Settings::None);

		int		 frequence;
		TimeUnit dateUnit;
		int		 pattern;
		int		 patternSize;
		QString	 ends;
	};

public:
	Habit(const QString& name = "", const QDate& date = currentDate(),
		Type type = Type::OneTime, const QString& units = "", int dailyGoal = 1,
		const Habit::RepeatInfo& repeatInfo = Habit::RepeatInfo{RI_Settings::Daily});

	void setId(int id);
	void setName(const QString& name);
	void setStartDate(const QDate& date);
	void setType(Habit::Type type);
	void setUnits(const QString& units);
	void setDailyGoal(int dailyGoal);
	void setDateStatus(const QDate& date, int status);
	void addDateStatus(const QDate& date, int status);
	void toggleDateStatus(const QDate& date);
	void loadDatesInfo(const QMap<QDate, int>& datesStatus);

	int						getId() const;
	const QString&			getName() const;
	const QDate&			getStartDate() const;
	Habit::Type				getType() const;
	const QString&			getUnits() const;
	int						getDailyGoal() const;
	int						getDateStatus(const QDate& date) const;
	const QMap<QDate, int>& getDatesStatus() const;
	const RepeatInfo		getRepeatInfo() const;
	int						getRepeatFrequence() const;
	Habit::TimeUnit			getRepeatDateUnit() const;
	int						getRepeatPattern() const;
	const QString&			getRepeatEnds() const;
	int						getRepeatPatternSize() const;

	QString getIDName() const;

	static QString	   typeToString(Habit::Type type);
	static Habit::Type stringToType(const QString& str);

private:
	int				 mId;
	QString			 mName;
	QDate			 mStartDate;
	Habit::Type		 mType;
	QString			 mUnits;
	int				 mDailyGoal;
	QMap<QDate, int> mDatesStatus;
	RepeatInfo		 mRepeatInfo;
};
