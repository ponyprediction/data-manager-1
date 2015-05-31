#ifndef JOBCREATOR_HPP
#define JOBCREATOR_HPP

#include <QDate>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class JobCreator
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
    JobCreator();
    ~JobCreator();
    static void createJob(const QDate & dateStart,
                          const QDate & dateEnd,
                          const QDate & dateStartHistory);
    static QJsonObject getProblem(const QString & completeIdRace,
                            const QDate &  dateStartHistory,
                            const QDate &  dateEndHistory);
};

#endif // JOBCREATOR_HPP
