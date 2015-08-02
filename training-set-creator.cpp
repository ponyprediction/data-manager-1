#include "training-set-creator.hpp"
#include "util.hpp"
#include "database-manager.hpp"
#include <QVector>
#include <QFileInfo>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

TrainingSetCreator::TrainingSetCreator()
{
}

TrainingSetCreator::~TrainingSetCreator()
{
}

void TrainingSetCreator::createTrainingSet(const QDate & dateStart,
                                           const QDate & dateEnd,
                                           const int & history,
                                           const int & type)
{
    switch(type)
    {
    case 0:
    {
        createTrainingSet0(dateStart, dateEnd, history);
        break;
    }
    case 1:
    {
        createTrainingSet1(dateStart, dateEnd, history);
        break;
    }
    default:
    {
        break;
    }
    }
}


void TrainingSetCreator::createTrainingSet0(const QDate & dateStart,
                                            const QDate & dateEnd,
                                            const int & history)
{
    // Init
    Util::write("Create training set #0 from " + dateStart.toString("yyyy-MM-dd")
                + " to " + dateEnd.toString("yyyy-MM-dd")
                + " history " + QString::number(history));
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
            jsonFilename.replace("HISTORY", QString::number(history));
            jsonFilename.replace("DATE_START",
                                 dateStart.toString("yyyy-MM-dd"));
            jsonFilename.replace("DATE_END", dateEnd.toString("yyyy-MM-dd"));
            jsonFilename.replace("TYPE", "0");
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
        for(QDate date = dateStart ; date <= dateEnd ; date = date.addDays(1))
        {
            foreach (QString id,
                     DatabaseManager::getIdRaces(date))
            {
                problems << getProblem(id);
            }
        }
    }
    // Write down Json
    if(ok)
    {
        QJsonDocument document;
        QJsonObject trainingSet;
        trainingSet["problems"] = problems;
        trainingSet["type"] = 0;
        document.setObject(trainingSet);
        jsonFile.write(document.toJson());
    }
    jsonFile.close();
}


QJsonObject TrainingSetCreator::getProblem(const QString &raceId)
{
    Util::overwrite(raceId);
    // Init
    bool ok = true;
    QVector<int> wantedOutputs;
    QJsonObject winnings;
    QJsonObject json;
    QString inputs;
    QString targetsStr;
    //
    if(ok)
    {
        inputs = DatabaseManager::getInputs(raceId, "default", ok);
    }
    //
    if(ok)
    {
        wantedOutputs = DatabaseManager::getArrival(raceId);
        for(int i = 0 ; i < 20 ; i++)
        {
            float target = 0;

            for(int j = 0 ; j < wantedOutputs.size() ; j++)
            {
                if(i+1 == wantedOutputs[j])
                {
                    target = 1-(0.1428*(j));
                }
            }
            if(i)
            {
                targetsStr += " ; ";
            }
            targetsStr += QString::number(target);
        }
    }
    if(ok)
    {
        QJsonDocument doc = QJsonDocument::fromJson(DatabaseManager::getWinnings(raceId).toUtf8());
        winnings = doc.object();
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
        json["winnings"] = winnings;
        json["inputs"] = inputs;
        json["wantedOutputs"] = wantedOutputsStr;
        json["targets"] = targetsStr;
        json["id"] = raceId;
    }
    return json;
}


void TrainingSetCreator::createTrainingSet1(const QDate & dateStart,
                                            const QDate & dateEnd,
                                            const int & history)
{
    // Init
    QDate dateStartHistory = dateStart.addDays(-history);
    Util::write("Create training set #1 from " + dateStart.toString("yyyy-MM-dd")
                + " to " + dateEnd.toString("yyyy-MM-dd")
                + " history " + QString::number(history));
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
            jsonFilename.replace("HISTORY", QString::number(history));
            jsonFilename.replace("DATE_START",
                                 dateStart.toString("yyyy-MM-dd"));
            jsonFilename.replace("DATE_END", dateEnd.toString("yyyy-MM-dd"));
            jsonFilename.replace("TYPE", "1");
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
                problems << getProblem1(id,
                                        dateStartHistory,
                                        date.addDays(-1));
            }
        }
    }
    // Write down Json
    if(ok)
    {
        QJsonDocument document;
        QJsonObject trainingSet;
        trainingSet["problems"] = problems;
        trainingSet["type"] = 1;
        document.setObject(trainingSet);
        jsonFile.write(document.toJson());
    }
    jsonFile.close();
}


QJsonObject TrainingSetCreator::getProblem1(const QString & raceId, const QDate & dateStartHistory, const QDate & dateEndHistory)
{
    Util::overwrite(raceId);
    bool ok = true;
    int wantedOutput;
    QJsonObject json;
    QString inputs;
    //
    if(ok)
    {
        inputs = getInputsByOdds(raceId, dateStartHistory, dateEndHistory, ok);
    }
    //
    if(ok)
    {
        if(DatabaseManager::favoriteShow(raceId))
        {
            wantedOutput = 1;
        }
        else
        {
            wantedOutput = 0;
        }
    }
    // Prepare object
    if(ok)
    {
        json["inputs"] = inputs;
        json["wantedOutputs"] = QString::number(wantedOutput);
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
    QVector<float> disqualifications;
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
        // DISQUALIFACATIONS
        foreach (QString disqualification,
                 DatabaseManager::getListFromRaceOf("disqualification",raceId))
        {
            //Util::writeError(disqualification);
            disqualifications << disqualification.toFloat();
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
            //Disqualification
            if(disqualifications.size() == ponies.size())
                inputs << 1.0 - (disqualifications[i] / 12);
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


QString TrainingSetCreator::getInputsByOdds(const QString & raceId,
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
    QVector<int> idsByOdds;
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
    // Order by odds
    if(ok)
    {
        QVector<float> tmpOdds = odds;
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
                idsByOdds.push_front(id);
            }
            else
            {
                idsByOdds.push_back(id);
            }
            tmpOdds[id] = -1;
        }
    }
    //
    if(ok)
    {
        for(int i = 0 ; i < idsByOdds.size() ; i++)
        {
            int id = idsByOdds[i];
            // Pony
            if(ponies[id].raceCount)
                inputs << ponies[id].firstCount / ponies[id].raceCount;
            else
                inputs << 0.0;
            // Joskey
            if(jockeys[id].raceCount)
                inputs << jockeys[id].firstCount / jockeys[id].raceCount;
            else
                inputs << 0.0;
            // Trainer
            if(trainers[id].raceCount)
                inputs << trainers[id].firstCount / trainers[id].raceCount;
            else
                inputs << 0.0;
            // Odds
            if(odds[id])
                inputs << 1.0 / (odds[id]);
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
            inputsStr += QString::number(inputs[i]);
        }
    }
    return inputsStr;
}
