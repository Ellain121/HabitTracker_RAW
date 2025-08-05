#include "Habit.hpp"

#include "Constants.hpp"

namespace
{
const QString oneTime = "One time";
const QString quantitative = "Quantitative";
}	 // namespace

Habit::RepeatInfo::RepeatInfo(RI_Settings settings)
	: frequence{0}
	, dateUnit{TimeUnit::Day}
	, pattern{0}
	, patternSize{0}
	, ends{""}
{
	if (settings == RI_Settings::Daily)
	{
		frequence = 1;
		dateUnit = TimeUnit::Day;
		pattern = 0;
		patternSize = 0;
		ends = endsNeverOption;
	}
}

QString Habit::typeToString(Habit::Type type)
{
	if (type == Habit::Type::OneTime)
	{
		return oneTime;
	}
	if (type == Habit::Type::Quantitative)
	{
		return quantitative;
	}
	assert(false);
}

Habit::Type Habit::stringToType(const QString& str)
{
	if (str == oneTime)
	{
		return Habit::Type::OneTime;
	}
	if (str == quantitative)
	{
		return Habit::Type::Quantitative;
	}
	assert(false);
}

Habit::Habit(const QString& name, const QDate& date, Type type, const QString& units,
	int dailyGoal, const RepeatInfo& repeatInfo)
	: mId{-1}
	, mName{name}
	, mStartDate{date}
	, mType{type}
	, mUnits{units}
	, mDailyGoal{dailyGoal}
	, mRepeatInfo{repeatInfo}
{
}

void Habit::setId(int id)
{
	mId = id;
}

void Habit::setName(const QString& name)
{
	mName = name;
}

void Habit::setStartDate(const QDate& date)
{
	mStartDate = date;
}

void Habit::setType(Type type)
{
	mType = type;
}

void Habit::setUnits(const QString& units)
{
	mUnits = units;
}

void Habit::setDailyGoal(int dailyGoal)
{
	mDailyGoal = dailyGoal;
}

void Habit::setDateStatus(const QDate& date, int status)
{
	QMap<QDate, int>::iterator found = mDatesStatus.find(date);
	assert((found != mDatesStatus.end()) && "Habit::setDateStatus failed!");

	*found = status;
}

void Habit::addDateStatus(const QDate& date, int status)
{
	assert((!mDatesStatus.contains(date)) &&
		   "Habit::addDateStatus failed: date already exist!");

	mDatesStatus[date] = status;
}

void Habit::toggleDateStatus(const QDate& date)
{
	// if not a one-type do nothing, toggle make no sense
	if (mType == Type::Quantitative)
		return;

	QMap<QDate, int>::iterator found = mDatesStatus.find(date);
	assert((found != mDatesStatus.end()) && "Habit::setDateStatus failed!");
	*found = 1 - *found;
}

void Habit::loadDatesInfo(const QMap<QDate, int>& datesStatus)
{
	mDatesStatus = std::move(datesStatus);
}

int Habit::getId() const
{
	return mId;
}

const QString& Habit::getName() const
{
	return mName;
}

const QDate& Habit::getStartDate() const
{
	return mStartDate;
}

Habit::Type Habit::getType() const
{
	return mType;
}

const QString& Habit::getUnits() const
{
	return mUnits;
}

int Habit::getDailyGoal() const
{
	return mDailyGoal;
}

int Habit::getDateStatus(const QDate& date) const
{
	auto found = mDatesStatus.find(date);
	if (found == mDatesStatus.end())
	{
		return INT_NULL_VALUE;
	}
	//    assert((found != mDatesStatus.end()) && "Habit::getDateStatus: date not
	//    found!");

	return found.value();
}

const QMap<QDate, int>& Habit::getDatesStatus() const
{
	return mDatesStatus;
}

const Habit::RepeatInfo Habit::getRepeatInfo() const
{
	return mRepeatInfo;
}

int Habit::getRepeatFrequence() const
{
	return mRepeatInfo.frequence;
}

Habit::TimeUnit Habit::getRepeatDateUnit() const
{
	return mRepeatInfo.dateUnit;
}

int Habit::getRepeatPattern() const
{
	return mRepeatInfo.pattern;
}

const QString& Habit::getRepeatEnds() const
{
	return mRepeatInfo.ends;
}

int Habit::getRepeatPatternSize() const
{
	return mRepeatInfo.patternSize;
}

QString Habit::getIDName() const
{
	return QString{"id_"} + QString::number(getId());
}
