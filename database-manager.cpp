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
    // Init
    Util::addMessage("Insert " + type + " from "
                     + dateStart.toString("yyyy-MM-dd")
                     + " to "
                     + dateEnd.toString("yyyy-MM-dd"));
    init();
    DBClientConnection db;
    //
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
                        QString filename = Util::getFileName(currentRace);
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
                                    QString filename = Util::getFileName(currentRace);
                                    Util::addWarning("Already exist -> "
                                                     + QString::fromStdString(bson.getField("completeId").valuestr())
                                                     +filename
                                                     + " (insertData "+type+")");
                                }
                            }
                            else
                            {
                                QString filename = Util::getFileName(currentRace);
                                Util::addError(filename
                                               + "is not valid (insertData)");
                            }
                        }
                        else
                        {
                            QString filename = Util::getFileName(currentRace);
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

QStringList DatabaseManager::getListFromRaceOf(const QString &type,const QString &completeIdRace)
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
        BSONObj query = BSON("teams."+ type.toStdString()<< 1);
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
                        if(teams[i][type.toStdString()].ok())
                        {
                            retour.append(teams[i][type.toStdString()].valuestr());
                        }
                        else
                        {
                            ok = false;
                            error = "No field "+type+"trainer found";
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
            Util::addError(error + " (getListFromRaceOf)");
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
    return retour;
}

int DatabaseManager::getFirstCountOf(const QString &type,const QString &name,
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
        BSONObj projection = BSON("teams."+ type.toStdString()<< name.toStdString()
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
            Util::addError("Projection is not valid (getFirstCountOf"+type+")");
        }
    }
    else
    {
        Util::addError("Not connected to the DB");
    }
    return retour;
}

int DatabaseManager::getRaceCountOf(const QString &type ,
                                    const QString &name,
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
        BSONObj projection = BSON("teams."+ type.toStdString()
                                  << name.toStdString()
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

