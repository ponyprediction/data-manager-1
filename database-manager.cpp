#include "database-manager.hpp"
#include "mongo/bson/bson.h"
#include "util.hpp"
#include <QDir>
#include <QDebug>
#include <QFile>

bool DatabaseManager::initialized = false;
const std::string DatabaseManager::HOST = "localhost";

DatabaseManager::DatabaseManager()
{

}

DatabaseManager::~DatabaseManager()
{

}

void DatabaseManager::init()
{
    if(!initialized)
    {
        initialized = true;
        Util::addMessage("Init database manager...");
        mongo::client::initialize();
    }
}

void DatabaseManager::insertData(const QString & type,const QDate & dateStart
                                , const QDate & dateEnd)
{
    Util::addMessage("Adding from " + dateStart.toString("yyyy-MM-dd")
                     + " to " + dateEnd.toString("yyyy-MM-dd")
                     + " in database");
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
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
                           + "/"+ type +"s/",currentDate.toString("yyyy-MM-dd")
                           + "*");
            QStringList raceFile = directory.entryList();
            if(raceFile.size() != 0)
            {
                for (int i = 0 ; i < raceFile.size() ; i++)
                {
                    QFile currentRace(directory.absolutePath() + "/"
                                      + raceFile[i]);
                    if (!currentRace
                            .open(QIODevice::ReadOnly|QIODevice::Append))
                    {
                        QFileInfo fileInfo(currentRace.fileName());
                        QString filename(fileInfo.fileName());
                        Util::addError("File not found : " + filename
                                       + " (insertData"+type+")");
                    }
                    else
                    {
                        if(currentRace.pos() != 0)
                        {
                            BSONObj bson = fromjson(currentRace.readAll());
                            if(bson.isValid())
                            {
                                if(db.count("ponyprediction."+ type.toStdString(),bson) == 0)
                                    db.insert("ponyprediction"+ type.toStdString(), bson);
                                else
                                {
                                    QFileInfo fileInfo(currentRace.fileName());
                                    QString filename(fileInfo.fileName());
                                    Util::addWarning("Already exist -> "
                                                     + QString::fromStdString(bson.getField("completeId").valuestr())
                                                     +filename
                                                   + " (insertData "+type+")");
                                }
                            }
                            else
                            {
                                QFileInfo fileInfo(currentRace.fileName());
                                QString filename(fileInfo.fileName());
                                Util::addError(filename
                                               + "is not valid (insertData)");
                            }
                        }
                        else
                        {
                            QFileInfo fileInfo(currentRace.fileName());
                            QString filename(fileInfo.fileName());
                            Util::addError("Empty file -> " +
                                           filename + " (insertData "+type+")");
                        }
                    }
                    currentRace.close();
                }
            }
            else
            {
                Util::addWarning("No data found for -> " +
                                 currentDate.toString("yyyy-MM-dd")
                                 + " (insertData "+type+")");
            }
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
}

void DatabaseManager::insertRace(const QDate & dateStart
                                 , const QDate & dateEnd)
{
    Util::addMessage("Adding from " + dateStart.toString("yyyy-MM-dd")
                     + " to " + dateEnd.toString("yyyy-MM-dd")
                     + " in database");
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
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
            if(raceFile.size() != 0)
            {
                for (int i = 0 ; i < raceFile.size() ; i++)
                {
                    QFile currentRace(directory.absolutePath() + "/"
                                      + raceFile[i]);
                    if (!currentRace
                            .open(QIODevice::ReadOnly|QIODevice::Append))
                    {
                        QFileInfo fileInfo(currentRace.fileName());
                        QString filename(fileInfo.fileName());
                        Util::addError("File not found : " + filename
                                       + "(insertRace)");
                    }
                    else
                    {
                        if(currentRace.pos() != 0)
                        {
                            BSONObj bson = fromjson(currentRace.readAll());
                            if(bson.isValid())
                            {
                                if(db.count("ponyprediction.race",bson) == 0)
                                    db.insert("ponyprediction.race", bson);
                                else
                                {
                                    QFileInfo fileInfo(currentRace.fileName());
                                    QString filename(fileInfo.fileName());
                                    Util::addWarning("Already exist -> "
                                                     + QString::fromStdString(bson.getField("completeId").valuestr())
                                                     +filename
                                                     + " (insertRace)");
                                }
                            }
                            else
                            {
                                QFileInfo fileInfo(currentRace.fileName());
                                QString filename(fileInfo.fileName());
                                Util::addError(filename
                                               + "is not valid (insertRace)");
                            }
                        }
                        else
                        {
                            QFileInfo fileInfo(currentRace.fileName());
                            QString filename(fileInfo.fileName());
                            Util::addError("Empty file -> " +
                                           filename + "(insertRace)");
                        }
                    }
                    currentRace.close();
                }
            }
            else
            {
                Util::addWarning("No data found for -> " +
                                 currentDate.toString("yyyy-MM-dd") +
                                 "(insertRace)");
            }
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
}

