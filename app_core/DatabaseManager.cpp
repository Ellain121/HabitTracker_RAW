#include "DatabaseManager.hpp"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <cassert>

void DatabaseManager::debugQuery(const QSqlQuery& query)
{
    if (query.lastError().type() == QSqlError::NoError)
    {
        qDebug() << "Query OK: " << query.lastQuery()
                 << "| Affected rows: " << query.numRowsAffected();
    }
    else
    {
        qDebug() << "===============================";
        qWarning() << "Query KO:" << query.lastError().text();
        qWarning() << "Query text:" << query.lastQuery();
        qDebug() << "===============================";
    }
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager singletonDB{};
    return singletonDB;
}

DatabaseManager::DatabaseManager(const QString& path)
    : mDatabase{std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE"))}
    , habitDao{*mDatabase}
{
    mDatabase->setDatabaseName(path);
    bool openStatus = mDatabase->open();
    habitDao.init();

    assert(("mDatabase->open failed!") && openStatus);
}

DatabaseManager::~DatabaseManager()
{
    mDatabase->close();
}
