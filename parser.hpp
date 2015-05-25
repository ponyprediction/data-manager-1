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
        void parseReunion(QXmlStreamWriter & xmlWriter,
                            const QString & id,
                            const QString & name,
                            const QString & number);
};

#endif // PARSER_HPP
