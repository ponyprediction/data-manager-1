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
                                  const int & history);

    static QJsonObject getProblem(const QString & raceId,
                                  const QDate &  dateStartHistory,
                                  const QDate &  dateEndHistory);

    static QString getInputs(const QString & raceId,
                             const QDate &  dateStartHistory,
                             const QDate &  dateEndHistory,
                             bool & ok);

};

