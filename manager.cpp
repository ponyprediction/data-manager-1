#include "manager.hpp"
#include "util.hpp"
#include "download-manager.hpp"

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
    //

    //
    QDate dateStart(2015,03,01);
    QDate dateEnd(2015,05,21);
    QDate date = dateStart;
    bool force = true;
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
    /*/
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
