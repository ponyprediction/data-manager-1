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
    static void init();
    static void insertData(const QString & type,const QDate & dateStart
                           , const QDate & dateEnd);

    static int getRaceCountOf(const QString &type ,
                              const QString &name,
                              const QDate &dateStart,
                              const QDate &dateEnd);
    static QStringList getListFromRaceOf(const QString &type
                                         ,const QString &completeIdRace);

    static QStringList getCompleteIdRaces(const QDate &  date);

    static int getPonyFirstCount(const QString & ponyName,
                                 const QDate & dateStart,
                                 const QDate & dateEnd);

    static int getJockeyFirstCount(const QString & jockeyName,
                                   const QDate & dateStart,
                                   const QDate & dateEnd);

    static int getTrainerFirstCount(const QString & trainerName,
                                    const QDate & dateStart,
                                    const QDate & dateEnd);

    static QString getTrainerInRaceWhereTeamAndPonyAndJockey(
        const QString & completeraceId,
        const int & teamId,
        const QString & pony,
        const QString & jockey);

private:
    static bool initialized;
    static const std::string HOST;
};

#endif // DATABASEMANAGER_HPP
