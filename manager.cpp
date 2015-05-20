#include "manager.hpp"
#include "util.hpp"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QStringList>
#include <QVector>

Manager::Manager()
{

}

Manager::~Manager()
{

}

void Manager::processDay(const QDate & date)
{
    // Init
    bool ok = true;
    QString error = "";
    QString dayUrl = "";
    QString html = "";
    QString dayXml = "";
    QXmlStreamWriter xmlWriter(&dayXml);
    // Check date
    if(ok
       && (date >= QDate::currentDate()))
    {
        ok = false;
        error = "invalid date : " + date.toString("yyyy-MM-dd") + " >= today";
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
        Util::addMessage(dayXml);
    }
    // End
    if(!ok)
    {
        Util::addError(error);
    }
}

void Manager::processReunion(QXmlStreamWriter & xmlWriter,
                             const QString & url,
                             const QString & zeturfId,
                             const QString & name,
                             const QString & id)
{
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

void Manager::processRace(QXmlStreamWriter & xmlWriter,
                          const QString & url,
                          const QString & zeturfId,
                          const QString & name,
                          const QString & id)
{
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

        }
    }
    xmlWriter.writeEndElement();
}

const QString Manager::getHtml(const QString & url)
{
    Util::addMessage("Downloading " + url + "...");
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
