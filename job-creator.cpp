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
    // Init
    Util::write("Creating job from " + dateStart.toString("yyyy-MM-dd")
                     + " to " + dateEnd.toString("yyyy-MM-dd")
                     + " history " + dateStartHistory.toString("yyyy-MM-dd"));
    bool ok = true;
    QString error = "";
    //
    if(ok)
    {
        for(QDate date = dateStart ; date <= dateEnd
            ; date = date.addDays(1))
        {
            foreach (QString completeIdRace,
                     DatabaseManager::getCompleteIdRaces(date))
            {
                processRace(completeIdRace, dateStartHistory, date.addDays(-1));
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
    Util::write(completeIdRace);
    QVector<Pony> ponies;
    QVector<Jockey> jockeys;
    QVector<Trainer> trainers;
    // Ponies
    foreach (QString ponyName,
             DatabaseManager::getListFromRaceOf("pony",completeIdRace))
    {
        int raceCount = DatabaseManager::getRaceCountOf("pony",
                                                        ponyName,
                                                        dateStartHistory,
                                                        dateEndHistory);
        int firstCount = DatabaseManager::getFirstCountOf("pony",
                    ponyName, dateStartHistory, dateEndHistory);
        /*Pony temp;
        temp.name = ponyName;
        temp.firstCount = firstCount;
        temp.raceCount = raceCount;
        ponies.push_back(temp);*/

    }
    // Jockeys
    foreach (QString jockeyName,
             DatabaseManager::getListFromRaceOf("jockey",completeIdRace))
    {
        int raceCount = DatabaseManager::getRaceCountOf("jockey",
                                                        jockeyName,
                                                        dateStartHistory,
                                                        dateEndHistory);
        int firstCount = DatabaseManager::getFirstCountOf("jockey",
                    jockeyName, dateStartHistory, dateEndHistory);
        Jockey temp;
        temp.name = jockeyName;
        temp.firstCount = firstCount;
        temp.raceCount = raceCount;
        jockeys.push_back(temp);
    }
    // Trainers
    foreach (QString trainerName,
             DatabaseManager::getListFromRaceOf("trainer",completeIdRace))
    {
        int raceCount = DatabaseManager::getRaceCountOf("trainer",
                                                        trainerName,
                                                        dateStartHistory,
                                                        dateEndHistory);
        int firstCount = DatabaseManager::getFirstCountOf("trainer",
                    trainerName, dateStartHistory, dateEndHistory);
        Trainer temp;
        temp.name = trainerName;
        temp.firstCount = firstCount;
        temp.raceCount = raceCount;
        trainers.push_back(temp);
    }
    // Visualisation
    for(int i = 0 ; i < ponies.size() ; i++) {
        Util::write("    #" + QString::number(i+1));
        Util::write("        " + ponies[i].name + " : "
                         + QString::number(ponies[i].firstCount)
                         + " / " + QString::number(ponies[i].raceCount));
        Util::write("        " + jockeys[i].name + " : "
                         + QString::number(jockeys[i].firstCount)
                         + " / " + QString::number(jockeys[i].raceCount));
        Util::write("        " + trainers[i].name + " : "
                         + QString::number(trainers[i].firstCount)
                         + " / " + QString::number(trainers[i].raceCount));
    }
}
