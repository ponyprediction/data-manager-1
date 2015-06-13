#include "manager.hpp"
#include "util.hpp"
#include "download-manager.hpp"
#include "parser.hpp"
#include "solver/solver.hpp"
#include "training-set-creator.hpp"
#include "database-manager.hpp"
#include <QStringList>
#include <QDebug>


namespace State {enum State{ANY, FROM_TO, ANY_BUT_TASK};}


Manager::Manager()
{
}


Manager::~Manager()
{
}


void Manager::execute(const QString & command)
{
    // Init
    QStringList args = command.split(' ');
    bool ok = true;
    QStringList tasks;
    QStringList arguments;
    QDate dateStart = QDate::currentDate();
    QDate dateEnd = QDate::currentDate();
    QDate dateHistory = QDate::currentDate();
    QString dateStartStr = "today";
    QString dateEndStr = "today";
    QString dateHistoryStr = "today";
    QString brainId;
    //
    QStringList acceptedArgs;
    enum AcceptedArg{BRAIN=0, CREATE_TRAINING_SET, DOWNLOAD, FROM,
                     HISTORY, INSERT, PARSE, SOLVE, TO};
    acceptedArgs << "brain" << "create-training-set" << "download" << "from"
                 << "history" << "insert" << "parse" << "solve" << "to";
    State::State state = State::ANY;
    // Parse command
    if(ok)
    {
        for(int i = 0 ; i < args.size() ; i++)
        {
            switch(state)
            {
                case State::ANY:
                {
                    if(args[i] == acceptedArgs[DOWNLOAD])
                    {
                        tasks << acceptedArgs[DOWNLOAD];
                        arguments << "aF";
                    }
                    else if(args[i] == acceptedArgs[PARSE])
                    {
                        tasks << acceptedArgs[PARSE];
                        arguments << "aF";
                    }
                    else if(args[i] == acceptedArgs[INSERT])
                    {
                        tasks << acceptedArgs[INSERT];
                        arguments << "aF";
                    }
                    else if(args[i] == acceptedArgs[CREATE_TRAINING_SET])
                    {
                        tasks << acceptedArgs[CREATE_TRAINING_SET];
                        arguments << "aF";
                    }
                    else if(args[i] == acceptedArgs[SOLVE])
                    {
                        tasks << acceptedArgs[SOLVE];
                        arguments << "aF";
                    }
                    else if(args[i][0] == '-')
                    {
                        arguments.last() += args[i].mid(1);
                    }
                    else if(args[i] == acceptedArgs[FROM])
                    {
                        i++;
                        dateStartStr = args[i];
                        dateHistoryStr = dateStartStr;
                        state = State::FROM_TO;
                    }
                    else if(args[i] == acceptedArgs[HISTORY])
                    {
                        i++;
                        dateHistoryStr = args[i];
                        state = State::ANY_BUT_TASK;
                    }
                    else if(args[i] == acceptedArgs[BRAIN])
                    {
                        i++;
                        brainId = args[i];
                        state = State::ANY_BUT_TASK;
                    }
                    else
                    {
                        ok = false;
                        Util::writeError("unknown command " + args[i]);
                    }
                    break;
                }
                case State::ANY_BUT_TASK:
                {
                    if(args[i] == acceptedArgs[FROM])
                    {
                        i++;
                        dateStartStr = args[i];
                        dateHistoryStr = dateStartStr;
                        state = State::FROM_TO;
                    }
                    else if(args[i] == acceptedArgs[HISTORY])
                    {
                        i++;
                        dateHistoryStr = args[i];
                        state = State::ANY_BUT_TASK;
                    }
                    else if(args[i] == acceptedArgs[BRAIN])
                    {
                        i++;
                        brainId = args[i];
                        state = State::ANY_BUT_TASK;
                    }
                    else
                    {
                        ok = false;
                        Util::writeError("unknown command " + args[i]);
                    }
                    break;
                }
                case State::FROM_TO:
                {
                    if(args[i] == acceptedArgs[TO])
                    {
                        i++;
                        dateEndStr = args[i];
                        state = State::ANY_BUT_TASK;
                    }
                    else if(args[i] == acceptedArgs[HISTORY])
                    {
                        i++;
                        dateHistoryStr = args[i];
                        state = State::ANY_BUT_TASK;
                    }
                    else
                    {
                        ok = false;
                        Util::writeError("unknown command " + args[i]);
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
    // Parse dates
    if(ok)
    {
        //
        if(dateStartStr == "today")
        {
            dateStart = QDate::currentDate();
        }
        else if(dateStartStr == "yesterday")
        {
            dateStart  = QDate::currentDate().addDays(-1);
        }
        else
        {
            dateStart= QDate::fromString(dateStartStr, "yyyy-MM-dd");
        }
        //
        if(dateEndStr == "today")
        {
            dateEnd = QDate::currentDate();
        }
        else if(dateEndStr == "yesterday")
        {
            dateEnd  = QDate::currentDate().addDays(-1);
        }
        else
        {
            dateEnd = QDate::fromString(dateEndStr, "yyyy-MM-dd");
        }
        //
        if(dateHistoryStr == "today")
        {
            dateHistory = QDate::currentDate();
        }
        else if(dateHistoryStr == "yesterday")
        {
            dateHistory  = QDate::currentDate().addDays(-1);
        }
        else
        {
            dateHistory = QDate::fromString(dateHistoryStr, "yyyy-MM-dd");
        }
    }
    // Execute command
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
            if(task == acceptedArgs[DOWNLOAD])
            {
                download(dateStart, dateEnd, start, end, force);
            }
            else if(task == acceptedArgs[PARSE])
            {
                parse(dateStart, dateEnd, start, end, force);
            }
            else if(task == acceptedArgs[INSERT])
            {
                insert(dateStart, dateEnd, start, end, force);
            }
            else if(task == acceptedArgs[CREATE_TRAINING_SET])
            {
                TrainingSetCreator::createTrainingSet(dateStart, dateEnd, dateHistory);
            }
            else if(task == acceptedArgs[SOLVE])
            {
                for(QDate date = dateStart
                    ; date <= dateEnd
                    ; date = date.addDays(1))
                {
                    Solver::solve(date, dateHistory, brainId);
                }
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
    DatabaseManager::insertRace(dateStart, dateEnd, force);
    /*if(end && start)
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
    }*/
}
