#ifndef PARSER_HPP
#define PARSER_HPP

#include <QDate>
#include <QXmlStreamWriter>

class Parser {
public:
    enum RacePart{ALL, START, END};
    Parser();
    ~Parser();
    static void parseDay(const RacePart & racePart,
                         const QDate & date,
                         const bool & force);
    static void parseReunion(const RacePart & racePart,
                             const QString & date,
                             const QString & reunionId,
                             const QString & id,
                             const QString & name,
                             const bool & force);
    static void parseStart(const QString & date,
                           const QString & reunionId,
                           const QString & zeturfId,
                           const QString & name,
                           const QString & raceId,
                           const bool & force);
    static void addEnd(const QString & date,
                       const QString & reunionId,
                       const QString & zeturfId,
                       const QString & name,
                       const QString & raceId,const bool & force);

};

#endif // PARSER_HPP
