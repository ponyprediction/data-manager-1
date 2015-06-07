#pragma once

#include <QObject>

class Manager : public QObject
{
    Q_OBJECT
public:
    Manager();
    ~Manager();
    static void execute(const QString & command);

    static void processArgs(const QString & args,
                            bool & start,
                            bool & end,
                            bool & force);

    static void download(const QDate & dateStart,
                         const QDate & dateEnd,
                         const bool & start,
                         const bool & end,
                         const bool & force);

    static void parse(const QDate & dateStart,
                      const QDate & dateEnd,
                      const bool & start,
                      const bool & end,
                      const bool & force);

    static void insert(const QDate & dateStart,
                       const QDate & dateEnd,
                       const bool & start,
                       const bool & end,
                       const bool & force);
};
