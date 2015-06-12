#include "parser.hpp"
#include "util.hpp"
#include "database-manager.hpp"
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

Parser::Parser() {

}

Parser::~Parser() {

}

void Parser::parseDay(const RacePart & racePart,
                      const QDate & date,
                      const bool & force)
{
    // Init
    Util::overwrite("Parsing " + date.toString("yyyy-MM-dd"));
    bool ok = true;
    QString error = "";
    QString htmlFilename = Util::getLineFromConf("dayHtmlFilename", &ok);
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
                parseReunion(racePart,
                             date.toString("yyyy-MM-dd"),
                             reunionId,
                             zeturfId,
                             name,
                             force);
                reunions.push_back(name);
            }
            pos++;
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
        Util::writeError(error);
    }
}


void Parser::parseReunion(const RacePart & racePart,
                          const QString & date,
                          const QString & reunionId,
                          const QString & zeturfId,
                          const QString & name,
                          const bool & force)
{
    // Init
    Util::overwrite("Parsing " + date + "-" + reunionId);
    bool ok = true;
    QString error = "";
    QString htmlFilename = Util::getLineFromConf("reunionHtmlFilename", &ok);
    htmlFilename.replace("DATE", date);
    htmlFilename.replace("REUNION_ID", reunionId);
    QFile htmlFile;
    // Open files
    if(ok) {
        htmlFile.setFileName(htmlFilename);
        if (!htmlFile.open(QFile::ReadOnly)) {
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
                switch(racePart)
                {
                    case ALL:
                    {
                        parseStart(date, reunionId, zeturfId, name, raceId, force);
                        addEnd(date, reunionId, zeturfId, name, raceId, force);
                        break;
                    }
                    case START:
                    {
                        parseStart(date, reunionId, zeturfId, name, raceId, force);
                        break;
                    }
                    case END:
                    {
                        addEnd(date, reunionId, zeturfId, name, raceId, force);
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
                races.push_back(name);
            }
            pos++;
        }
    }
    // End
    if(ok) {
        //Util::addMessage("Good");
    }
    if(!ok) {
        Util::writeError(error);
    }
}


void Parser::parseStart(const QString & date,
                        const QString & reunionId,
                        const QString & zeturfId,
                        const QString & name,
                        const QString & raceId,
                        const bool & force)
{
    // Init
    QString id = date + "-" + reunionId + "-" + raceId;
    Util::overwrite("Parsing " + id);
    bool ok = true;

    QFile htmlStartFile;
    QString htmlStartFilename = Util::getLineFromConf("startHtmlFilename", &ok);
    htmlStartFilename.replace("DATE", date);
    htmlStartFilename.replace("REUNION_ID", reunionId);
    htmlStartFilename.replace("RACE_ID", raceId);

    QFile htmlOddsFile;
    QString htmOddslFilename = Util::getLineFromConf("oddsHtmlFilename", &ok);
    htmOddslFilename.replace("DATE", date);
    htmOddslFilename.replace("REUNION_ID", reunionId);
    htmOddslFilename.replace("RACE_ID", raceId);

    QString jsonFilename = Util::getLineFromConf("raceFileName", &ok);
    jsonFilename.replace("DATE", date);
    jsonFilename.replace("REUNION_ID", reunionId);
    jsonFilename.replace("RACE_ID", raceId);
    QFile jsonFile;
    // Check force
    if(ok && !force && QFile::exists(jsonFilename))
    {
        ok = false;
        Util::writeError("can't parse start of " + id
                         + " : the file already exists at "
                         + QFileInfo(jsonFilename).absoluteFilePath());
    }
    // Open html files
    if(ok)
    {
        htmlStartFile.setFileName(htmlStartFilename);
        if (!htmlStartFile.open(QFile::ReadOnly))
        {
            ok = false;
            Util::writeError("cannot open file "
                             + QFileInfo(htmlStartFile).absoluteFilePath());
        }
    }
    if(ok)
    {
        htmlOddsFile.setFileName(htmOddslFilename);
        if (!htmlOddsFile.open(QFile::ReadOnly))
        {
            ok = false;
            Util::writeError("cannot open file "
                             + QFileInfo(htmlOddsFile).absoluteFilePath());
        }
    }
    // Other init
    QString html;
    QString htmlOdds;
    QStringList ponies;
    QStringList jockeys;
    QStringList trainers;
    QStringList odds;
    if(ok)
    {
        html = htmlStartFile.readAll();
        html.replace('\n', ' ');
        htmlOdds = htmlOddsFile.readAll();
        htmlOdds.replace('\n', ' ');
    }
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
        Util::writeError("not same pony count between start and odds for "
                         + id
                         + " : " + QString::number(ponies.size())
                         + " - " + QString::number(odds.size()));
    }
    // Same non partant count ?
    if(ok) {
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
            Util::writeError("not same non partant count for "
                             + id
                             + " : " + QString::number(oddNullCount)
                             + " - " + QString::number(npCount));

        }
    }
    // Write down JSON ..
    if(ok)
    {
        jsonFile.setFileName(jsonFilename);
        if (!jsonFile.open(QFile::WriteOnly))
        {
            ok = false;
            Util::writeError("cannot open file "
                             + QFileInfo(jsonFile).absoluteFilePath());
        }
    }
    // Write to json
    if(ok)
    {
        //
        QJsonDocument document;
        QJsonObject race;
        QString dateInt = date;
        dateInt.remove('-');
        QJsonArray teams;
        //
        race["status"] = "start";
        race["zeturfId"] = zeturfId;
        race["name"] = name;
        race["date"] = dateInt.toInt();
        race["reunion"] = reunionId;
        race["race"] = raceId;
        race["id"] = id ;
        race["ponyCount"] = ponies.size();
        for (int i = 0 ; i < ponies.size() ; i++)
        {
            QJsonObject team;
            team["id"] = i+1;
            team["pony"] = ponies[i];
            team["odds"] = odds[i];
            team["trainer"] = trainers[i];
            team["jockey"] = jockeys[i];
            teams.append(team);
        }
        //
        race["teams"] = teams;
        document.setObject(race);
        jsonFile.write(document.toJson());
    }
    // End
    htmlStartFile.close();
    jsonFile.close();
    htmlOddsFile.close();
}


