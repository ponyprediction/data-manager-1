#include "manager.hpp"
#include "util.hpp"
#include "download-manager.hpp"
#include "parser.hpp"
#include "training-set-creator.hpp"
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
    int state = 0;
    QStringList tasks;
    QStringList arguments;
    QDate dateStart = QDate::currentDate();
    QDate dateEnd = QDate::currentDate();
    QDate dateStartHistory = QDate::currentDate();
    // Parse command
    if(ok)
    {
        const int ANY = 0;
        const int ANY_BUT_TASK = 1;
        const int FROM_TO = 2;
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
                else if(commands[i] == "create-training-set")
                {
                    tasks << "create-training-set";
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
                    state = ANY_BUT_TASK;
                }
                else
                {
                    ok = false;
                    Util::writeError("unknown command " + commands[i]);
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
                    state = ANY_BUT_TASK;
                }
                else
                {
                    ok = false;
                    Util::writeError("unknown command " + commands[i]);
                }
                break;
            }
            case FROM_TO:
            {
                if(commands[i] == "to")
                {
                    i++;
                    dateEnd = QDate::fromString(commands[i], "yyyy-MM-dd");
                    state = ANY_BUT_TASK;
                }
                else
                {
                    ok = false;
                    Util::writeError("unknown command " + commands[i]);
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
        Util::writeError("difference between task count and argument count");
    }
    // Excute command
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
            else if(task == "create-training-set")
            {
                TrainingSetCreator::createTrainingSet(dateStart, dateEnd, dateStartHistory);
            }
            else
            {
                Util::writeWarning("me not understand task : " + task);
            }
        }
    }
    // The end
    if(ok)
    {
        Util::writeSuccess("Done");
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
            Util::writeWarning("me not understand argument : -"
                             + QString(args[j]));
        }
    }
}

void Manager::download(const QDate &dateStart, const QDate &dateEnd,
                       const bool &start, const bool &end, const bool &force)
{
    if(end && start)
    {
        Util::write("Download start & end from "
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
        Util::writeWarning("Download end from "
                         + dateStart.toString("yyyy-MM-dd")
                         + " to "
                         + dateEnd.toString("yyyy-MM-dd")
                         + " not implemented");
    }
    else if(start)
    {
        Util::writeWarning("Download start from "
                         + dateStart.toString("yyyy-MM-dd")
                         + " to "
                         + dateEnd.toString("yyyy-MM-dd")
                         + " not implemented");
    }
    else
    {
        Util::writeWarning("you can't donwload nothing");
    }
}

void Manager::parse(const QDate &dateStart, const QDate &dateEnd,
                    const bool &start, const bool &end, const bool &force)
{
    if(end && start)
    {
        Util::write("Parse start & end from "
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
        Util::write("Parse start from "
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
        Util::write("Parse end from "
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
        Util::writeWarning("you can't parse nothing");
    }
}

void Manager::insert(const QDate &dateStart, const QDate &dateEnd,
                     const bool &start, const bool &end, const bool &force)
{
    if(end && start)
    {
        DatabaseManager::insertData("start", dateStart, dateEnd);
        DatabaseManager::insertData("arrival", dateStart, dateEnd);
    }
    else if(end)
    {
        DatabaseManager::insertData("arrival", dateStart, dateEnd);
    }
    else if(start)
    {
        DatabaseManager::insertData("start", dateStart, dateEnd);
    }
    else
    {
        Util::writeWarning("you can't insert nothing");
    }
}
