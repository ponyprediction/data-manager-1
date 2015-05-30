#include "manager.hpp"
#include "util.hpp"
#include "download-manager.hpp"
#include "parser.hpp"
#include "job-creator.hpp"
#include "database-manager.hpp"
#include <QStringList>
#include <QDebug>

Manager::Manager()
{
}

Manager::~Manager()
{
}

void Manager::execute(const QString & command)
{
    // Init
    QStringList commands = command.split(' ');
    bool ok = true;
    QString error = "";
    int state = 0;
    QStringList tasks;
    QStringList arguments;
    QDate dateStart = QDate::currentDate();
    QDate dateEnd = QDate::currentDate();
    QDate dateStartHistory = QDate::currentDate();
    QDate dateEndHistory = QDate::currentDate();
    // Check folders
    if(ok)
    {
        checkFolder(ok, error);
    }
    // Parsing command
    if(ok)
    {
        const int ANY = 0;
        const int ANY_BUT_TASK = 1;
        const int FROM_TO = 2;
        const int HISTORY_TO = 3;
        for(int i = 0 ; i < commands.size() ; i++)
        {
            switch(state)
            {
            case ANY:
            {
                if(commands[i] == "download")
                {
                    tasks << "download";
                    arguments << "aF";
                }
                else if(commands[i] == "parse")
                {
                    tasks << "parse";
                    arguments << "aF";
                }
                else if(commands[i] == "insert")
                {
                    tasks << "insert";
                    arguments << "aF";
                }
                else if(commands[i] == "create-job")
                {
                    tasks << "create-job";
                    arguments << "aF";
                }
                else if(commands[i][0] == '-')
                {
                    arguments.last() += commands[i].mid(1);
                }
                else if(commands[i] == "from")
                {
                    i++;
                    dateStart = QDate::fromString(commands[i], "yyyy-MM-dd");
                    dateStartHistory = dateStart;
                    state = FROM_TO;
                }
                else if(commands[i] == "history")
                {
                    i++;
                    dateStartHistory = QDate::fromString(commands[i],
                                                         "yyyy-MM-dd");
                    state = HISTORY_TO;
                }
                else
                {
                    ok = false;
                    error = "unknown command " + commands[i];
                }
                break;
            }
            case ANY_BUT_TASK:
            {
                if(commands[i] == "from")
                {
                    i++;
                    dateStart = QDate::fromString(commands[i], "yyyy-MM-dd");
                    dateStartHistory = dateStart;
                    state = FROM_TO;
                }
                else if(commands[i] == "history")
                {
                    i++;
                    dateStartHistory = QDate::fromString(commands[i],
                                                         "yyyy-MM-dd");
                    state = HISTORY_TO;
                }
                else
                {
                    ok = false;
                    error = "unknown command " + commands[i];
                }
                break;
            }
            case FROM_TO:
            {
                if(commands[i] == "to")
                {
                    i++;
                    dateEnd = QDate::fromString(commands[i], "yyyy-MM-dd");
                    dateEndHistory = dateEnd;
                    state = ANY_BUT_TASK;
                }
                else
                {
                    ok = false;
                    error = "unknown command " + commands[i];
                }
                break;
            }
            case HISTORY_TO:
            {
                if(commands[i] == "to")
                {
                    i++;
                    dateEndHistory = QDate::fromString(commands[i],
                                                       "yyyy-MM-dd");
                    state = ANY_BUT_TASK;
                }
                else
                {
                    ok = false;
                    error = "unknown command " + commands[i];
                }
                break;
            }
            default:
            {
                break;
            }
            }
        }
    }
    //
    if(ok && tasks.size() != arguments.size())
    {
        ok = false;
        error = "difference between task count and argument count";
    }
    //
    if(ok)
    {
        for(int i = 0 ; i < tasks.size() ; i++)
        {
            QString task = tasks[i];
            QString args = arguments[i];
            bool force = false;
            bool start = false;
            bool end = false;
            processArgs(args, start, end, force);
            if(task == "download")
            {
                download(dateStart, dateEnd, start, end, force);
            }
            else if(task == "parse")
            {
                parse(dateStart, dateEnd, start, end, force);
            }
            else if(task == "insert")
            {
                insert(dateStart, dateEnd, start, end, force);
            }
            else if(task == "create-job")
            {
                prepareJob(dateStart, dateEnd, dateStartHistory, dateEndHistory,
                           start, end, force);
            }
            else
            {
                Util::addWarning("me not understand task : " + task);
            }

        }
    }
    // The end
    if(ok)
    {
        Util::addSuccess("Done");
    }
    if(!ok)
    {
        Util::addError(error);
    }
}

