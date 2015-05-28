#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include <QDate>
#include <QStringList>
#include "mongo/client/dbclient.h"

using namespace mongo;
class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();
    static void insertRace(const QDate & dateStart, const QDate & dateEnd);
    static void insertArrival(const QDate & dateStart, const QDate & dateEnd);

    static QStringList getCompleteIdRaces(const QDate &  date);

    static QStringList getPoniesFromRace(const QString & completeIdRace);
    static int getPonyRaceCount(const QString & ponyName,
                                const QDate & dateStart,
                                const QDate & dateEnd);
    static int getPonyFirstCount(const QString & ponyName,
                                 const QDate & dateStart,
                                 const QDate & dateEnd);

    static QStringList getJockeysFromRace(const QString & completeIdRace);
    static int getJockeyRaceCount(const QString & ponyName,
                                  const QDate & dateStart,
                                  const QDate & dateEnd);
    static int getJockeyFirstCount(const QString & ponyName,
                                   const QDate & dateStart,
                                   const QDate & dateEnd);

    static QStringList getTrainersFromRace(const QString & completeIdRace);
    static int getTrainerRaceCount(const QString & ponyName,
                                   const QDate & dateStart,
                                   const QDate & dateEnd);
    static int getTrainerFirstCount(const QString & ponyName,
                                    const QDate & dateStart,
                                    const QDate & dateEnd);
};

#endif // DATABASEMANAGER_HPP