void DatabaseManager::insertArrival(const QDate &dateStart,
                                    const QDate &dateEnd)
{
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
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
            if(arrivalFile.size() != 0)
            {
                for (int i = 0 ; i < arrivalFile.size() ; i++)
                {
                    QFile currentArrival(directory.absolutePath() + "/"
                                         + arrivalFile[i]);
                    if (!currentArrival
                            .open(QIODevice::ReadOnly|QIODevice::Append))
                    {
                        QFileInfo fileInfo(currentArrival.fileName());
                        QString filename(fileInfo.fileName());
                        Util::addError("File not found : " + filename
                                       + "(insertArrival)" );
                    }
                    else
                    {
                        if(currentArrival.pos() != 0)
                        {
                            BSONObj bson = fromjson(currentArrival.readAll());
                            if(bson.isValid())
                            {
                                if(db.count("ponyprediction.arrival",
                                            bson) == 0)
                                {
                                    db.insert("ponyprediction.arrival", bson);
                                }
                                else
                                {
                                    QFileInfo fileInfo(currentArrival
                                                       .fileName());
                                    QString filename(fileInfo.fileName());
                                    Util::addWarning("Already exist -> "
                                                     + QString::fromStdString(bson.getField("completeId").valuestr())
                                                     + filename
                                                     + " (insertArrival)");
                                }
                            }
                            else
                            {
                                QFileInfo fileInfo(currentArrival.fileName());
                                QString filename(fileInfo.fileName());
                                Util::addError(filename +
                                               "is not valid (insertArrival)");
                            }
                        }
                        else
                        {
                            QFileInfo fileInfo(currentArrival.fileName());
                            QString filename(fileInfo.fileName());
                            Util::addError("Empty file -> " + filename +
                                           " (inserArrival)");
                        }
                    }
                    currentArrival.close();
                }
            }
            else
            {
                Util::addWarning("No data found for -> " +
                                 currentDate.toString("yyyy-MM-dd") +
                                 "(insertArrival)");
            }
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
}

