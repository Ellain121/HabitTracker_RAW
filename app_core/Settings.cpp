#include "Settings.hpp"

#include "config.h"

namespace
{
QString getSettingPath(const QString& group, const QString& name)
{
	return group + "/" + name;
}
}	 // namespace

SettingInfo::SettingInfo(SettingVariantType type, QVariant defaultValue)
	: type{type}
	, defaultValue{std::move(defaultValue)}
{
}

Settings& Settings::instance()
{
	static Settings singletonSettings{};
	return singletonSettings;
}

Settings::Settings()
	: mQSettings{ORGANIZATION_NAME, APPLICATION_NAME}
{
	registerSettings();
}

void Settings::registerSettings()
{
	mRegisteredSettings.insert("new_day_offset", {SettingVariantType::Boolean, false});

	mRegisteredSettings.insert(
		"enable_theo_graph_sg", {SettingVariantType::Boolean, false});
	mRegisteredSettings.insert(
		"theo_graph_daily_result_sg", {SettingVariantType::Int, 1});

	mRegisteredSettings.insert("x_tick_combo_box_indx_rrg", {SettingVariantType::Int, 0});
	mRegisteredSettings.insert("n_days_spin_box_rrg", {SettingVariantType::Int, 0});
}

SettingInfo Settings::getSettingInfo(const QString& settingName) const
{
	auto found = mRegisteredSettings.find(settingName);
	assert(found != mRegisteredSettings.end());

	return found.value();
}

void Settings::checkValidQVariant(
	SettingVariantType settingVariantType, const QVariant& var) const
{
	switch (settingVariantType)
	{
		using enum SettingVariantType;

		case Int:
		{
			assert(var.canConvert<int>());
		}
		break;

		case Boolean:
		{
			assert(var.canConvert<bool>());
		}
		break;
	}
}

void Settings::setSetting(
	const QString& settingGroup, const QString& settingName, const QVariant& var)
{
	SettingInfo settingInfo = getSettingInfo(settingName);

	checkValidQVariant(settingInfo.type, var);

	mQSettings.setValue(getSettingPath(settingGroup, settingName), var);
}

QVariant Settings::getSetting(const QString& settingGroup, const QString& settingName)
{
	SettingInfo settingInfo = getSettingInfo(settingName);
	QVariant	var = mQSettings.value(
		   getSettingPath(settingGroup, settingName), settingInfo.defaultValue);

	checkValidQVariant(settingInfo.type, var);

	return var;
}
