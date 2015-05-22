#include "download-manager.hpp"
#include "util.hpp"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QStringList>
#include <QVector>
#include <QFile>
#include <QFileInfo>

DownloadManager::DownloadManager() : QThread()
{

}

DownloadManager::~DownloadManager()
{

}

/******************************************************************************/

void DownloadManager::start(const QString & command)
{
    setCommand(command);
    QThread::start();
}
void DownloadManager::run()
{
    QDate date = QDate::fromString(command, "yyyy-MM-dd");
    downloadDay(date, true);
}

void DownloadManager::downloadDay(const QDate & date, const bool & force)
{
    // Init
    bool ok = true;
    QString error = "";
    QString dayUrl = "";
    QString html = "";
    QString filename = Util::getLineFromConf("dayHtmlFilename");
    filename.replace("DATE", date.toString("yyyy-MM-dd"));
    QFile file;
    // Check date
    if(ok
       && (date >= QDate::currentDate()))
    {
        ok = false;
        error = "invalid date : " + date.toString("yyyy-MM-dd") + " >= today";
    }
    // Check file
    if(ok && !force && QFile::exists(filename))
    {
        ok = false;
        error = "the file already exists "
                + QFileInfo(filename).absoluteFilePath();
    }
    // Open file
    if(ok)
    {
        file.setFileName(filename);
        if (!file.open(QFile::WriteOnly))
        {
            ok = false;
            error = "cannot open file " + QFileInfo(file).absoluteFilePath();
        }
    }
    // Prepare url
    if(ok)
    {
        dayUrl = Util::getLineFromConf("dayUrl");
        dayUrl.replace("DATE", date.toString("yyyy-MM-dd"));
    }
    // Download html & save
    if(ok)
    {
        html = getHtml(dayUrl);
        file.write(html.toUtf8());
        file.close();
    }
    // Parsing
    if(ok)
    {
        QVector<QString> reunions;
        QRegExp rx("href=\"([^\"]*id=([0-9]*)[^\"]*)\" "
                   "title=\"([^\"]*)\" "
                   "class=\"halfpill\">(R[0-9]+)<");
        int pos = 0;
        while ((pos = rx.indexIn(html, pos)) != -1)
        {
            pos += rx.matchedLength();
            QString url = rx.cap(1);
            QString reunionId = rx.cap(2);;
            QString name = rx.cap(3);
            QString number = rx.cap(4);
            bool addReunion = true;
            for(int i = 0 ; i < reunions.size() ; i++)
            {
                if(reunionId == reunions[i])
                {
                    addReunion = false;
                }
            }
            if(addReunion)
            {
                downloadReunion(reunionId, name, number, force);
                reunions.push_back(reunionId);
            }
        }
    }
    // End
    if(ok)
    {
    }
    if(!ok)
    {
        Util::addError(error);
    }
}

void DownloadManager::downloadReunion(const QString & id,
                                      const QString & name,
                                      const QString & number,
                                      const bool & force)
{
    // Start
    bool ok = true;
    QString error = "";
    QString url = Util::getLineFromConf("reunionUrl");
    url.replace("ID", id);
    QString filename = Util::getLineFromConf("reunionHtmlFilename");
    filename.replace("ID", id);
    QFile file;
    QString html = "";
    // Check file
    if(ok && !force && QFile::exists(filename))
    {
        ok = false;
        error = "the file already exists "
                + QFileInfo(filename).absoluteFilePath();
    }
    // Open file
    if(ok)
    {
        file.setFileName(filename);
        if (!file.open(QFile::WriteOnly))
        {
            ok = false;
            error = "cannot open file " + QFileInfo(file).absoluteFilePath();
        }
    }
    // Download html & save
    if(ok)
    {
        html = getHtml(url);
        file.write(html.toUtf8());
        file.close();
    }
    // Parse
    if(ok)
    {
        QVector<QString> races;
        QRegExp rx("href=\"([^\"]*id=([0-9]*))\" "
                   "title=\""+name+" - ([^\"]*)\" "
                   +"class=\"pill\">&nbsp;"+number+" (C[0-9]+)");
        int pos = 0;
        while ((pos = rx.indexIn(html, pos)) != -1)
        {
            pos += rx.matchedLength();
            QString url = rx.cap(1);
            QString id = rx.cap(2);
            QString name = rx.cap(3);
            QString number = rx.cap(4);
            bool addRace = true;
            for(int i = 0 ; i < races.size() ; i++)
            {
                if(name == races[i])
                {
                    addRace = false;
                }
            }
            if(addRace)
            {
                downloadRaceStart(id, name, number, force);
                downloadRaceOdds(id, name, number, force);
                downloadRaceArrival(id, name, number, force);
                races.push_back(name);
            }
        }
    }
    // End
    if(ok)
    {
    }
    if(!ok)
    {
        Util::addError(error);
    }
}

