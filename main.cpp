#include "database-manager.hpp"
#include "manager.hpp"
#include "util.hpp"
#include <QCoreApplication>
#include <QDate>
#include <QString>
#include <QRegExp>
#include <QStringList>
#include <QVector>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString command;
    if(argc > 1)
    {
        for(int i = 1 ; i < argc ; i++)
        {
            command += argv[i];
            command += " ";
        }
    }
    else
    {
        command = Util::getLineFromConf("command");
    }
    Manager::execute(command);
    return a.exec();
}
