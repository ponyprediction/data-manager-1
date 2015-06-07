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
                             date.toString("yyyy-MM-dd"), reunionId,
                             zeturfId, name, force);
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
        while ((pos = rx.indexIn(html, pos)) != -1) {
            QString url = rx.cap(1);
            QString zeturfId = rx.cap(2);
            QString name = rx.cap(3);
            QString raceId = rx.cap(4);
            bool addRace = true;
            for(int i = 0 ; i < races.size() ; i++) {
                if(name == races[i]) {
                    addRace = false;
                }
            }
            if(addRace) {
                switch(racePart)
                {
                case ALL:
                {
                    parseStart(date, reunionId, zeturfId, name, raceId, force);
                    parseEnd(date, reunionId, zeturfId, name, raceId, force);
                    break;
                }
                case START:
                {
                    parseStart(date, reunionId, zeturfId, name, raceId, force);
                    break;
                }
                case END:
                {
                    parseEnd(date, reunionId, zeturfId, name, raceId, force);
                    break;
                }
                default:
                    break;
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
                        const bool & force) {
    // Init
    QString completeRaceId = date + "-" + reunionId + "-" + raceId;
    Util::overwrite("Parsing " + completeRaceId);
    bool ok = true;
    QString htmlFilename = Util::getLineFromConf("startHtmlFilename", &ok);
    htmlFilename.replace("DATE", date);
    htmlFilename.replace("REUNION_ID", reunionId);
    htmlFilename.replace("RACE_ID", raceId);
    QFile htmlFile;
    QString htmlFilename2 = Util::getLineFromConf("oddsHtmlFilename", &ok);
    htmlFilename2.replace("DATE", date);
    htmlFilename2.replace("REUNION_ID", reunionId);
    htmlFilename2.replace("RACE_ID", raceId);
    QFile htmlOddsFile;
    QString jsonFilename = Util::getLineFromConf("startJsonFilename", &ok);
    jsonFilename.replace("DATE", date);
    jsonFilename.replace("REUNION_ID", reunionId);
    jsonFilename.replace("RACE_ID", raceId);
    QFile jsonFile;
    // Check force
    if(ok && !force && QFile::exists(jsonFilename)) {
        ok = false;
        Util::overwriteWarning("can't parse start of " + completeRaceId
                         + " : the file already exists ");
    }
    // Open files
    if(ok) {
        htmlFile.setFileName(htmlFilename);
        if (!htmlFile.open(QFile::ReadOnly)) {
            ok = false;
            Util::writeError("cannot open file "
                           + QFileInfo(htmlFile).absoluteFilePath());
        }
    }
    if(ok) {
        htmlOddsFile.setFileName(htmlFilename2);
        if (!htmlOddsFile.open(QFile::ReadOnly)) {
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
        html = htmlFile.readAll();
        html.replace('\n',' ');
        htmlOdds = htmlOddsFile.readAll();
        htmlOdds.replace('\n',' ');
    }
    // Parsing start
    if(ok) {
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
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            ponies << match.captured(2);
            jockeys << match.captured(4);
            trainers << match.captured(5);
        }
    }
    // Parsing odds
    if(ok) {
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
        Util::writeError("not same pony count");
    }
    // Same non partant count ?
    if(ok) {
        int npCount = html.count("(NP)");
        int oddNullCount = 0;
        for(int i = 0 ; i < ponies.size() ; i++) {
            if(odds[i]=="0") {
                oddNullCount++;
            }
        }
        if(oddNullCount*12 != npCount) {
            ok = false;
            Util::writeError("not same non partant count for : " + completeRaceId
                           + " : " + QString::number(oddNullCount) + " - "
                           + QString::number(npCount));

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
    if(ok) {
        QJsonDocument document;
        QJsonObject race;
        race["zeturfId"] = zeturfId;
        race["name"] = name;
        QString tempDate = date;
        race["date"] = tempDate.remove('-').toInt();
        race["reunion"] = reunionId;
        race["completeId"] = completeRaceId ;
        race["id"] = raceId;
        race["ponyCount"] = ponies.size();
        QJsonArray teams;
        for (int i = 0 ; i < ponies.size() ; i++) {
            QJsonObject team;
            team["id"] = i+1;
            team["pony"] = ponies[i];
            team["odds"] = odds[i];
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
}

void Parser::parseEnd(const QString & date,
                      const QString & reunionId,
                      const QString & zeturfId,
                      const QString & name,
                      const QString & raceId,
                      const bool & force) {
    // Init
    QString completeRaceId = date + "-" + reunionId + "-" + raceId;
    Util::overwrite("Parsing " + completeRaceId);
    bool ok = true;
    QString htmlFilename = Util::getLineFromConf("arrivalHtmlFilename", &ok);
    htmlFilename.replace("DATE", date);
    htmlFilename.replace("REUNION_ID", reunionId);
    htmlFilename.replace("RACE_ID", raceId);
    QFile htmlFile;

    QString JsonFilename = Util::getLineFromConf("arrivalJsonFilename", &ok);
    JsonFilename.replace("DATE", date);
    JsonFilename.replace("REUNION_ID", reunionId);
    JsonFilename.replace("RACE_ID", raceId);
    QFile jsonFile;
    QStringList listRanks;
    listRanks << "first" << "second" << "third" << "fourth"
              << "fifth" << "sixth" << "seventh";
    // Check force
    if(ok && !force && QFile::exists(JsonFilename)) {
        ok = false;
        Util::overwriteWarning("can't parse end of " + completeRaceId
                         + " : the file already exists ");
    }
    // Check race exist
    if(ok) {
        QString filename = Util::getLineFromConf("startJsonFilename", &ok);
        filename.replace("DATE", date);
        filename.replace("REUNION_ID", reunionId);
        filename.replace("RACE_ID", raceId);
        if(!QFile::exists(filename)) {
            ok = false;
            Util::writeError("the corresponding race doesn't exist "
                           + QFileInfo(filename).absoluteFilePath());
        }
    }
    // Open files
    if(ok) {
        htmlFile.setFileName(htmlFilename);
        if (!htmlFile.open(QFile::ReadOnly)) {
            ok = false;
            Util::writeError("cannot open file "
                           + QFileInfo(htmlFile).absoluteFilePath());
        }
    }
    // Other init
    QString html;
    QStringList ponies;
    QStringList teamIds;
    QStringList jockeys;
    QStringList ranks;
    QStringList trainers;
    if(ok)
    {
        html = htmlFile.readAll();
        html.replace('\n',' ');
    }
    // Parsing start
    if(ok) {
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
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            ranks << match.captured(1);
            teamIds << match.captured(2);
            ponies << match.captured(3);
            jockeys << match.captured(4);
        }
    }
    // Get Trainer
    if(ok) {
        QString completeraceId = date + "-" + reunionId + "-" + raceId;
        for (int i = 0 ; i < ponies.size() ; i++) {
            trainers << DatabaseManager::getTrainerInRaceWhereTeamAndPonyAndJockey(
                            completeraceId, teamIds[i].toInt(),
                            ponies[i], jockeys[i]);
        }
    }
    //
    if(ok) {
        if(ponies.size() == 7) {
        } else if(ponies.size() < 7 && ponies.size() > 0) {
            //Util::writeWarning(QString::number(ponies.size())
            //                 + " ponies in " + completeRaceId);
        } else if(ponies.size() < 1) {
            ok = false;
            Util::writeError("less than 1 pony in " + completeRaceId + " - end");
        } else {
            ok = false;
            Util::writeError("more than 7 ponies in " + completeRaceId+ " - end");
        }
    }
    // Write down JSON ..
    if(ok) {
        jsonFile.setFileName(JsonFilename);
        if (!jsonFile.open(QFile::WriteOnly)) {
            ok = false;
            Util::writeError("cannot open file "
                           + QFileInfo(jsonFile).absoluteFilePath());
        }
    }
    if(ok) {

        QJsonDocument document;
        QJsonObject arrival;
        arrival["zeturfId"] = zeturfId;
        arrival["name"] = name;
        QString tempDate = date;
        arrival["date"] = tempDate.remove('-').toInt();
        arrival["reunion"] = reunionId;
        arrival["completeId"] = completeRaceId ;
        arrival["id"] = raceId;
        QJsonArray teams;
        for (int i = 0 ; i < ponies.size() ; i++) {
            QJsonObject team;
            team["rank"] = ranks[i].toInt();
            team["id"] = teamIds[i].toInt();
            team["pony"] = ponies[i];
            team["jockey"] = jockeys[i];
            team["trainer"] = trainers[i];
            teams.append(team);
        }
        arrival["teams"] = teams;

        document.setObject(arrival);
        jsonFile.write(document.toJson());
    }
    // End
    htmlFile.close();
    jsonFile.close();
}
