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
        //Util::write("Init database manager...");
        mongo::client::initialize();
    }
}


void DatabaseManager::insertRace(const QDate & dateStart, const QDate & dateEnd, const bool force)
{
    // Init
    Util::write("Insert races from "
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
        Util::writeError("Connexion à la DB échoué (insertRace) : " +
                         QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        for (QDate currentDate = dateStart ; currentDate <= dateEnd
             ; currentDate = currentDate.addDays(1))
        {
            Util::overwrite("Inserting race " + currentDate.toString("yyyy-MM-dd"));
            QDir dir(Util::getLineFromConf("pathToRaces", 0),
                     currentDate.toString("yyyy-MM-dd") + "*");
            QStringList raceFile = dir.entryList();
            if(raceFile.size() != 0)
            {
                for (int i = 0 ; i < raceFile.size() ; i++)
                {
                    QFile currentRace(dir.absolutePath() + "/"
                                      + raceFile[i]);
                    if (!currentRace
                            .open(QIODevice::ReadOnly))
                    {
                        QString filename = Util::getFileName(currentRace);
                        Util::writeError("File not found : " + filename
                                         + " (insert race)");
                    }
                    else
                    {
                        if(currentRace.size() != 0)
                        {
                            BSONObj bson = fromjson(currentRace.readAll());
                            if(bson.isValid())
                            {
                                if(db.count("ponyprediction.races",bson) == 0)
                                {
                                    db.insert("ponyprediction.races", bson);
                                }
                                else
                                {
                                    QString filename = Util::getFileName(currentRace);
                                    Util::overwriteWarning("Already exist -> "
                                                           + filename
                                                           + " (insert races)");
                                }
                            }
                            else
                            {
                                QString filename = Util::getFileName(currentRace);
                                Util::writeError(filename
                                                 + "is not valid (insertData)");
                            }
                        }
                        else
                        {
                            QString filename = Util::getFileName(currentRace);
                            Util::writeError("Empty file -> " +
                                             filename + " (insert races)");
                        }
                        currentRace.close();
                    }
                }
            }
            else
            {
                Util::overwriteWarning("No data found for -> " +
                                       currentDate.toString("yyyy-MM-dd")
                                       + " (insert races)");
            }
        }
    }
    else
    {
        Util::writeError("Not connected to the DB");
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
        Util::writeError("Connexion à la DB échoué (DataBaseManager) : " +
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
                    = db.query("ponyprediction.races",projection,0,0,&query);
            while (cursor->more())
            {
                retour.append(QString(cursor->next()
                                      .getField("completeId").valuestr()));
            }
        }
        else
        {
            Util::writeError("Query or Projection are not valid (getCompleteIdRaces)");
        }
    }
    else
    {
        Util::writeError("Not connected to the DB");
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
        Util::writeError("Connexion à la DB échoué (DataBaseManager) : " +
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
                    .query("ponyprediction.races",projection,0,0,&query);
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
            Util::writeError(error + " (getListFromRaceOf)");
        }
    }
    else
    {
        Util::writeError("Not connected to the DB");
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
        Util::writeError("Connexion à la DB échoué (DataBaseManager) : " +
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
            retour = db.count("ponyprediction.races",projection,0,0,0);
        }
        else
        {
            Util::writeError("Projection is not valid (getFirstCountOf"+type+")");
        }
    }
    else
    {
        Util::writeError("Not connected to the DB");
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
        Util::writeError("Connexion à la DB échoué (DataBaseManager) : " +
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
            retour = db.count("ponyprediction.races",projection,0,0,0);
        }
        else
        {
            Util::writeError("Projection is not valid (getTrainerRaceCount)");
        }
    }
    else
    {
        Util::writeError("Not connected to the DB");
    }
    return retour;
}

/*QString DatabaseManager::getTrainerInRaceWhereTeamAndPonyAndJockey(
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
        Util::writeError("Connexion à la DB échoué (DataBaseManager) : " +
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
                    query("ponyprediction.start",query,0,0,&projection);
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
            Util::writeError(error +
                             " (getTrainerInRaceWhereTeamAndPonyAndJockey)");
        }
    }
    else
    {
        Util::writeError("Not connected to the DB");
    }
    return retour;
}*/

QVector<int> DatabaseManager::getArrival(const QString &completeIdRace)
{
    QVector<int> ids;
    QVector<int> ranks;
    QVector<int> orderedRank;
    init();
    DBClientConnection db;
    try
    {
        db.connect(HOST);
    }
    catch ( const mongo::DBException &e )
    {
        Util::writeError("Connexion à la DB échoué (DataBaseManager) : " +
                         QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        bool ok = true;
        QString error = QString();
        //Query query = BSON("teams.id" << 1);
        //BSONObj projection = BSON("completeId"<< completeIdRace.toStdString());
        BSONObj projection = BSON("teams.id" << 1 << "teams.rank" << 1);
        BSONObj query = BSON("completeId" << completeIdRace.toStdString());
        if(query.isValid() && projection.isValid())
        {
            std::auto_ptr<DBClientCursor> cursor = db
                    .query("ponyprediction.races",query,0,0,&projection);
            if(cursor->more())
            {
                BSONObj result = cursor->next();
                //qDebug() << QString::fromStdString(result.toString());
                if(result.hasField("teams"))
                {
                    std::vector<BSONElement> teams = result
                            .getField("teams").Array();

                    for (int i = 0 ; i< teams.size(); i++)
                    {
                        if(teams[i]["id"].ok() && teams[i]["rank"].ok())
                        {
                            ids.push_back((teams[i]["id"]._numberInt()));
                            ranks.push_back((teams[i]["rank"]._numberInt()));
                        }
                        else
                        {
                            ok = false;
                            error = "No field id/rank found";
                        }
                    }

                    for (int j = 1 ; j <= 7 ; j++)
                    {
                        for(int i = 0 ; i < ranks.size() ; i++)
                        {
                            if(ranks[i] == j)
                                orderedRank << ids[i];
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
            Util::writeError(error + " (getArrival)");
        }
    }
    else
    {
        Util::writeError("Not connected to the DB");
    }
    return orderedRank;
}

