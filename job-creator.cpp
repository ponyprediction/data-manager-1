#include "job-creator.hpp"
#include "util.hpp"
#include "database-manager.hpp"
#include <QVector>
#include <QFileInfo>

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
    Util::write("Create job from " + dateStart.toString("yyyy-MM-dd")
                + " to " + dateEnd.toString("yyyy-MM-dd")
                + " history " + dateStartHistory.toString("yyyy-MM-dd"));
    bool ok = true;
    QFile jsonFile;
    QJsonArray problems;
    QString jsonFilename;
    // Check filename
    if(ok)
    {
        jsonFilename = Util::getLineFromConf("jobJsonFilename");
        if(jsonFilename.size())
        {
            jsonFilename.replace("DATE_START_HISTORY",
                                 dateStartHistory.toString("yyyy-MM-dd"));
            jsonFilename.replace("DATE_START",
                                 dateStart.toString("yyyy-MM-dd"));
            jsonFilename.replace("DATE_END", dateEnd.toString("yyyy-MM-dd"));
        }
        else
        {
            ok = false;
            Util::addError("no template for the job filename");
        }
    }
    // Check filepath
    if(ok)
    {
        jsonFile.setFileName(jsonFilename);
        if (!jsonFile.open(QFile::WriteOnly))
        {
            ok = false;
            Util::addError("cannot open file "
                           + QFileInfo(jsonFile).absoluteFilePath());
        }
    }
    // Get problems
    if(ok)
    {
        for(QDate date = dateStart ; date <= dateEnd
            ; date = date.addDays(1))
        {
            foreach (QString completeIdRace,
                     DatabaseManager::getCompleteIdRaces(date))
            {
                problems << getProblem(completeIdRace,
                                       dateStartHistory, date.addDays(-1));
            }
        }
    }
    // Write down Json
    if(ok)
    {
        QJsonDocument document;
        QJsonObject job;
        job["problems"] = problems;
        document.setObject(job);
        jsonFile.write(document.toJson());
    }
    jsonFile.close();
}
#include <QDebug>
QJsonObject JobCreator::getProblem(const QString &completeIdRace,
                                   const QDate &dateStartHistory,
                                   const QDate &dateEndHistory)
{
    // Init
    Util::overwrite(completeIdRace);
    QVector<Pony> ponies;
    QVector<Jockey> jockeys;
    QVector<Trainer> trainers;
    QVector<float> odds;
    bool ok = true;
    int minTeamCount = 1;
    int maxTeamCount = 20;
    QVector<float> inputs;
    QVector<int> wantedOutputs;
    QJsonObject json;
    //
    if(ok)
    {
        // Ponies
        foreach (QString ponyName,
                 DatabaseManager::getListFromRaceOf("pony",completeIdRace))
        {
            int raceCount = DatabaseManager::getRaceCountOf(
                        "pony", ponyName, dateStartHistory, dateEndHistory);
            int firstCount = DatabaseManager::getFirstCountOf(
                        "pony", ponyName, dateStartHistory, dateEndHistory);
            Pony temp;
            temp.name = ponyName;
            temp.firstCount = firstCount;
            temp.raceCount = raceCount;
            ponies.push_back(temp);

        }
        // Jockeys
        foreach (QString jockeyName,
                 DatabaseManager::getListFromRaceOf("jockey",completeIdRace))
        {
            int raceCount = DatabaseManager::getRaceCountOf(
                        "jockey",jockeyName,dateStartHistory,dateEndHistory);
            int firstCount = DatabaseManager::getFirstCountOf(
                        "jockey",jockeyName,dateStartHistory,dateEndHistory);
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
            int raceCount = DatabaseManager::getRaceCountOf(
                        "trainer",trainerName,dateStartHistory,dateEndHistory);
            int firstCount = DatabaseManager::getFirstCountOf(
                        "trainer",trainerName,dateStartHistory,dateEndHistory);
            Trainer temp;
            temp.name = trainerName;
            temp.firstCount = firstCount;
            temp.raceCount = raceCount;
            trainers.push_back(temp);
        }
        // Odds
        foreach (QString odd,
                 DatabaseManager::getListFromRaceOf("odds",completeIdRace))
        {
            odds << odd.toFloat();
        }
        // Check
        if(!(ponies.size() == jockeys.size()
             && ponies.size() == trainers.size()
             && ponies.size() == odds.size()))
        {
            ok = false;
            Util::addError("ponyCount, jockeyCount, trainerCount, oddsCount"
                           " don't match");
        }
    }
    //
    if(ok && !(ponies.size() >= minTeamCount))
    {
        ok = false;
        Util::addError("not enough teams in " + completeIdRace + " : "
                       + QString::number(ponies.size())
                       + " < " + QString::number(minTeamCount));
    }
    if(ok && !(ponies.size() <= maxTeamCount))
    {
        ok = false;
        Util::addError("too much teams in " + completeIdRace + " : "
                       + QString::number(ponies.size())
                       + " > " + QString::number(maxTeamCount));
    }
    //
    if(ok)
    {
        for(int i = 0 ; i < ponies.size() ; i++)
        {
            // Pony
            if(ponies[i].raceCount)
                inputs << ponies[i].firstCount / ponies[i].raceCount;
            else
                inputs << 0.0;
            // Joskey
            if(jockeys[i].raceCount)
                inputs << jockeys[i].firstCount / jockeys[i].raceCount;
            else
                inputs << 0.0;
            // Trainer
            if(trainers[i].raceCount)
                inputs << trainers[i].firstCount / trainers[i].raceCount;
            else
                inputs << 0.0;
            // Odds
            if(odds[i])
                inputs << 1.0 / (1.0+odds[i]);
            else
                inputs << 0.0;
        }
    }
    //
    if(ok)
    {
        wantedOutputs = DatabaseManager::getArrival(completeIdRace);
    }
    // Prepare object
    if(ok)
    {
        QString inputsStr;
        QString wantedOutputsStr;
        for(int i = 0 ; i < inputs.size() ; i++)
        {
            if(i)
            {
                inputsStr += " ; ";
            }
            inputsStr += QString::number(inputs[i], 'f');
        }
        for(int i = 0 ; i < wantedOutputs.size() ; i++)
        {
            if(i)
            {
                wantedOutputsStr += " ; ";
            }
            wantedOutputsStr += QString::number(wantedOutputs[i]);
        }
        json["inputs"] = inputsStr;
        json["wantedOutputs"] = wantedOutputsStr;
    }
    return json;
}
