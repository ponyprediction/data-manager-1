#include "database-manager.hpp"
#include "mongo/client/dbclient.h"
#include "mongo/bson/bson.h"
#include "util.hpp"
#include <QDir>
#include <QDebug>
#include <QFile>


using namespace mongo;


bool DatabaseManager::initialized = false;
const std::string DatabaseManager::HOST = "localhost";
const std::string DatabaseManager::RACES = "ponyprediction.races";


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
                                if(db.count(RACES,bson) == 0)
                                {
                                    db.insert(RACES, bson);
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



QStringList DatabaseManager::getIdRaces(const QDate &currentDate)
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
        BSONObj query = BSON("id" << 1);
        BSONObj projection = BSON("date" << currentDate.toString("yyyyMMdd")
                                  .toInt());
        if(query.isValid() && projection.isValid())
        {
            std::auto_ptr<DBClientCursor> cursor
                    = db.query(RACES,projection,0,0,&query);
            while (cursor->more())
            {
                retour.append(QString(cursor->next()
                                      .getField("id").valuestr()));
            }
        }
        else
        {
            Util::writeError("Query or Projection are not valid (getIdRaces)");
        }
    }
    else
    {
        Util::writeError("Not connected to the DB");
    }
    return retour;
}


QStringList DatabaseManager::getListFromRaceOf(const QString &type,const QString &id)
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
        BSONObj projection = BSON("id"<< id.toStdString());
        if(query.isValid() && projection.isValid())
        {
            std::auto_ptr<DBClientCursor> cursor = db
                    .query(RACES,projection,0,0,&query);
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
                            if(type != "disqualification")
                            {
                                ok = false;
                                error = "No field "+type+" found for "
                                        + id;
                            }
                            else
                            {
                                retour.append("0");
                            }
                        }
                    }
                }
                else
                {
                    ok = false;
                    error = "No field teams found for "
                            + id;
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
            retour = db.count(RACES,projection,0,0,0);
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
            retour = db.count(RACES,projection,0,0,0);
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


QVector<int> DatabaseManager::getArrival(const QString &id)
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
        BSONObj projection = BSON("teams.id" << 1 << "teams.rank" << 1);
        BSONObj query = BSON("id" << id.toStdString());
        if(query.isValid() && projection.isValid())
        {
            std::auto_ptr<DBClientCursor> cursor = db
                    .query(RACES,query,0,0,&projection);
            if(cursor->more())
            {
                BSONObj result = cursor->next();
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
                            error = "No field id/rank found for "
                                    + id;
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
                    error = "No field teams found for " + id;
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


bool DatabaseManager::favoriteShow(const QString & id)
{
    init();
    bool ok = true;
    bool returnValue = false;
    DBClientConnection db;
    BSONObj projection = BSON("teams.id" << 1 << "teams.rank" << 1 << "teams.odds" << 1);
    BSONObj query = BSON("id" << id.toStdString());
    std::auto_ptr<DBClientCursor> cursor;
    BSONObj result;
    QVector<int> ranks;
    QVector<int> ids;
    QVector<float> odds;
    //
    if(ok && !(query.isValid() && projection.isValid()))
    {
        ok = false;
        Util::writeError("the query is invalid in firstShow()");
    }
    // Connect to database
    if(ok)
    {
        try
        {
            db.connect(HOST);
        }
        catch ( const mongo::DBException &e )
        {
            ok = false;
            Util::writeError("Connexion à la DB échoué (insertRace) : " +
                             QString::fromStdString(e.toString()));
        }
    }
    // Check connection
    if(ok && !db.isStillConnected())
    {
        ok = false;
        Util::writeError("DB is not connected anymore");
    }
    // Prepare cursor
    if(ok)
    {
        cursor = db.query(RACES,query,0,0,&projection);
        if(cursor->more())
        {
            result = cursor->next();
        }
        else
        {
            ok = false;
            Util::writeError("no data in database");
        }
    }
    //
    if(ok && !result.hasField("teams"))
    {
        ok = false;
        Util::writeError("No field teams found for " + id);
    }
    //
    if(ok)
    {
        std::vector<BSONElement> teams = result.getField("teams").Array();
        for (int i = 0 ; i < teams.size(); i++)
        {
            if(teams[i]["id"].ok() && teams[i]["rank"].ok() && teams[i]["odds"].ok())
            {
                ids << teams[i]["id"]._numberInt();
                ranks << teams[i]["rank"]._numberInt();
                odds << QString::fromStdString(teams[i]["odds"].String()).toFloat();
            }
            else
            {
                ok = false;
                Util::writeError("no field id/rank found for " + id);
            }
        }
    }
    //
    if(ok)
    {
        //
        QVector<int> idsByRank;
        QVector<int> idsByOdds;
        QVector<float> tmpOdds = odds;
        QVector<int> tmpRanks = ranks;
        //
        for(int i = 0 ; i < tmpOdds.size() ; i++)
        {
            int bestOdds = 0;
            int id = -1;
            for(int j = 0 ; j < tmpOdds.size() ; j++)
            {
                if(tmpOdds[j] >= bestOdds)
                {
                    bestOdds = tmpOdds[j];
                    id = j;
                }
            }
            if(tmpOdds[id])
            {
                idsByOdds.push_front(id+1);
            }
            else
            {
                idsByOdds.push_back(id+1);
            }
            tmpOdds[id] = -1;
        }
        //
        for(int i = 0 ; i < tmpRanks.size() ; i++)
        {
            int bestRank = 0;
            int id = -1;
            for(int j = 0 ; j < tmpRanks.size() ; j++)
            {
                if(tmpRanks[j] >= bestRank)
                {
                    bestRank = tmpRanks[j];
                    id = j;
                }
            }
            if(tmpRanks[id])
            {
                idsByRank.push_front(id+1);
            }
            else
            {
                idsByRank.push_back(id+1);
            }
            tmpRanks[id] = -1;
        }
        //

        if(ranks.size() >= 8)
        {
            if(idsByOdds[0] == idsByRank[0]
                    || idsByOdds[0] == idsByRank[1]
                    || idsByOdds[0] == idsByRank[2])
            {
                returnValue = true;
            }
        }
        else if (ranks.size() <= 7)
        {
            if(idsByOdds[0] == idsByRank[0]
                    || idsByOdds[0] == idsByRank[1])
            {
                returnValue = true;
            }
        }
    }
    return returnValue;
}


bool DatabaseManager::insertPrediction(const QJsonDocument & prediction,
                                       const QString & id)
{
    Util::overwrite("Inserting prediction " + id);
    bool ok = true;
    init();
    DBClientConnection db;
    BSONObj bson = fromjson(prediction.toJson());
    std::string collection = "ponyprediction.predictions";
    // Check BSON
    if(ok && !bson.isValid())
    {
        ok = false;
        Util::writeError("Unvalid bson for inserting prediction " + id);
    }
    // Connect to database
    if(ok)
    {
        try
        {
            db.connect(HOST);
        }
        catch ( const mongo::DBException &e )
        {
            ok = false;
            Util::writeError("Connexion à la DB échoué (insertRace) : " +
                             QString::fromStdString(e.toString()));
        }
    }
    // Check connection
    if(ok && !db.isStillConnected())
    {
        ok = false;
        Util::writeError("DB is not connected anymore");
    }
    // Check
    if(ok && db.count(collection, bson) != 0)
    {
        ok = false;
        Util::writeError("Prediction already exists : " + id);
    }
    // Insert
    if(ok)
    {
        db.insert(collection, bson);
    }
    return ok;
}
