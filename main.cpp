#include <QCoreApplication>
#include <QDate>
#include "manager.hpp"
#include <QString>
#include <iostream>
#include <QRegExp>
#include <QStringList>
#include <QDebug>

int main(int argc, char *argv[])
{
    // Init
    QCoreApplication a(argc, argv);
    Manager manager;
    // Start
    manager.processDay(QDate::fromString("2015-01-01", "yyyy-MM-dd"), true);
    // End
    return a.exec();
}