void Manager::checkFolder(bool &ok, QString &error)
{
    QStringList paths;
    paths << Util::getLineFromConf("pathToHtml") + "/arrivals";
    paths << Util::getLineFromConf("pathToHtml") + "/days";
    paths << Util::getLineFromConf("pathToHtml") + "/odds";
    paths << Util::getLineFromConf("pathToHtml") + "/reunions";
    paths << Util::getLineFromConf("pathToHtml") + "/starts";
    paths << Util::getLineFromConf("pathToJson") + "/arrivals";
    paths << Util::getLineFromConf("pathToJson") + "/races";
    foreach(QString path, paths)
    {
        if(!Util::createDir(path))
        {
            ok = false;
            error = "cannot create " + path;
            break;
        }
        else
        {
            Util::addMinorMessage(path + " ok");
        }
    }
}

void Manager::processArgs(const QString &args, bool &start, bool &end,
                          bool &force)
{
    for(int j = 0 ; j < args.size() ; j++)
    {
        if(args[j] == 'F')
        {
            force = false;
        }
        else if(args[j] == 'f')
        {
            force = true;
        }
        else if(args[j] == 'a')
        {
            start = true;
            end = true;
        }
        else if(args[j] == 's')
        {
            start = true;
            end = false;
        }
        else if(args[j] == 'e')
        {
            start = false;
            end = true;
        }
        else
        {
            Util::addWarning("me not understand argument : -"
                             + QString(args[j]));
        }
    }
}

void Manager::download(const QDate &dateStart, const QDate &dateEnd,
                       const bool &start, const bool &end, const bool &force)
{
    if(end && start)
    {
        Util::addMessage("Download start & end from "
                         + dateStart.toString("yyyy-MM-dd")
                         + " to "
                         + dateEnd.toString("yyyy-MM-dd"));
        for(QDate date = dateStart
            ; date <= dateEnd
            ; date = date.addDays(1))
        {
            DownloadManager::downloadDay(date, force);
        }
    }
    else if(end)
    {
        Util::addWarning("Download end from "
                         + dateStart.toString("yyyy-MM-dd")
                         + " to "
                         + dateEnd.toString("yyyy-MM-dd")
                         + " not implemented");
    }
    else if(start)
    {
        Util::addWarning("Download start from "
                         + dateStart.toString("yyyy-MM-dd")
                         + " to "
                         + dateEnd.toString("yyyy-MM-dd")
                         + " not implemented");
    }
    else
    {
        Util::addWarning("you can't donwload nothing");
    }
}

void Manager::parse(const QDate &dateStart, const QDate &dateEnd,
                    const bool &start, const bool &end, const bool &force)
{
    if(end && start)
    {
        Util::addMessage("Parse start & end from "
                         + dateStart.toString("yyyy-MM-dd")
                         + " to "
                         + dateEnd.toString("yyyy-MM-dd"));
        for(QDate date = dateStart
            ; date <= dateEnd
            ; date = date.addDays(1))
        {
            Parser::parseDay(Parser::ALL, date, force);
        }
    }
    else if(start)
    {
        Util::addMessage("Parse start from "
                         + dateStart.toString("yyyy-MM-dd")
                         + " to "
                         + dateEnd.toString("yyyy-MM-dd"));
        for(QDate date = dateStart
            ; date <= dateEnd
            ; date = date.addDays(1))
        {
            Parser::parseDay(Parser::START, date, force);
        }
    }
    else if(end)
    {
        Util::addMessage("Parse end from "
                         + dateStart.toString("yyyy-MM-dd")
                         + " to "
                         + dateEnd.toString("yyyy-MM-dd"));
        for(QDate date = dateStart
            ; date <= dateEnd
            ; date = date.addDays(1))
        {
            Parser::parseDay(Parser::END, date, force);
        }
    }
    else
    {
        Util::addWarning("you can't parse nothing");
    }
}

void Manager::insert(const QDate &dateStart, const QDate &dateEnd,
                     const bool &start, const bool &end, const bool &force)
{
    if(end && start)
    {
        DatabaseManager::insertData("race", dateStart, dateEnd);
        DatabaseManager::insertData("arrival", dateStart, dateEnd);
    }
    else if(end)
    {
        DatabaseManager::insertData("arrival", dateStart, dateEnd);
    }
    else if(start)
    {
        DatabaseManager::insertData("race", dateStart, dateEnd);
    }
    else
    {
        Util::addWarning("you can't insert nothing");
    }
}

void Manager::prepareJob(
        const QDate &dateStart, const QDate &dateEnd,
        const QDate &dateStartHistory, const QDate &dateEndHistory,
        const bool &start, const bool &end, const bool &force)
{
    Util::addMessage("Insert start & end from "
                     + dateStart.toString("yyyy-MM-dd")
                     + " to "
                     + dateEnd.toString("yyyy-MM-dd"));
    JobCreator::createJob(dateStart, dateEnd, dateStartHistory);
}
