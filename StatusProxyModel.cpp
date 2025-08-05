#include "StatusProxyModel.hpp"

#include "Constants.hpp"
#include "HabitModel.hpp"
#include "Utility.hpp"

#include <QDebug>
#include <QImage>
#include <QPixmap>

namespace
{
const unsigned int STATUS_SIZE = 25;
}

StatusProxyModel::StatusProxyModel(QObject* parent)
	: QIdentityProxyModel{parent}
{
	QPixmap truePic = QPixmap{QPixmap::fromImage(QImage{":/correct.png"})};
	QPixmap falsePic = QPixmap{QPixmap::fromImage(QImage{":/x-mark.png"})};
	QPixmap nullPic = QPixmap{QPixmap::fromImage(QImage{":/minus.png"})};

	mTruePic = truePic.scaled(
		STATUS_SIZE, STATUS_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	mFalsePic = falsePic.scaled(
		STATUS_SIZE, STATUS_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	mNullPic = nullPic.scaled(
		STATUS_SIZE, STATUS_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QVariant StatusProxyModel::data(const QModelIndex& index, int role) const
{
	if (role != Qt::DecorationRole)
	{
		return QIdentityProxyModel::data(index, role);
	}

	QVariant disRole = sourceModel()->data(index, HabitModel::Roles::HabitStatusRole);
	if (disRole.userType() != QMetaType::Int && disRole.userType() != QMetaType::UInt)
	{
		return QIdentityProxyModel::data(index, role);
	}
	switch (toInt(disRole))
	{
		case INT_TRUE_VALUE:
			return QVariant{mTruePic};
		case INT_FALSE_VALUE:
			return QVariant{mFalsePic};
		case INT_NULL_VALUE:
			return QVariant{mNullPic};
		default:
			return QVariant{};
	}

	return toInt(disRole) > 0 ? QVariant{mTruePic} : QVariant{mFalsePic};
}

void StatusProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
	QIdentityProxyModel::setSourceModel(sourceModel);
}

HabitModel* StatusProxyModel::habitModel() const
{
	HabitModel* habitModel = dynamic_cast<HabitModel*>(sourceModel());
	assert((habitModel != nullptr) && "dynamic_cast to HabitModel* failed!");

	return habitModel;
}
