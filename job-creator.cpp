#include "job-creator.hpp"
#include "util.hpp"
#include "database-manager.hpp"

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
            // get list races
            foreach (QString completeIdRace, DatabaseManager::getCompleteIdRaces(date))
            {
                Util::addMessage("Processing race : " + completeIdRace);
                // get pony infos
                foreach (QString ponyName, DatabaseManager::getPoniesFromRace(completeIdRace))
                {
                    Util::addMessage("  Processing pony : " + ponyName);
                    int raceCount = DatabaseManager::getPonyRaceCount(ponyName, dateStartHistory, date);
                    int firstCount = DatabaseManager::getPonyFirstCount(ponyName, dateStartHistory, date);
                    // pony = {ponyname, count, count};
                }
                // get pony infos
                foreach (QString jockeyName, DatabaseManager::getJockeysFromRace(completeIdRace))
                {
                    int raceCount = DatabaseManager::getJockeyRaceCount(jockeyName, dateStartHistory, date);
                    int firstCount = DatabaseManager::getJockeyFirstCount(jockeyName, dateStartHistory, date);
                    // pony = {ponyname, count, count};
                }
                // get pony infos
                foreach (QString trainerName, DatabaseManager::getTrainersFromRace(completeIdRace))
                {
                    int raceCount = DatabaseManager::getTrainerRaceCount(trainerName, dateStartHistory, date);
                    int firstCount = DatabaseManager::getTrainerFirstCount(trainerName, dateStartHistory, date);
                    // pony = {ponyname, count, count};
                }
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
