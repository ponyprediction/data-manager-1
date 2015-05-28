#include "parser.hpp"
#include "util.hpp"
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>

Parser::Parser()
{

}

Parser::~Parser()
{

}

void Parser::parseDay(const QDate & date, const bool & force)
{
    Util::addMessage("Parsing " + date.toString("yyyy-MM-dd"));
    // Init
    bool ok = true;
    QString error = "";
    QString htmlFilename = Util::getLineFromConf("dayHtmlFilename");
    htmlFilename.replace("DATE", date.toString("yyyy-MM-dd"));
    QFile htmlFile;
    // Open files
    if(ok)
    {
        htmlFile.setFileName(htmlFilename);
        if (!htmlFile.open(QFile::ReadOnly))
        {
            ok = false;
            error = "cannot open file "
                    + QFileInfo(htmlFile).absoluteFilePath();
        }
    }
    //
    if(ok)
    {
        {
            QVector<QString> reunions;
            QRegExp rx("href=\"([^\"]*id=([0-9]*)[^\"]*)\" "
                       "title=\"([^\"]*)\" "
                       "class=\"halfpill\">(R[0-9]+)<");
            int pos = 0;
            QString html = htmlFile.readAll();
            while ((pos = rx.indexIn(html, pos)) != -1)
            {
                QString url = rx.cap(1);
                QString zeturfId = rx.cap(2);
                QString name = rx.cap(3);
                QString reunionId = rx.cap(4);
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
                    parseReunion(date.toString("yyyy-MM-dd"), reunionId,
                                 zeturfId, name, force);
                    reunions.push_back(name);
                }
                pos++;
            }
        }
    }
    // End
    if(ok)
    {
        //Util::addMessage("File ready at "
        //+ QFileInfo(xmlFile).absoluteFilePath());
    }
    if(!ok)
    {
        Util::addError(error);
    }
}


void Parser::parseReunion(const QString & date,
                          const QString & reunionId,
                          const QString & zeturfId,
                          const QString & name,
                          const bool & force)
{
    // Init
    bool ok = true;
    QString error = "";
    QString htmlFilename = Util::getLineFromConf("reunionHtmlFilename");
    htmlFilename.replace("DATE", date);
    htmlFilename.replace("REUNION_ID", reunionId);
    QFile htmlFile;
    // Open files
    if(ok)
    {
        htmlFile.setFileName(htmlFilename);
        if (!htmlFile.open(QFile::ReadOnly))
        {
            ok = false;
            error = "cannot open file "
                    + QFileInfo(htmlFile).absoluteFilePath();
        }
    }
    // Parsing
    {
        QVector<QString> races;
        QRegExp rx("href=\"([^\"]*id=([0-9]*))\" "
                   "title=\""+name+" - ([^\"]*)\" "
                   +"class=\"pill\">&nbsp;"+reunionId+" (C[0-9]+)");
        int pos = 0;
        QString html = htmlFile.readAll();
        while ((pos = rx.indexIn(html, pos)) != -1)
        {
            QString url = rx.cap(1);
            QString zeturfId = rx.cap(2);
            QString name = rx.cap(3);
            QString raceId = rx.cap(4);
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
                parseRace(date, reunionId, zeturfId, name, raceId, force);
                parseArrival(date, reunionId, zeturfId, name, raceId, force);
                races.push_back(name);
            }
            pos++;
        }
    }
    // End
    if(ok)
    {
        //Util::addMessage("Good");
    }
    if(!ok)
    {
        Util::addError(error);
    }
}

