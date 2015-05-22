#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <QString>
#include <QDate>
#include <QXmlStreamWriter>
#include <QThread>

class DownloadManager : public QThread
{
    public:
        DownloadManager();
        ~DownloadManager();
        void start(const QString & command);
        void run();

        void downloadDay(const QDate & date, const bool & force);
        void downloadReunion(const QString & id,
                             const QString & name,
                             const QString & number,
                             const bool & force);
        void downloadRaceStart(const QString & id, const QString & name,
                               const QString & number, const bool & force);
        void downloadRaceOdds(const QString & id, const QString & name,
                               const QString & number, const bool & force);
        void downloadRaceArrival(const QString & id, const QString & name,
                               const QString & number, const bool & force);



        void processDay(const QDate & date, const bool & force);
        void processReunion(QXmlStreamWriter & xmlWriter,
                            const QString & url, const QString & zeturfId,
                            const QString & name, const QString & id);
        void processRace(QXmlStreamWriter & xmlWriter,
                         const QString & url, const QString & zeturfId,
                         const QString & name, const QString & id);
    private:
        const QString getHtml(const QString & url);
    public:
        void setCommand(const QString & command);

    private:
        QString command;
};

#endif // MANAGER_HPP
