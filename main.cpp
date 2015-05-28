#include <QCoreApplication>
#include <QDate>
#include "manager.hpp"
#include "util.hpp"
#include <QString>
#include <iostream>
#include <QRegExp>
#include <QStringList>
#include <QDebug>
#include <QVector>
#include "database-manager.hpp"
int main(int argc, char *argv[])
{
    // Init
    QCoreApplication a(argc, argv);
    Manager manager;
    manager.execute(Util::getLineFromConf("command"));//from 2013-04 to 2013-07
    // End
    return a.exec();
}
