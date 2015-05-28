#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include <QDate>
#include "mongo/client/dbclient.h"
using namespace mongo;
class DatabaseManager
{
    public:
        DatabaseManager();
        ~DatabaseManager();
        static void insertRace(const QDate & dateStart, const QDate & dateEnd);
};

#endif // DATABASEMANAGER_HPP
