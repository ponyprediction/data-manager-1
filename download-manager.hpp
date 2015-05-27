#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <QString>
#include <QDate>
#include <QXmlStreamWriter>
#include <QThread>

class DownloadManager
{
    public:
        DownloadManager();
        ~DownloadManager();

        void downloadDay(const QDate & date, const bool & force);
        void downloadReunion(const QString & date,
                             const QString & zeturfId,
                             const QString & name,
                             const QString & reunionId,
                             const bool & force);
        void downloadRaceStart(const QString & date,
                               const QString & reunionId,
                               const QString & raceId,
                               const QString & zeturfId,
                               const QString & name,
                               const bool & force);
        void downloadRaceOdds(const QString & date,
                               const QString & reunionId,
                               const QString & raceId,
                               const QString & zeturfId,
                               const QString & name,
                               const bool & force);
        void downloadRaceArrival(const QString & date,
                               const QString & reunionId,
                               const QString & raceId,
                               const QString & zeturfId,
                               const QString & name,
                               const bool & force);

    private:
        const QString getHtml(const QString & url);
        QString command;
};

#endif // MANAGER_HPP
