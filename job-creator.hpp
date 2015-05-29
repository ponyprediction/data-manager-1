#ifndef JOBCREATOR_HPP
#define JOBCREATOR_HPP

#include <QDate>

class JobCreator {
    struct Pony {
        QString name;
        int raceCount;
        int firstCount;
    };
    struct Jockey {
        QString name;
        int raceCount;
        int firstCount;
    };
    struct Trainer {
        QString name;
        int raceCount;
        int firstCount;
    };
  public:
    JobCreator();
    ~JobCreator();
    static void createJob(const QDate & dateStart,
                          const QDate & dateEnd,
                          const QDate & dateStartHistory);
    static void processRace(const QString & completeIdRace,
                            const QDate &  dateStartHistory,
                            const QDate &  dateEndHistory);
};

#endif // JOBCREATOR_HPP
