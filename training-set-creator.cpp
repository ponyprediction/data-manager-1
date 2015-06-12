#include "training-set-creator.hpp"
#include "util.hpp"
#include "database-manager.hpp"
#include <QVector>
#include <QFileInfo>

TrainingSetCreator::TrainingSetCreator()
{
}

TrainingSetCreator::~TrainingSetCreator()
{
}

void TrainingSetCreator::createTrainingSet(const QDate & dateStart,
                                           const QDate & dateEnd,
                                           const QDate & dateStartHistory)
{
    // Init
    Util::write("Create training set from " + dateStart.toString("yyyy-MM-dd")
                + " to " + dateEnd.toString("yyyy-MM-dd")
                + " history " + dateStartHistory.toString("yyyy-MM-dd"));
    bool ok = true;
    QFile jsonFile;
    QJsonArray problems;
    QString jsonFilename;
    // Check filename
    if(ok)
    {
        jsonFilename = Util::getLineFromConf("trainingSetFilename", &ok);
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
            Util::writeError("no template for the training set filename");
        }
    }
    // Check filepath
    if(ok)
    {
        jsonFile.setFileName(jsonFilename);
        if (!jsonFile.open(QFile::WriteOnly))
        {
            ok = false;
            Util::writeError("cannot open file "
                             + QFileInfo(jsonFile).absoluteFilePath());
        }
    }
    // Get problems
    if(ok)
    {
        for(QDate date = dateStart ; date <= dateEnd
            ; date = date.addDays(1))
        {
            foreach (QString id,
                     DatabaseManager::getIdRaces(date))
            {
                problems << getProblem(id,
                                       dateStartHistory, date.addDays(-1));
            }
        }
    }
    // Write down Json
    if(ok)
    {
        QJsonDocument document;
        QJsonObject trainingSet;
        trainingSet["problems"] = problems;
        document.setObject(trainingSet);
        jsonFile.write(document.toJson());
    }
    jsonFile.close();
}


QJsonObject TrainingSetCreator::getProblem(const QString &raceId,
                                           const QDate &dateStartHistory,
                                           const QDate &dateEndHistory)
{
    Util::overwrite(raceId);
    // Init
    bool ok = true;
    QVector<int> wantedOutputs;
    QJsonObject json;
    QString inputs;
    //
    if(ok)
    {
        inputs = getInputs(raceId, dateStartHistory, dateEndHistory, ok);
    }
    //
    if(ok)
    {
        wantedOutputs = DatabaseManager::getArrival(raceId);
    }
    // Prepare object
    if(ok)
    {
        QString wantedOutputsStr;
        for(int i = 0 ; i < wantedOutputs.size() ; i++)
        {
            if(i)
            {
                wantedOutputsStr += " ; ";
            }
            wantedOutputsStr += QString::number(wantedOutputs[i]);
        }
        json["inputs"] = inputs;
        json["wantedOutputs"] = wantedOutputsStr;
        json["id"] = raceId;
    }
    return json;
}


QString TrainingSetCreator::getInputs(const QString & raceId,
                                      const QDate & dateStartHistory,
                                      const QDate & dateEndHistory,
                                      bool & ok)
{
    // Init
    QVector<Pony> ponies;
    QVector<Jockey> jockeys;
    QVector<Trainer> trainers;
    QVector<float> odds;
    int minTeamCount = 1;
    int maxTeamCount = 20;
    QVector<float> inputs;
    QString inputsStr;
    //
    if(ok)
    {
        // Ponies
        foreach (QString ponyName,
                 DatabaseManager::getListFromRaceOf("pony",raceId))
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
                 DatabaseManager::getListFromRaceOf("jockey",raceId))
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
                 DatabaseManager::getListFromRaceOf("trainer",raceId))
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
                 DatabaseManager::getListFromRaceOf("odds",raceId))
        {
            odds << odd.toFloat();
        }
        // Check
        if(!(ponies.size() == jockeys.size()
             && ponies.size() == trainers.size()
             && ponies.size() == odds.size()))
        {
            ok = false;
            Util::writeError("ponyCount, jockeyCount, trainerCount, oddsCount"
                             " don't match");
        }
    }
    //
    if(ok && !(ponies.size() >= minTeamCount))
    {
        ok = false;
        Util::writeError("not enough teams in " + raceId + " : "
                         + QString::number(ponies.size())
                         + " < " + QString::number(minTeamCount));
    }
    if(ok && !(ponies.size() <= maxTeamCount))
    {
        ok = false;
        Util::writeError("too much teams in " + raceId + " : "
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
        for(int i = 0 ; i < inputs.size() ; i++)
        {
            if(i)
            {
                inputsStr += " ; ";
            }
            inputsStr += QString::number(inputs[i], 'f');
        }
    }
    return inputsStr;
}
