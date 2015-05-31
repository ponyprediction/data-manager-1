#include "database-manager.hpp"
#include "manager.hpp"
#include "util.hpp"
#include <QCoreApplication>
#include <QDate>
#include <QString>
#include <QRegExp>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    //
    QCoreApplication a(argc, argv);
    Util::init();
    QString command;
    // Command from terminal or conf.xml ?
    if(argc > 1)
    {
        for(int i = 1 ; i < argc ; i++)
        {
            if(i-1)
            {
                command += " ";
            }
            command += argv[i];
        }
    }
    else
    {
        command = Util::getLineFromConf("command");
    }
    // Go
    Manager::execute(command);
    // The end
    return 0;
}
