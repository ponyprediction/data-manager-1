#include "manager.hpp"
#include "util.hpp"
#include "download-manager.hpp"
#include "parser.hpp"
#include <QStringList>

Manager::Manager() :
    downloadCount(0),
    finishedDonwloadCount(0)
{

}

Manager::~Manager()
{

}

void Manager::execute(const QString & command)
{
    // Init
    QStringList commandList = command.split(' ');
    bool ok = true;
    QString error = "";
    bool download = false;
    bool parse = false;
    bool commandIsSet = false;
    bool force = false;
    bool asynchrone = false;
    //bool add = false;
    QString date1 = "";
    QString date2 = "";
    // Parsing command
    if(ok)
    {
        for(int i = 0 ; i < commandList.size() ; i++)
        {
            if(commandIsSet)
            {
                if(commandList[i] == "from"
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
            DownloadManager downloadManager;
            QDate dateStart = QDate::fromString(date1, "yyyy-MM-dd");
            QDate dateEnd = QDate::fromString(date2, "yyyy-MM-dd");
            QDate date = dateStart;
            while(date <= dateEnd)
            {
                Util::addMessage("Download " + date.toString("yyyy-MM-dd"));
                downloadManager.downloadDay(date, force);
                date = date.addDays(1);
            }
        }
        if(parse)
        {
            Parser parser;
            QDate dateStart = QDate::fromString(date1, "yyyy-MM-dd");
            QDate dateEnd = QDate::fromString(date2, "yyyy-MM-dd");
            QDate date = dateStart;
            while(date <= dateEnd)
            {
                Util::addMessage("Parsing " + date.toString("yyyy-MM-dd"));
                parser.parseDay(date, force);
                date = date.addDays(1);
            }
        }
    }
    //
    /*QDate dateStart(2013,8,01); //10:05
    QDate dateEnd(2013,12,31);
    QDate date = dateStart;
    bool force = true;*/
    // Start
    /*/
    QVector<DownloadManager*> downloadManagers;
    while(date <= dateEnd)
    {
        downloadManagers.push_back(new DownloadManager());
        QString command = date.toString("yyyy-MM-dd");
        downloadManagers[downloadManagers.size()-1]->start(command);
        date = date.addDays(1);
        QThread::sleep(1);
    }
    //
    DownloadManager downloadManager;
    while(date <= dateEnd)
    {
        Util::addMessage("Start " + date.toString("yyyy-MM-dd"));
        downloadManager.downloadDay(date, force);
        date = date.addDays(1);
    }
    Util::addMessage("Done");
    /**/
}

void Manager::addFinishedDownolad()
{
    finishedDonwloadCount++;
    Util::addMessage("Downloaded "
                     + QString::number(finishedDonwloadCount)
                     + "/"
                     + QString::number(downloadCount));
}