void Parser::parseRace(const QString & date,
                       const QString & reunionId,
                       const QString & zeturfId,
                       const QString & name,
                       const QString & raceId,
                       const bool & force)
{
    // Init
    bool ok = true;
    QString error = "";
    QString htmlFilename = Util::getLineFromConf("startHtmlFilename");
    htmlFilename.replace("DATE", date);
    htmlFilename.replace("REUNION_ID", reunionId);
    htmlFilename.replace("RACE_ID", raceId);
    QFile htmlFile;
    QString htmlFilename2 = Util::getLineFromConf("oddsHtmlFilename");
    htmlFilename2.replace("DATE", date);
    htmlFilename2.replace("REUNION_ID", reunionId);
    htmlFilename2.replace("RACE_ID", raceId);
    QFile htmlOddsFile;
    QString jsonFilename = Util::getLineFromConf("raceJsonFilename");
    jsonFilename.replace("DATE", date);
    jsonFilename.replace("REUNION_ID", reunionId);
    jsonFilename.replace("RACE_ID", raceId);
    QFile jsonFile;
    QString completeRaceId = date + "-" + reunionId + "-" + raceId;
    // Check force
    if(ok && !force && QFile::exists(jsonFilename))
    {
        ok = false;
        error = "the file already exists "
                + QFileInfo(jsonFilename).absoluteFilePath();
    }
    // Open files
    if(ok)
    {
        htmlFile.setFileName(htmlFilename);
        if (!htmlFile.open(QFile::ReadOnly))
        {
            ok = false;
            error = "cannot open file "
                    + QFileInfo(htmlFile).absoluteFilePath();
        }
    }
    if(ok)
    {
        htmlOddsFile.setFileName(htmlFilename2);
        if (!htmlOddsFile.open(QFile::ReadOnly))
        {
            ok = false;
            error = "cannot open file "
                    + QFileInfo(htmlOddsFile).absoluteFilePath();
        }
    }
    if(ok)
    {
        jsonFile.setFileName(jsonFilename);
        if (!jsonFile.open(QFile::WriteOnly))
        {
            ok = false;
            error = "cannot open file "
                    + QFileInfo(jsonFile).absoluteFilePath();
        }
    }
    // Other init
    QString html = htmlFile.readAll();
    html.replace('\n',' ');
    QString htmlOdds = htmlOddsFile.readAll();
    htmlOdds.replace('\n',' ');
    QStringList ponies;
    QStringList jockeys;
    QStringList trainers;
    QStringList odds;
    // Parsing start
    if(ok)
    {
        QRegularExpression rxTeams(
                    "</td>[^\"]*<td(?: class=\"blur\")?>[^\"]*"
                    "<a href=\"([^\"]*)\" "
                    "title=\"([^\"]*)\" "
                    "id="
                    "\"myrunner_[0-9]*\">(?:.*?)</a>(.*?)"
                    "<td(?: class=\"blur\")?>([^<]*)</td>[^<]*"
                    "<td(?: class=\"blur\")?>[^<]*</td>[^<]*"
                    "<td(?: class=\"blur\")?>([^<]*)</td>"
                    );
        QRegularExpressionMatchIterator matchIterator
                = rxTeams.globalMatch(html);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            ponies << match.captured(2);
            jockeys << match.captured(4);
            trainers << match.captured(5);
        }
    }
    // Parsing odds
    if(ok)
    {
        QRegularExpression rxOdds(
                    "<a href=\"[^\"]*\" "
                    "title=\"([^\"]*)\" "       // 1 - pony
                    "id=\"myrunner([^\"]*)\">"  // 2 -
                    "\\1</a>(?:.*?)"
                    "</td>[^<]*<td( class=\"textright(?: focus)?\")?>"
                    "([^<]*)</td>"
                    );
        QRegularExpressionMatchIterator matchIterator
                = rxOdds.globalMatch(htmlOdds);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            odds << QString::number(match.captured(4).toDouble());
        }
    }
    // Same pony count ?
    if(ok && (ponies.size() != odds.size()))
    {
        ok = false;
        error = "not same pony count";
    }
    // Same non partant count ?
    if(ok)
    {
        int npCount = html.count("(NP)");
        int oddNullCount = 0;
        for(int i = 0 ; i < ponies.size() ; i++)
        {
            if(odds[i]=="0")
            {
                oddNullCount++;
            }
        }
        if(oddNullCount*12 != npCount)
        {
            ok = false;
            error = "not same non partant count for : " + completeRaceId;
        }
    }
    // Write down JSON ..
    if(ok)
    {
        QJsonDocument document;
        QJsonObject race;
        race["zeturfId"] = zeturfId;
        race["name"] = name;
        race["date"] = date;
        race["reunion"] = reunionId;
        race["completeId"] = completeRaceId ;
        race["id"] = raceId;
        race["ponyCount"] = ponies.size();
        QJsonArray teams;
        for (int i = 0 ; i < ponies.size() ; i++)
        {
            QJsonObject team;
            team["number"] = i+1;
            team["pony"] = ponies[i];
            team["odd"] = odds[i];
            team["trainer"] = trainers[i];
            team["jockey"] = jockeys[i];
            teams.append(team);
        }
        race["teams"] = teams;
        document.setObject(race);
        jsonFile.write(document.toJson());
    }
    // End
    htmlFile.close();
    jsonFile.close();
    htmlOddsFile.close();
    if(ok)
    {
        //Util::addMessage("Good");
    }
    if(!ok)
    {
        Util::addError(error);
    }
}

