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
    Util::addMessage("Adding from " + dateStart.toString("yyyy-MM-dd")
                     + " to " + dateEnd.toString("yyyy-MM-dd")
                     + " in database");
    mongo::client::initialize();
    DBClientConnection db;
    try
    {
        db.connect("localhost");
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (insertRace) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        for (QDate currentDate = dateStart ; currentDate <= dateEnd
             ; currentDate = currentDate.addDays(1))
        {
            QDir directory(Util::getLineFromConf("pathToJson")
                           + "/races/",currentDate.toString("yyyy-MM-dd")
                           + "*");
            QStringList raceFile = directory.entryList();
            for (int i = 0 ; i < raceFile.size() ; i++)
            {
                QFile currentRace(directory.absolutePath() + "/" + raceFile[i]);
                if (!currentRace.open(QIODevice::ReadOnly))
                    Util::addError("File not found : " + currentRace.fileName()
                                   + "(insertRace)");
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

void DatabaseManager::insertArrival(const QDate &dateStart, const QDate &dateEnd)
{
    mongo::client::initialize();
    DBClientConnection db;
    try
    {
        db.connect("localhost");
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (insertArrival) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        for (QDate currentDate = dateStart ; currentDate <= dateEnd
             ; currentDate = currentDate.addDays(1))
        {
            QDir directory(Util::getLineFromConf("pathToJson")
                           + "/arrivals/",currentDate.toString("yyyy-MM-dd")
                           + "*");
            QStringList arrivalFile = directory.entryList();
            for (int i = 0 ; i < arrivalFile.size() ; i++)
            {
                QFile currentArrival(directory.absolutePath() + "/" + arrivalFile[i]);
                if (!currentArrival.open(QIODevice::ReadOnly))
                    Util::addError("File not found : " + currentArrival.fileName()
                                   + "(insertArrival)");
                else
                {
                    BSONObj bson = fromjson(currentArrival.readAll());
                    db.insert("ponyprediction.race", bson);
                }
                currentArrival.close();
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
        BSONObj select = BSON("completeId" << 1);
        BSONObj where = BSON("date" << currentDate.toString("yyyyMMdd").toInt());
        std::auto_ptr<DBClientCursor> cursor
                = db.query("ponyprediction.race",where,0,0,&select);
        while (cursor->more())
        {
            retour.append(QString(cursor->next()
                                  .getField("completeId").valuestr()));
        }
    }
    return retour;
}

QStringList DatabaseManager::getPoniesFromRace(const QString &completeIdRace)
{
    QStringList retour = QStringList();
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
        std::vector<BSONElement> teams = cursor->next().getField("teams").Array();
        for (int i = 0 ; i< teams.size();i++)
        {
            retour.append(teams[i]["pony"].valuestr());
        }
    }
    qDebug() << retour;
    return retour;
}

int DatabaseManager::getPonyRaceCount(const QString &ponyName, const QDate &dateStart, const QDate &dateEnd)
{
    int retour = -1;
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
        BSONObj where = BSON("teams.pony"<< ponyName.toStdString() << "date"
                             << GTE << dateStart.toString("yyyyMMdd").toInt()
                             << LTE << dateEnd.toString("yyyyMMdd").toInt());
        retour = db.count("ponyprediction.race",where,0,0,0);
    }
    return retour;
}

int DatabaseManager::getPonyFirstCount(const QString &ponyName, const QDate &dateStart, const QDate &dateEnd)
{
    int retour = -1;
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
        BSONObj where = BSON("ranks.first"<< ponyName.toStdString() << "date"
                             << GTE << dateStart.toString("yyyyMMdd").toInt()
                             << LTE << dateEnd.toString("yyyyMMdd").toInt());
        retour = db.count("ponyprediction.race",where,0,0,0);
        qDebug() << retour;
    }
    return retour;
}

QStringList DatabaseManager::getJockeysFromRace(const QString &completeIdRace)
{
    QStringList retour = QStringList();
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
        BSONObj select = BSON("teams.jockey"<< 1);
        BSONObj where = BSON("completeId"<< completeIdRace.toStdString());
        std::auto_ptr<DBClientCursor> cursor = db.query("ponyprediction.race",where,0,0,&select);
        std::vector<BSONElement> teams = cursor->next().getField("teams").Array();
        for (int i = 0 ; i< teams.size();i++)
        {
            retour.append(teams[i]["jockey"].valuestr());
        }
    }
    qDebug() << retour;
    return retour;
}

int DatabaseManager::getJockeyRaceCount(const QString &jockeyName, const QDate &dateStart, const QDate &dateEnd)
{
    int retour = -1;
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
        BSONObj where = BSON("teams.jockey"<< jockeyName.toStdString() << "date"
                             << GTE << dateStart.toString("yyyyMMdd").toInt()
                             << LTE << dateEnd.toString("yyyyMMdd").toInt());
        retour = db.count("ponyprediction.race",where,0,0,0);
    }
    qDebug() << retour;
    return retour;
}

int DatabaseManager::getJockeyFirstCount(const QString &jockeyName, const QDate &dateStart, const QDate &dateEnd)
{
    return -1;
}

QStringList DatabaseManager::getTrainersFromRace(const QString &completeIdRace)
{
    QStringList retour = QStringList();
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
        BSONObj select = BSON("teams.trainer"<< 1);
        BSONObj where = BSON("completeId"<< completeIdRace.toStdString());
        std::auto_ptr<DBClientCursor> cursor = db.query("ponyprediction.race",where,0,0,&select);
        std::vector<BSONElement> teams = cursor->next().getField("teams").Array();
        for (int i = 0 ; i< teams.size();i++)
        {
            retour.append(teams[i]["trainer"].valuestr());
        }
    }
    qDebug() << retour;
    return retour;
}

int DatabaseManager::getTrainerRaceCount(const QString &trainerName, const QDate &dateStart, const QDate &dateEnd)
{
    int retour = -1;
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
        BSONObj where = BSON("teams.trainer"<< trainerName.toStdString() << "date"
                             << GTE << dateStart.toString("yyyyMMdd").toInt()
                             << LTE << dateEnd.toString("yyyyMMdd").toInt());
        retour = db.count("ponyprediction.race",where,0,0,0);
    }
    qDebug() << retour;
    return retour;
}

int DatabaseManager::getTrainerFirstCount(const QString &trainerName, const QDate &dateStart, const QDate &dateEnd)
{
    return -1;
}

