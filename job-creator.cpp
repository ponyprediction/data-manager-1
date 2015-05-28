#include "job-creator.hpp"
#include "util.hpp"
#include "database-manager.hpp"

#include <QVector>

JobCreator::JobCreator()
{

}

JobCreator::~JobCreator()
{

}

void JobCreator::createJob(const QDate & dateStart,
                           const QDate & dateEnd,
                           const QDate & dateStartHistory)
{
    Util::addMessage("Creating job from " + dateStart.toString("yyyy-MM-dd")
                     + " to " + dateEnd.toString("yyyy-MM-dd"));
    // Init
    bool ok = true;
    QString error = "";
    //
    if(ok)
    {
        for(QDate date = dateStart ; date <= dateEnd ; date = date.addDays(1))
        {
            foreach (QString completeIdRace, DatabaseManager::getCompleteIdRaces(date))
            {
                processRace(completeIdRace, dateStartHistory, date);

            }
        }
    }
    // The end
    if(ok)
    {
        //Util::addMessage("Done");
    }
    if(!ok)
    {
        Util::addError(error);
    }
}

void JobCreator::processRace(const QString &completeIdRace,
                             const QDate &dateStartHistory,
                             const QDate &dateEndHistory)
{
    // Init
    Util::addMessage("Processing race : " + completeIdRace);
    QVector<Pony> ponies;
    QVector<Jockey> jockeys;
    QVector<Trainer> trainers;
    // Ponies
    foreach (QString ponyName,
             DatabaseManager::getPoniesFromRace(completeIdRace))
    {
        int raceCount = DatabaseManager::getPonyRaceCount(ponyName, dateStartHistory, dateEndHistory);
        int firstCount = DatabaseManager::getPonyFirstCount(ponyName, dateStartHistory, dateEndHistory);
        ponies.push_back(Pony{ponyName, raceCount, firstCount});
        Util::addMessage("  " + ponyName + " : "
                         + QString::number(firstCount)
                         + " / " + QString::number(raceCount));
    }
    // Jockeys
    foreach (QString jockeyName, DatabaseManager::getJockeysFromRace(completeIdRace))
    {
        int raceCount = DatabaseManager::getJockeyRaceCount(jockeyName, dateStartHistory, dateEndHistory);
        int firstCount = DatabaseManager::getJockeyFirstCount(jockeyName, dateStartHistory, dateEndHistory);
        jockeys.push_back(Jockey{jockeyName, raceCount, firstCount});
    }
    // Trainers
    foreach (QString trainerName, DatabaseManager::getTrainersFromRace(completeIdRace))
    {
        int raceCount = DatabaseManager::getTrainerRaceCount(trainerName, dateStartHistory, dateEndHistory);
        int firstCount = DatabaseManager::getTrainerFirstCount(trainerName, dateStartHistory, dateEndHistory);
        trainers.push_back(Trainer{trainerName, raceCount, firstCount});
    }
    //Util::addMessage("  ponies : " + QString::number(ponies.size()));
}