void DownloadManager::downloadRaceStart(const QString & id,
                                        const QString & name,
                                        const QString & number,
                                        const bool & force)
{
    // Start
    (void)name;
    (void)number;
    bool ok = true;
    QString error = "";
    QString url = Util::getLineFromConf("raceStartUrl");
    url.replace("ID", id);
    QString filename = Util::getLineFromConf("raceStartHtmlFilename");
    filename.replace("ID", id);
    QFile file;
    QString html = "";
    // Check file
    if(ok && !force && QFile::exists(filename))
    {
        ok = false;
        error = "the file already exists "
                + QFileInfo(filename).fileName();
    }
    // Open file
    if(ok)
    {
        file.setFileName(filename);
        if (!file.open(QFile::WriteOnly))
        {
            ok = false;
            error = "cannot open file " + QFileInfo(file).absoluteFilePath();
        }
    }
    // Download html & save
    if(ok)
    {
        html = getHtml(url);
        file.write(html.toUtf8());
        file.close();
    }
    // End
    if(ok)
    {
    }
    if(!ok)
    {
        Util::addError(error);
    }
}

void DownloadManager::downloadRaceOdds(const QString & id,
                                       const QString & name,
                                       const QString & number,
                                       const bool & force)
{
    // Start
    (void)name;
    (void)number;
    bool ok = true;
    QString error = "";
    QString url = Util::getLineFromConf("raceOddsUrl");
    url.replace("ID", id);
    QString filename = Util::getLineFromConf("raceOddsHtmlFilename");
    filename.replace("ID", id);
    QFile file;
    QString html = "";
    // Check file
    if(ok && !force && QFile::exists(filename))
    {
        ok = false;
        error = "the file already exists "
                + QFileInfo(filename).fileName();
    }
    // Open file
    if(ok)
    {
        file.setFileName(filename);
        if (!file.open(QFile::WriteOnly))
        {
            ok = false;
            error = "cannot open file " + QFileInfo(file).absoluteFilePath();
        }
    }
    // Download html & save
    if(ok)
    {
        html = getHtml(url);
        file.write(html.toUtf8());
        file.close();
    }
    // End
    if(ok)
    {
    }
    if(!ok)
    {
        Util::addError(error);
    }
}

void DownloadManager::downloadRaceArrival(const QString & id,
                                          const QString & name,
                                          const QString & number,
                                          const bool & force)
{
    // Start
    (void)name;
    (void)number;
    bool ok = true;
    QString error = "";
    QString url = Util::getLineFromConf("raceArrivalUrl");
    url.replace("ID", id);
    QString filename = Util::getLineFromConf("raceArrivalHtmlFilename");
    filename.replace("ID", id);
    QFile file;
    QString html = "";
    // Check file
    if(ok && !force && QFile::exists(filename))
    {
        ok = false;
        error = "the file already exists "
                + QFileInfo(filename).fileName();
    }
    // Open file
    if(ok)
    {
        file.setFileName(filename);
        if (!file.open(QFile::WriteOnly))
        {
            ok = false;
            error = "cannot open file " + QFileInfo(file).absoluteFilePath();
        }
    }
    // Download html & save
    if(ok)
    {
        html = getHtml(url);
        file.write(html.toUtf8());
        file.close();
    }
    // End
    if(ok)
    {
    }
    if(!ok)
    {
        Util::addError(error);
    }
}

/******************************************************************************/

void DownloadManager::processDay(const QDate & date, const bool & force)
{
    Util::addMessage("Starting " + date.toString("yyyy-MM-dd"));
    // Init
    bool ok = true;
    QString error = "";
    QString dayUrl = "";
    QString html = "";
    QXmlStreamWriter xmlWriter;
    QString filename = Util::getLineFromConf("pathToRawData")
            + "/" + date.toString("yyyy-MM-dd") + ".xml";
    QFile file;
    // Check date
    if(ok
       && (date >= QDate::currentDate()))
    {
        ok = false;
        error = "invalid date : " + date.toString("yyyy-MM-dd") + " >= today";
    }
    // Check file
    if(ok && !force && QFile::exists(filename))
    {
        ok = false;
        error = "the file already exists "
                + QFileInfo(filename).absoluteFilePath();
    }
    // Open file
    if(ok)
    {
        file.setFileName(filename);
        if (!file.open(QFile::WriteOnly))
        {
            ok = false;
            error = "cannot open file " + QFileInfo(file).absoluteFilePath();
        }
    }
    // Prepare url
    if(ok)
    {
        dayUrl = Util::getLineFromConf("dayUrl");
        dayUrl.replace("DATE", date.toString("yyyy-MM-dd"));
    }
    // Download html
    if(ok)
    {
        html = getHtml(dayUrl);
    }
    // Parsing
    if(ok)
    {
        xmlWriter.setDevice(&file);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("day");
        {
            xmlWriter.writeTextElement("date", date.toString("yyyy-MM-dd"));
            QVector<QString> reunions;
            QRegExp rx("href=\"([^\"]*id=([0-9]*)[^\"]*)\" "
                       "title=\"([^\"]*)\" "
                       "class=\"halfpill\">(R[0-9]+)<");
            int pos = 0;
            while ((pos = rx.indexIn(html, pos)) != -1)
            {
                pos += rx.matchedLength();
                QString url = rx.cap(1);
                QString zeturfId = rx.cap(2);
                QString name = rx.cap(3);
                QString id = rx.cap(4);
                bool addReunion = true;
                for(int i = 0 ; i < reunions.size() ; i++)
                {
                    if(name == reunions[i])
                    {
                        addReunion = false;
                    }
                }
                if(addReunion)
                {
                    processReunion(xmlWriter, url, zeturfId, name, id);
                    reunions.push_back(name);
                }
            }
        }
        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
    }
    // End
    if(ok)
    {
        Util::addMessage("File ready at " + QFileInfo(file).absoluteFilePath());
    }
    if(!ok)
    {
        Util::addError(error);
    }
}

