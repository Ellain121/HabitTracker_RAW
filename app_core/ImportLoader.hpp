#pragma once

#include "Habit.hpp"

#include <QString>
#include <vector>

namespace
{
const QString DB_FILENAME = "backup.db";
}

std::vector<Habit> loadImportedHabits(const QString& path = DB_FILENAME);
