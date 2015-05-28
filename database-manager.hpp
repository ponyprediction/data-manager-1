#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include <QDate>

class DatabaseManager
{
    public:
        DatabaseManager();
        ~DatabaseManager();
        static insertRace(const QDate & dateStart, const QDate & dateEnd);
};

#endif // DATABASEMANAGER_HPP
