#pragma once

#include <QHash>
#include <QIdentityProxyModel>
#include <QPixmap>

class HabitModel;

class StatusProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    explicit StatusProxyModel(QObject* parent = nullptr);

    QVariant	data(const QModelIndex& index, int role) const override;
    void		setSourceModel(QAbstractItemModel* sourceModel) override;
    HabitModel* habitModel() const;

private:
    QHash<QString, QPixmap*> mInfo;
    QPixmap					 mTruePic;
    QPixmap					 mFalsePic;
    QPixmap					 mNullPic;
};
