#ifndef PARSER_HPP
#define PARSER_HPP

#include <QDate>
#include <QXmlStreamWriter>

class Parser
{
    public:
        Parser();
        ~Parser();
        static void parseDay(const QDate & date, const bool & force);
        static void parseReunion(const QString & date,
                          const QString & reunionId,
                          const QString & id,
                          const QString & name,
                          const bool & force);
        static void parseRace(const QString & date,
                       const QString & reunionId,
                       const QString & zeturfId,
                       const QString & name,
                       const QString & raceId,
                       const bool & force);
         static void parseArrival(const QString & date,
                          const QString & reunionId,
                          const QString & zeturfId,
                          const QString & name,
                          const QString & raceId,const bool & force);
};

#endif // PARSER_HPP
