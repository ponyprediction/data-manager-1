#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

class DatabaseManager
{
    public:
        DatabaseManager();
        ~DatabaseManager();
        static void insertRace(const QString & completeRaceId);
};

#endif // DATABASEMANAGER_HPP
