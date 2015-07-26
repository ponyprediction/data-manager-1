#pragma once

#include "mongo/client/dbclient.h"
#include "mongo/bson/bson.h"
#include <QDate>
#include <QJsonDocument>
#include <QStringList>
#include <QVector>

using namespace mongo;

class DatabaseManager
{

public:

    DatabaseManager();
    ~DatabaseManager();
    static void init();

    static void insertRace(const QDate & dateStart,
                           const QDate & dateEnd,
                           const bool force);

    static int getRaceCountOf(const QString &type ,
                              const QString &name,
                              const QDate &dateStart,
                              const QDate &dateEnd);

    static QStringList getListFromRaceOf(const QString &type
                                         ,const QString &id);

    static QStringList getIdRaces(const QDate &  date);

    static int getFirstCountOf(const QString &type,const QString &name,
                               const QDate &dateStart,
                               const QDate &dateEnd);

    static QVector<int> getArrival(const QString &id);
    static QString getWinnings(const QString &id);

    static bool favoriteShow(const QString & id);

    static bool insertPrediction(const QJsonDocument & prediction, const QString & id);

    static QString getInputs(QString id, QString type, bool & ok);

    static void test();



private:

    static bool initialized;
    static const std::string HOST;
    static const std::string RACES;

};