QStringList DatabaseManager::getCompleteIdRaces(const QDate &currentDate)
{
    QStringList retour;
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (DataBaseManager) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        BSONObj query = BSON("completeId" << 1);
        BSONObj projection = BSON("date" << currentDate.toString("yyyyMMdd")
                                  .toInt());
        if(query.isValid() && projection.isValid())
        {
            std::auto_ptr<DBClientCursor> cursor
                    = db.query("ponyprediction.race",projection,0,0,&query);
            while (cursor->more())
            {
                retour.append(QString(cursor->next()
                                      .getField("completeId").valuestr()));
            }
        }
        else
        {
            Util::addError("Query or Projection are not valid (getCompleteIdRaces)");
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
    return retour;
}

QStringList DatabaseManager::getPoniesFromRace(const QString &completeIdRace)
{
    QStringList retour = QStringList();
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (DataBaseManager) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        bool ok = true;
        QString error = QString();
        BSONObj query = BSON("teams.pony"<< 1);
        BSONObj projection = BSON("completeId"<< completeIdRace.toStdString());
        if(query.isValid() && projection.isValid())
        {
            std::auto_ptr<DBClientCursor> cursor = db
                    .query("ponyprediction.race",projection,0,0,&query);
            if(cursor->more())
            {
                BSONObj result = cursor->next();
                if(result.hasField("teams"))
                {
                    std::vector<BSONElement> teams = result
                            .getField("teams").Array();
                    for (int i = 0 ; i< teams.size(); i++)
                    {
                        if(teams[i]["pony"].ok())
                        {
                            retour.append(teams[i]["pony"].valuestr());
                        }
                        else
                        {
                            ok = false;
                            error = "No field trainer found";
                        }
                    }
                }
                else
                {
                    ok = false;
                    error = "No field teams found";
                }
            }
            else
            {
                ok = false;
                error = "No data in database";
            }
        }
        else
        {
            ok = false;
            error = "Query or Projection is not valid";
        }
        if(!ok)
        {
            Util::addError(error + " (getPoniesFromRace)");
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
    return retour;
}

int DatabaseManager::getPonyRaceCount(const QString &ponyName,
                                      const QDate &dateStart,
                                      const QDate &dateEnd)
{
    int retour = -1;
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (DataBaseManager) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        BSONObj projection = BSON("teams.pony"<< ponyName.toStdString() <<
                                  "date"<< GTE <<
                                  dateStart.toString("yyyyMMdd").toInt()
                                  << LTE <<
                                  dateEnd.toString("yyyyMMdd").toInt());
        if(projection.isValid())
        {
            retour = db.count("ponyprediction.race",projection,0,0,0);
        }
        else
        {
            Util::addError("Projection is not valid (getPonyRaceCount)");
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
    return retour;
}

int DatabaseManager::getPonyFirstCount(const QString &ponyName,
                                       const QDate &dateStart,
                                       const QDate &dateEnd)
{
    int retour = -1;
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (DataBaseManager) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        BSONObj projection = BSON("teams.poney"<< ponyName.toStdString() <<
                                  "date"<< GTE <<
                                  dateStart.toString("yyyyMMdd").toInt()
                                  << LTE << dateEnd.toString("yyyyMMdd").toInt()
                                  << "teams.rank" << 1);
        if(projection.isValid())
        {
            retour = db.count("ponyprediction.arrival",projection,0,0,0);
        }
        else
        {
            Util::addError("Projection is not valid (getPonyFirstCount)");
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
    return retour;
}

QStringList DatabaseManager::getJockeysFromRace(const QString &completeIdRace)
{
    QStringList retour = QStringList();
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (DataBaseManager) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        bool ok = true;
        QString error = QString();
        BSONObj query = BSON("teams.jockey"<< 1);
        BSONObj projection = BSON("completeId"<< completeIdRace.toStdString());
        if(query.isValid() && projection.isValid())
        {
            std::auto_ptr<DBClientCursor> cursor = db
                    .query("ponyprediction.race",projection,0,0,&query);
            if(cursor->more())
            {
                BSONObj result = cursor->next();
                if(result.hasField("teams"))
                {
                    std::vector<BSONElement> teams = result
                            .getField("teams").Array();
                    for (int i = 0 ; i< teams.size(); i++)
                    {
                        if(teams[i]["jockey"].ok())
                        {
                            retour.append(teams[i]["jockey"].valuestr());
                        }
                        else
                        {
                            ok = false;
                            error = "No field trainer found";
                        }
                    }
                }
                else
                {
                    ok = false;
                    error = "No field teams found";
                }
            }
            else
            {
                ok = false;
                error = "No data in database";
            }
        }
        else
        {
            ok = false;
            error = "Query or Projection is not valid";
        }
        if(!ok)
        {
            Util::addError(error + " (getJockeysFromRace)");
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
    return retour;
}

int DatabaseManager::getJockeyRaceCount(const QString &jockeyName,
                                        const QDate &dateStart,
                                        const QDate &dateEnd)
{
    int retour = -1;
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (DataBaseManager) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        BSONObj projection = BSON("teams.jockey"<< jockeyName.toStdString()
                                  << "date"<< GTE <<
                                  dateStart.toString("yyyyMMdd").toInt()<<
                                  LTE <<
                                  dateEnd.toString("yyyyMMdd").toInt());
        if(projection.isValid())
        {
            retour = db.count("ponyprediction.race",projection,0,0,0);
        }
        else
        {
            Util::addError("Projection is not valid (getJockeyRaceCount)");
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
    return retour;
}

int DatabaseManager::getJockeyFirstCount(const QString &jockeyName,
                                         const QDate &dateStart,
                                         const QDate &dateEnd)
{
    int retour = -1;
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (DataBaseManager) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        BSONObj projection = BSON("teams.jockey"<< jockeyName.toStdString()
                                  << "date"<< GTE <<
                                  dateStart.toString("yyyyMMdd").toInt()
                                  << LTE <<
                                  dateEnd.toString("yyyyMMdd").toInt()
                                  << "teams.rank" << 1);
        if(projection.isValid())
        {
            retour = db.count("ponyprediction.arrival",projection,0,0,0);
        }
        else
        {
            Util::addError("Projection is not valid (getJockeyFirstCount)");
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
    return retour;
}

QStringList DatabaseManager::getTrainersFromRace(
        const QString &completeIdRace)
{
    QStringList retour = QStringList();
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (DataBaseManager) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        bool ok = true;
        QString error = QString();
        BSONObj query = BSON("teams.trainer"<< 1);
        BSONObj projection = BSON("completeId"<< completeIdRace.toStdString());
        if(query.isValid() && projection.isValid())
        {
            std::auto_ptr<DBClientCursor> cursor = db
                    .query("ponyprediction.race",projection,0,0,&query);
            if(cursor->more())
            {
                BSONObj result = cursor->next();
                if(result.hasField("teams"))
                {
                    std::vector<BSONElement> teams = result
                            .getField("teams").Array();
                    for (int i = 0 ; i< teams.size(); i++)
                    {
                        if(teams[i]["trainer"].ok())
                        {
                            retour.append(teams[i]["trainer"].valuestr());
                        }
                        else
                        {
                            ok = false;
                            error = "No field trainer found";
                        }
                    }
                }
                else
                {
                    ok = false;
                    error = "No field teams found";
                }
            }
            else
            {
                ok = false;
                error = "No data in database";
            }
        }
        else
        {
            ok = false;
            error = "Query or Projection is not valid";
        }
        if(!ok)
        {
            Util::addError(error + " (getTrainersFromRace)");
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
    return retour;
}

int DatabaseManager::getTrainerRaceCount(const QString &trainerName,
                                         const QDate &dateStart,
                                         const QDate &dateEnd)
{
    int retour = -1;
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (DataBaseManager) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        BSONObj projection = BSON("teams.trainer"<< trainerName.toStdString()
                                  << "date"<< GTE <<
                                  dateStart.toString("yyyyMMdd").toInt()
                                  << LTE <<
                                  dateEnd.toString("yyyyMMdd").toInt());
        if(projection.isValid())
        {
            retour = db.count("ponyprediction.race",projection,0,0,0);
        }
        else
        {
            Util::addError("Projection is not valid (getTrainerRaceCount)");
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
    return retour;
}

int DatabaseManager::getTrainerFirstCount(const QString &trainerName,
                                          const QDate &dateStart,
                                          const QDate &dateEnd)
{
    int retour = -1;
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (DataBaseManager) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        BSONObj projection = BSON("teams.trainer"<< trainerName.toStdString()
                                  << "date"<< GTE <<
                                  dateStart.toString("yyyyMMdd").toInt()
                                  << LTE <<
                                  dateEnd.toString("yyyyMMdd").toInt()
                                  << "teams.rank" << 1);
        if(projection.isValid())
        {
            retour = db.count("ponyprediction.arrival",projection,0,0,0);
        }
        else
        {
            Util::addError("Projection is not valid (getTrainerFirstCount)");
        }

    }
    else
    {
        Util::addError("Not connected to the DB");
    }
    return retour;
}

QString DatabaseManager::getTrainerInRaceWhereTeamAndPonyAndJockey(
        const QString &completeraceId,
        const int &teamId,
        const QString &pony,
        const QString &jockey)
{
    QString retour = QString();
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
    }
    catch ( const mongo::DBException &e )
    {
        Util::addError("Connexion à la DB échoué (DataBaseManager) : " +
                       QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        bool ok = true;
        QString error = QString();
        BSONObj projection = fromjson("{teams : {$elemMatch:{pony:\"" +
                                      pony.toStdString() + "\"}}}");
        BSONObj query = BSON("completeId"<< completeraceId.toStdString());
        if(projection.isValid() && query.isValid())
        {
            std::auto_ptr<DBClientCursor> cursor = db.
                    query("ponyprediction.race",query,0,0,&projection);
            if(cursor->more())
            {
                BSONObj result = cursor->next();
                if(result.hasField("teams"))
                {
                    std::vector<BSONElement> teams = result
                            .getField("teams").Array();
                    //According to the documentation
                    if(teams[0]["trainer"].ok())
                    {
                        retour = QString::fromStdString(teams[0]["trainer"]
                                .valuestr());
                    }
                    else
                    {
                        ok = false;
                        error = "No field trainer found";
                    }

                }
                else
                {
                    ok = false;
                    error = "No field teams found";
                }
            }
            else
            {
                ok = false;
                error = "No data in database";
            }
        }
        else
        {
            ok = false;
            error = "Query or Projection invalid";
        }
        if(!ok)
        {
            Util::addError(error +
                           " (getTrainerInRaceWhereTeamAndPonyAndJockey)");
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
    return retour;
}