void Parser::parseArrival(const QString & date,
                          const QString & reunionId,
                          const QString & zeturfId,
                          const QString & name,
                          const QString & raceId,
                          const bool & force)
{
    // Init
    bool ok = true;
    QString error = "";
    QString htmlFilename = Util::getLineFromConf("arrivalHtmlFilename");
    htmlFilename.replace("DATE", date);
    htmlFilename.replace("REUNION_ID", reunionId);
    htmlFilename.replace("RACE_ID", raceId);
    QFile htmlFile;

    QString JsonFilename = Util::getLineFromConf("arrivalJsonFilename");
    JsonFilename.replace("DATE", date);
    JsonFilename.replace("REUNION_ID", reunionId);
    JsonFilename.replace("RACE_ID", raceId);
    QFile jsonFile;
    QString completeRaceId = date + "-" + reunionId + "-" + raceId;
    // Check force
    if(ok && !force && QFile::exists(JsonFilename))
    {
        ok = false;
        error = "the file already exists "
                + QFileInfo(JsonFilename).absoluteFilePath();
    }
    // Check race exist
    if(ok)
    {
        QString filename = Util::getLineFromConf("raceJsonFilename");
        filename.replace("DATE", date);
        filename.replace("REUNION_ID", reunionId);
        filename.replace("RACE_ID", raceId);
        if(!QFile::exists(filename))
        {
            ok = false;
            error = "the corresponding race doesn't exist "
                    + QFileInfo(filename).absoluteFilePath();
        }
    }
    // Open files
    if(ok)
    {
        htmlFile.setFileName(htmlFilename);
        if (!htmlFile.open(QFile::ReadOnly))
        {
            ok = false;
            error = "cannot open file "
                    + QFileInfo(htmlFile).absoluteFilePath();
        }
    }
    if(ok)
    {
        jsonFile.setFileName(JsonFilename);
        if (!jsonFile.open(QFile::WriteOnly))
        {
            ok = false;
            error = "cannot open file "
                    + QFileInfo(jsonFile).absoluteFilePath();
        }
    }
    // Other init
    QString html = htmlFile.readAll();
    html.replace('\n',' ');
    QStringList ponies;
    // Parsing start
    if(ok)
    {
        QRegularExpression rx(
                    "id=\"myrunner_[0-9]*\">([^\<]*)</a>"
                    "");
        QRegularExpressionMatchIterator matchIterator
                = rx.globalMatch(html);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            ponies << match.captured(1);
        }
    }
    //
    if(ok)
    {
        if(ponies.size() == 7)
        {
            //Util::addMessage("good");
        }
        else if(ponies.size() < 7)
        {
            Util::addMessage("Less than 7 pony : " + completeRaceId);
        }
        else
        {
            ok = false;
            error = "more than 7 ponies";
        }
    }
    // Write down JSON ..
    if(ok)
    {

        QJsonDocument document;
        QJsonObject arrival;
        arrival["zeturfId"] = zeturfId;
        arrival["name"] = name;
        arrival["date"] = date;
        arrival["reunion"] = reunionId;
        arrival["completeId"] = completeRaceId ;
        arrival["id"] = raceId;
        QJsonArray pony;
        for (int i = 0 ; i < ponies.size() ; i++)
        {
            pony.append(ponies[i]);
        }
        arrival["ponies"] = pony;
        document.setObject(arrival);
        jsonFile.write(document.toJson());
    }
    // End
    htmlFile.close();
    jsonFile.close();
    if(ok)
    {
        //Util::addMessage("Good");
    }
    if(!ok)
    {
        Util::addError(error);
    }
}
