#pragma once

#include <QDate>
#include <QString>
#include <QVariant>
#include <chrono>
#include <source_location>

using std::source_location;
using std::string_view;

QDate currentDate();

int exec(const char* cmd);

int toInt32(int64_t value);

int toInt(
	const QVariant& var, const source_location& location = source_location::current());

bool toBool(
	const QVariant& var, const source_location& location = source_location::current());

int64_t toInt64(
	const QVariant& var, const source_location& location = source_location::current());

QString toQString(
	const QVariant& var, const source_location& location = source_location::current());

QDate toQDate(
	const QVariant& var, const source_location& location = source_location::current());

QDate toQDate(const std::chrono::year_month_day& ymd);
int	  toInt(std::chrono::year year);
int	  toInt(std::chrono::month month);
int	  toInt(std::chrono::day day);

void logMessage(
	string_view msg, const source_location& location = source_location::current());