void DownloadManager::processReunion(QXmlStreamWriter & xmlWriter,
                                     const QString & url,
                                     const QString & zeturfId,
                                     const QString & name,
                                     const QString & id)
{
    (void)zeturfId;
    xmlWriter.writeStartElement("reunion");
    {
        xmlWriter.writeTextElement("id", id);
        xmlWriter.writeTextElement("name", name);
        QString html = getHtml(url);
        QVector<QString> races;
        QRegExp rx("href=\"([^\"]*id=([0-9]*))\" "
                   "title=\""+name+" - ([^\"]*)\" "
                   +"class=\"pill\">&nbsp;"+id+" (C[0-9]+)");
        int pos = 0;
        while ((pos = rx.indexIn(html, pos)) != -1)
        {
            pos += rx.matchedLength();
            QString url = rx.cap(1);
            QString zeturfId = rx.cap(2);
            QString name = rx.cap(3);
            QString id = rx.cap(4);
            bool addRace = true;
            for(int i = 0 ; i < races.size() ; i++)
            {
                if(name == races[i])
                {
                    addRace = false;
                }
            }
            if(addRace)
            {
                processRace(xmlWriter, url, zeturfId, name, id);
                races.push_back(name);
            }
        }
    }
    xmlWriter.writeEndElement();
}

void DownloadManager::processRace(QXmlStreamWriter & xmlWriter,
                                  const QString & url,
                                  const QString & zeturfId,
                                  const QString & name,
                                  const QString & id)
{
    (void)url;
    xmlWriter.writeStartElement("race");
    {
        xmlWriter.writeTextElement("id", id);
        xmlWriter.writeTextElement("name", name);
        // Départ
        {
            QString html = getHtml("http://www.zeturf.fr/fr/"
                                   "programmes-et-pronostics/course?id="
                                   + zeturfId);
            QRegExp rx1("href=\"([^\"]*)\"[^t]*"
                        "title=\"([^\"]*)\"[^i]*"
                        "id=\"myrunner_[0-9]*\">\\2</a>");
            QRegExp rx2("<td>([^<]*)</td>[^<]*"
                        "<td>[^<]*</td>[^<]*"
                        "<td>([^<]*)</td>");
            int pos = 0;
            while ((pos = rx1.indexIn(html, pos)) != -1)
            {
                pos += rx1.matchedLength();
                QString url = rx1.cap(1);
                QString pony = rx1.cap(2);
                //break;
                if((pos = rx2.indexIn(html, pos)) != -1)
                {
                    pos += rx2.matchedLength();
                    QString jockey = rx2.cap(1);
                    QString trainer = rx2.cap(2);
                    xmlWriter.writeStartElement("team");
                    xmlWriter.writeTextElement("pony", pony);
                    xmlWriter.writeTextElement("jockey", jockey);
                    xmlWriter.writeTextElement("trainer", trainer);
                    xmlWriter.writeEndElement();
                }
                else
                {
                    break;
                }
            }
        }
        // Arrivée
        {
            //<b class="bigtext">14 - 11 - 2 - 6 - 19 - 1 - 18</b>
            QString html = getHtml("http://www.zeturf.fr/fr/"
                                   "resultats-et-rapports/course?id="
                                   + zeturfId);
            QRegExp rx("<b class=\"bigtext\">"
                       "([^<]*)"
                       "</b>");
            int pos = 0;
            while ((pos = rx.indexIn(html, pos)) != -1)
            {
                pos += rx.matchedLength();
                QString arrival = rx.cap(1);
                xmlWriter.writeTextElement("arrival", arrival);
            }
        }
    }
    xmlWriter.writeEndElement();
}

const QString DownloadManager::getHtml(const QString & url)
{
    Util::addMessage("Download " + url);
    QNetworkRequest request(url);
    QNetworkAccessManager manager;
    QNetworkReply * download = manager.get(request);
    QEventLoop eventLoop;
    QObject::connect(download, SIGNAL(finished()),
                     &eventLoop, SLOT(quit()));
    eventLoop.exec(); // wait until download is done
    QString html = download->readAll();
    return html;
}

/******************************************************************************/

void DownloadManager::setCommand(const QString & command)
{
    this->command = command;
}