void Parser::addEnd(const QString & date,
                    const QString & reunionId,
                    const QString & zeturfId,
                    const QString & name,
                    const QString & raceId,
                    const bool & force)
{
    // Init
    QString id = date + "-" + reunionId + "-" + raceId;
    Util::overwrite("Adding end " + id);
    bool ok = true;

    QFile htmlFile;
    QString htmlFilename = Util::getLineFromConf("arrivalHtmlFilename", &ok);
    htmlFilename.replace("DATE", date);
    htmlFilename.replace("REUNION_ID", reunionId);
    htmlFilename.replace("RACE_ID", raceId);
    QString html;

    QFile jsonFile;
    QString jsonFilename = Util::getLineFromConf("raceFileName", &ok);
    jsonFilename.replace("DATE", date);
    jsonFilename.replace("REUNION_ID", reunionId);
    jsonFilename.replace("RACE_ID", raceId);

    QJsonObject race;

    // Check files exists
    if(ok && !QFile::exists(htmlFilename))
    {
        ok = false;
        Util::writeError("the corresponding html doesn't exist "
                         + QFileInfo(htmlFilename).absoluteFilePath());
    }
    if(ok && !QFile::exists(jsonFilename))
    {
        ok = false;
        Util::writeError("the corresponding race doesn't exist "
                         + id);
    }
    // Open files
    if(ok)
    {
        htmlFile.setFileName(htmlFilename);
        if (!htmlFile.open(QFile::ReadOnly))
        {
            ok = false;
            Util::writeError("cannot open file "
                             + QFileInfo(htmlFile).absoluteFilePath());
        }
    }
    if(ok)
    {
        jsonFile.setFileName(jsonFilename);
        if (!jsonFile.open(QFile::ReadOnly))
        {
            ok = false;
            Util::writeError("cannot open file "
                             + QFileInfo(jsonFile).absoluteFilePath());
        }
    }
    // Get html from file
    if(ok)
    {
        html = htmlFile.readAll();
        html.replace('\n',' ');
    }
    // Get json from file
    if(ok)
    {
        race = QJsonDocument::fromJson(jsonFile.readAll()).object();
        jsonFile.close();
        if(race.empty())
        {
            ok = false;
            Util::writeError("problem while getting the json for "
                             + id);
        }
    }
    // Check already arrival added
    if(ok)
    {
        if(force)
        {
            QString status;
            status = race["status"].toString();
            status.replace("-arrival", "");
        }
        else
        {
            if(race["status"].toString().contains("-arrival"))
            {
                ok = false;
                Util::writeError("can't overwrite arrival for "
                                 + id);
            }
        }
    }
    // Other init
    QStringList ponies;
    QStringList teamIds;
    QStringList jockeys;
    QStringList ranks;
    // Parsing
    if(ok)
    {
        QRegularExpression rx(
                    "<td class=\"first\"><b>([1-7])<sup>[^<]*</sup></b></td>[^<]*"
                    "<td>([0-9]*)</td>[^<]*"
                    "<td>[^<]*"
                    "<a href=\"[^\"]*\" "
                    "title=\"[^\"]*\" "
                    "id=\"myrunner_[0-9]*\">([^\<]*)</a>[^<]*"
                    "</td>[^<]*"
                    "<td>([^<]*)</td>"
                    "");
        QRegularExpressionMatchIterator matchIterator
                = rx.globalMatch(html);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            ranks << match.captured(1);
            teamIds << match.captured(2);
            ponies << match.captured(3);
            jockeys << match.captured(4);
        }
    }
    //
    if(ok)
    {
        if(ponies.size() == 7)
        {}
        else if(ponies.size() < 7 && ponies.size() > 0)
        {
            Util::writeWarning(QString::number(ponies.size())
                               + " ponies in " + id);
        }
        else if(ponies.size() < 1)
        {
            ok = false;
            Util::writeError("less than 1 pony in " + id + " - end");
        }
        else
        {
            ok = false;
            Util::writeError("more than 7 ponies in " + id+ " - end");
        }
    }
    // Check json
    if(ok)
    {
        if(race["zeturfId"] != zeturfId)
        {
            ok = false;
            Util::writeError("not same zeturfId for " + id);
        }
        if(race["name"] != name)
        {
            ok = false;
            Util::writeError("not same name for " + id);
        }
        if(race["reunion"] != reunionId)
        {
            ok = false;
            Util::writeError("not same reunion for " + id);
        }
        if(race["id"] != id)
        {
            ok = false;
            Util::writeError("not same id for " + id);
        }
        if(race["race"] != raceId)
        {
            ok = false;
            Util::writeError("not same id for " + id);
        }
    }
    // Rewrite JSON ..
    if(ok)
    {
        jsonFile.setFileName(jsonFilename);
        if (!jsonFile.open(QFile::WriteOnly))
        {
            ok = false;
            Util::writeError("cannot open file "
                             + QFileInfo(jsonFile).absoluteFilePath());
        }
    }
    if(ok)
    {
        QJsonArray teams = race["teams"].toArray();
        QJsonArray newTeams;
        for (int i = 0 ; i < teams.size() ; i++)
        {
            QJsonObject team = teams[i].toObject();

            team["rank"] = 0;

            for(int j = 0 ; j < ponies.size() ; j++)
            {
                if(team["id"].toInt() == teamIds[j].toInt()
                        && team["pony"] == ponies[j]
                        && team["jockey"] == jockeys[j])
                {
                    team["rank"] = ranks[j].toInt();
                }
            }
            newTeams.append(team);
        }
        race["teams"] = newTeams;
        race["status"] = race["status"].toString() + "-arrival";
        QJsonDocument document;
        document.setObject(race);
        jsonFile.write(document.toJson());
    }
    // End
    htmlFile.close();
    jsonFile.close();
}
