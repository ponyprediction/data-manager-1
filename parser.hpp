#pragma once

#include <QDate>
#include <QXmlStreamWriter>

class Parser {

public:

    enum RacePart{ALL, START, END, INPUTS};

    Parser();
    ~Parser();

    static void parseDay(const RacePart & racePart,
                         const QDate & date,
                         const bool & force);

    static void parseReunion(const RacePart & racePart,
                             const QString & date,
                             const QString & reunionId,
                             const QString & name,
                             const bool & force);

    static void parseStart(const QString & date,
                           const QString & reunionId,
                           const QString & zeturfId,
                           const QString & name,
                           const QString & raceId,
                           const bool & force);

    static void addInputs(const QString & date,
                          const QString & reunionId,
                          const QString & raceId,
                          const bool & force,
                          const int & dayCount,
                          const int & inputPerTeam,
                          const QString & type);

    static void addArrival(const QString & date,
                           const QString & reunionId,
                           const QString & zeturfId,
                           const QString & name,
                           const QString & raceId,
                           const bool & force);

};
