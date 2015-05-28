#include "database-manager.hpp"
#include "mongo/bson/bson.h"
#include "util.hpp"
#include <QDir>
#include <QDebug>
#include <QFile>

DatabaseManager::DatabaseManager()
{

}

DatabaseManager::~DatabaseManager()
{

}

void DatabaseManager::insertRace(const QDate & dateStart, const QDate & dateEnd)
{
    mongo::client::initialize();
    DBClientConnection db;
    try
    {
        db.connect("localhost");
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (DataBaseManager) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        for (QDate currentDate = dateStart ; currentDate <= dateEnd ; currentDate = currentDate.addDays(1))
        {
            QDir directory(Util::getLineFromConf("pathToJson") + "/races/",currentDate.toString("yyyy-MM-dd") + "*");
            QStringList raceFile = directory.entryList();
            for (int i = 0 ; i < raceFile.size() ; i++)
            {
                QFile currentRace(directory.absolutePath() + "/" + raceFile[i]);
                if (!currentRace.open(QIODevice::ReadOnly))
                    Util::addError("File not found : " + currentRace.fileName() + "(insertRace)");
                else
                {
                    BSONObj bson = fromjson(currentRace.readAll());
                    db.insert("ponyprediction.race", bson);
                }
                currentRace.close();
            }
        }
    }
}

QStringList DatabaseManager::getCompleteIdRaces(const QDate &currentDate)
{
    QStringList retour;
    mongo::client::initialize();
    DBClientConnection db;
    try
    {
        db.connect("localhost");
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (DataBaseManager) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        BSONObj select = BSON("completeId"<< 1);
        BSONObj where = BSON("date"<< 20140101);
        std::auto_ptr<DBClientCursor> cursor = db.query("ponyprediction.race",where,0,0,&select);
        while (cursor->more())
        {
            retour.append(QString(cursor->next().getField("completeId").valuestr()));
        }
    }
    return retour;
}

QStringList DatabaseManager::getPoniesFromRace(const QString &completeIdRace)
{
    QStringList retour;
    mongo::client::initialize();
    DBClientConnection db;
    try
    {
        db.connect("localhost");
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (DataBaseManager) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        BSONObj select = BSON("teams.pony"<< 1);
        BSONObj where = BSON("completeId"<< completeIdRace.toStdString());
        std::auto_ptr<DBClientCursor> cursor = db.query("ponyprediction.race",where,0,0,&select);
        while (cursor->more())
        {
            std::vector<BSONElement> teams = cursor->next().getField("teams").Array();
            for (int i = 0 ; i< teams.size();i++)
            {
                retour.append(teams[i]["pony"].valuestr());
            }
        }
    }
    return retour;
}

int DatabaseManager::getPonyRaceCount(const QString &ponyName, const QDate &dateStart, const QDate &dateEnd)
{
    return -1;
}

int DatabaseManager::getPonyFirstCount(const QString &ponyName, const QDate &dateStart, const QDate &dateEnd)
{
    return -1;
}

QStringList DatabaseManager::getJockeysFromRace(const QString &completeIdRace)
{
    return QStringList();
}

int DatabaseManager::getJockeyRaceCount(const QString &ponyName, const QDate &dateStart, const QDate &dateEnd)
{
    return -1;
}

int DatabaseManager::getJockeyFirstCount(const QString &ponyName, const QDate &dateStart, const QDate &dateEnd)
{
    return -1;
}

QStringList DatabaseManager::getTrainersFromRace(const QString &completeIdRace)
{
    return QStringList();
}

int DatabaseManager::getTrainerRaceCount(const QString &ponyName, const QDate &dateStart, const QDate &dateEnd)
{
    return -1;
}

int DatabaseManager::getTrainerFirstCount(const QString &ponyName, const QDate &dateStart, const QDate &dateEnd)
{
    return -1;
}

