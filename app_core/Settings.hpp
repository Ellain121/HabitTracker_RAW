#pragma once

#include <QHash>
#include <QSettings>
#include <QVariant>

enum class SettingVariantType
{
	Int = 0,
	Boolean,
};

struct SettingInfo
{
	SettingInfo(SettingVariantType type, QVariant defaultValue);

	SettingVariantType type;
	QVariant		   defaultValue;
};
/**
 * @brief The Settings class.
 * The purpose of the class to check if settingName(QString) is registered
 * and it's QVariant type in setter or getter same as when registered.
 * The problem of this class is that if you register settingName, it's QVariant
 * type must be the same even if it's in different settingGroup.
 * ALSO: settingGroup are dynamical, so there is no checks for it
 */
class Settings
{
public:
	static Settings& instance();

	void setSetting(
		const QString& settingGroup, const QString& settingName, const QVariant& var);
	QVariant getSetting(const QString& settingGroup, const QString& setting);

private:
	Settings();
	Settings& operator=(const Settings& rhs) = delete;

	void registerSettings();

	SettingInfo getSettingInfo(const QString& settingName) const;
	void		checkValidQVariant(
			   SettingVariantType settingVariantType, const QVariant& var) const;

private:
	QSettings					mQSettings;
	QHash<QString, SettingInfo> mRegisteredSettings;
};
