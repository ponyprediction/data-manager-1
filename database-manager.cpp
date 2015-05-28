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
