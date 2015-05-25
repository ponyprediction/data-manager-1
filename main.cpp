#include <QCoreApplication>
#include <QDate>
#include "manager.hpp"
#include <QString>
#include <iostream>
#include <QRegExp>
#include <QStringList>
#include <QDebug>
#include <QVector>

int main(int argc, char *argv[])
{
    // Init
    QCoreApplication a(argc, argv);
    Manager manager;
    manager.execute("parse from 2014-01-01 to 2014-01-01 -f");
    // End
    return a.exec();
}
