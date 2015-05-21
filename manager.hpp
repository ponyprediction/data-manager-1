#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <QString>
#include <QDate>
#include <QXmlStreamWriter>

class Manager
{
    public:
        Manager();
        ~Manager();
        void processDay(const QDate & date, const bool & force);
        void processReunion(QXmlStreamWriter & xmlWriter,
                            const QString & url, const QString & zeturfId,
                            const QString & name, const QString & id);
        void processRace(QXmlStreamWriter & xmlWriter,
                         const QString & url, const QString & zeturfId,
                         const QString & name, const QString & id);
    private:
        const QString getHtml(const QString & url);
};

#endif // MANAGER_HPP
