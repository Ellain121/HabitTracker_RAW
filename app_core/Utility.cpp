#include "Utility.hpp"

#include "Settings.hpp"

#include <QDateTime>
#include <QString>
#include <format>
#include <iostream>

using std::cout;
using std::endl;
using std::format;

QDate currentDate()
{
	QDateTime curDateTime{QDate::currentDate(), QTime::currentTime()};
	int hoursOffset = toInt(Settings::instance().getSetting("global", "new_day_offset"));

	curDateTime = curDateTime.addSecs(-hoursOffset * 60 * 60);
	return curDateTime.date();
}

int exec(const char* cmd)
{
	//	char  buffer[128];
	FILE* pipe = popen(cmd, "r");
	if (!pipe)
	{
		throw std::runtime_error("popen failed");
	}
	try
	{
		//		while (fgets(buffer, sizeof(buffer), pipe) != NULL)
		//		{
		//			;
		//		}
	}
	catch (...)
	{
		return WEXITSTATUS(pclose(pipe));
	}
	return WEXITSTATUS(pclose(pipe));
}

int toInt32(int64_t value)
{
	if (value > std::numeric_limits<int>::max() ||
		value < std::numeric_limits<int>::min())
	{
		logMessage("Failed conversion: int64_t to int(32) some data lost");
	}
	return static_cast<int>(value);
}

int toInt(const QVariant& var, const source_location& location)
{
	bool ok = false;
	int	 intValue = var.toInt(&ok);
	if (!ok)
	{
		logMessage("Failed conversion: QVariant to int", location);
	}
	return intValue;
}

bool toBool(const QVariant& var, const std::source_location& location)
{
	if (!var.canConvert<bool>())
	{
		logMessage("Failed conversion: QVariant to bool", location);
	}
	return var.toBool();
}

int64_t toInt64(const QVariant& var, const std::source_location& location)
{
	bool	  ok = false;
	qlonglong intLongValue = var.toLongLong(&ok);
	if (!ok)
	{
		logMessage("Failed conversion: QVariant to int", location);
	}
	return static_cast<int64_t>(intLongValue);
}

QString toQString(const QVariant& var, const std::source_location& location)
{
	if (!var.canConvert<QString>())
	{
		logMessage("Failed conversion: QVariant to QString", location);
	}
	return var.toString();
}

QDate toQDate(const QVariant& var, const std::source_location& location)
{
	if (!var.canConvert<QDate>())
	{
		logMessage("Failed conversion: QVariant to QDate", location);
	}
	return var.toDate();
}

QDate toQDate(const std::chrono::year_month_day& ymd)
{
	return QDate{toInt(ymd.year()), toInt(ymd.month()), toInt(ymd.day())};
}

int toInt(std::chrono::year year)
{
	return static_cast<int>(year);
}

int toInt(std::chrono::month month)
{
	uint m = static_cast<uint>(month);
	if (m > std::numeric_limits<int>::max())
	{
		logMessage("Failed conversion: std::chrono::month to int(32) some data lost");
	}
	return static_cast<int>(m);
}

int toInt(std::chrono::day day)
{
	uint d = static_cast<uint>(day);
	if (d > std::numeric_limits<int>::max())
	{
		logMessage("Failed conversion: std::chrono::month to int(32) some data lost");
	}
	return static_cast<int>(d);
}

void logMessage(string_view msg, const source_location& location)
{
	cout << format("{}({}): {}: {}", location.file_name(), location.line(),
				location.function_name(), msg)
		 << endl;
}
