#include "manager.hpp"
#include "util.hpp"
#include "download-manager.hpp"
#include "parser.hpp"
#include "job-creator.hpp"
#include "database-manager.hpp"
#include <QStringList>

Manager::Manager()
{
}

Manager::~Manager() {

}

void Manager::execute(const QString & command)
{
    // Init
    QStringList commandList = command.split(' ');
    bool ok = true;
    QString error = "";
    bool download = false;
    bool parse = false;
    bool createJob = false;
    bool commandIsSet = false;
    bool force = false;
    bool asynchrone = false;
    bool add = false;
    QString date1 = "";
    QString date2 = "";
    QString history = "";
    // Check folders
    if(ok) {
        QStringList paths;
        paths << Util::getLineFromConf("pathToHtml") + "/arrivals";
        paths << Util::getLineFromConf("pathToHtml") + "/days";
        paths << Util::getLineFromConf("pathToHtml") + "/odds";
        paths << Util::getLineFromConf("pathToHtml") + "/reunions";
        paths << Util::getLineFromConf("pathToHtml") + "/starts";
        paths << Util::getLineFromConf("pathToJson") + "/arrivals";
        paths << Util::getLineFromConf("pathToJson") + "/races";
        foreach(QString path, paths) {
            if(!Util::createDir(path)) {
                ok = false;
                error = "cannot create " + path;
                break;
            } else {
                Util::addMessage(path + " ok");
            }
        }
    }
    // Parsing command
    if(ok)
    {
        for(int i = 0 ; i < commandList.size() ; i++)
        {
            if(commandIsSet)
            {
                if(commandList[i] == "from"
                        && commandList[i+2] == "to"
                        && commandList[i+4] == "history")
                {
                    date1 = commandList[i+1];
                    date2 = commandList[i+3];
                    history = commandList[i+5];
                }
                else if(commandList[i] == "from"
                        && commandList[i+2] == "to")
                {
                    date1 = commandList[i+1];
                    date2 = commandList[i+3];
                }
                else if(commandList[i] == "-f")
                {
                    force = true;
                }
                else if(commandList[i] == "-a")
                {
                    asynchrone = true;
                }
            }
            else if(commandList[i] == "download")
            {
                download = true;
                commandIsSet = true;
                date1 = date2 = commandList[i+1];
            }
            else if(commandList[i] == "parse")
            {
                parse = true;
                commandIsSet = true;
                date1 = date2 = commandList[i+1];
            }
            else if(commandList[i] == "create-job")
            {
                createJob = true;
                commandIsSet = true;
                date1 = date2 = commandList[i+1];
            }
            else if(commandList[i] == "add")
            {
                add = true;
                commandIsSet = true;
                date1 = date2 = commandList[i+1];
            }
        }
    }
    if(ok && !commandIsSet)
    {
        ok = false;
        error = "invalid command";
    }
    // Applying command
    if(ok)
    {
        if(download)
        {
            QDate dateStart = QDate::fromString(date1, "yyyy-MM-dd");
            QDate dateEnd = QDate::fromString(date2, "yyyy-MM-dd");
            QDate date = dateStart;
            while(date <= dateEnd)
            {
                DownloadManager::downloadDay(date, force);
                date = date.addDays(1);
            }
        }
        if(parse)
        {
            QDate dateStart = QDate::fromString(date1, "yyyy-MM-dd");
            QDate dateEnd = QDate::fromString(date2, "yyyy-MM-dd");
            QDate date = dateStart;
            while(date <= dateEnd)
            {
                Parser::parseDay(date, force);
                date = date.addDays(1);
            }
        }
        if(createJob)
        {
            QDate dateStart = QDate::fromString(date1, "yyyy-MM-dd");
            QDate dateEnd = QDate::fromString(date2, "yyyy-MM-dd");
            QDate dateStartHistory = QDate::fromString(history, "yyyy-MM-dd");
            JobCreator::createJob(dateStart, dateEnd, dateStartHistory);
        }
        if(add)
        {
            QDate dateStart = QDate::fromString(date1, "yyyy-MM-dd");
            QDate dateEnd = QDate::fromString(date2, "yyyy-MM-dd");
            DatabaseManager::insertRace(dateStart, dateEnd);
            DatabaseManager::insertArrival(dateStart, dateEnd);
        }
    }
    // The end
    if(ok)
    {
        Util::addMessage("Done");
    }
    if(!ok)
    {
        Util::addError(error);
    }
}
