#ifndef PARSER_HPP
#define PARSER_HPP

#include <QDate>
#include <QXmlStreamWriter>

class Parser
{
    public:
        Parser();
        ~Parser();
        void parseDay(const QDate & date, const bool & force);
        void parseReunion(const QString & date,
                          const QString & id,
                          const QString & name,
                          const QString & number,
                          const bool & force);
        void parseRace(const QString & date,
                       const QString & reunionId,
                       const QString & zeturfId,
                       const QString & name,
                       const QString & raceId,
                       const bool & force);
        void parseArrival(const QString & date,
                          const QString & reunionId,
                          const QString & zeturfId,
                          const QString & name,
                          const QString & raceId,const bool & force);
};

#endif // PARSER_HPP
