#pragma once

#include <QDate>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class TrainingSetCreator
{
    struct Pony
    {
        QString name;
        float raceCount;
        float firstCount;
    };
    struct Jockey
    {
        QString name;
        float raceCount;
        float firstCount;
    };
    struct Trainer
    {
        QString name;
        float raceCount;
        float firstCount;
    };
public:
    TrainingSetCreator();
    ~TrainingSetCreator();
    static void createTrainingSet(const QDate & dateStart,
                          const QDate & dateEnd,
                          const QDate & dateStartHistory);
    static QJsonObject getProblem(const QString & completeIdRace,
                            const QDate &  dateStartHistory,
                            const QDate &  dateEndHistory);
};

