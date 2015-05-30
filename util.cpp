#include "util.hpp"
#include <QFile>
#include <QXmlStreamReader>
#include <iostream>
#include <QFileInfo>
#include <QDir>
#include "VT100.h"

bool Util::overwriteEnabled = true;
bool Util::minorMessagesEnabled = true;
bool Util::warningsEnabled = true;
bool Util::overwriteWarningEnabled = false;

bool Util::overwriting = false;

void Util::init()
{
    overwriteEnabled = getLineFromConf("overwriteEnabled").toInt();
    minorMessagesEnabled = getLineFromConf("minorMessagesEnabled").toInt();
    warningsEnabled = getLineFromConf("warningsEnabled").toInt();
    overwriteWarningEnabled = getLineFromConf("overwriteWarningEnabled").toInt();
}

void Util::write(const QString & message)
{
    if(overwriting)
    {
        overwrite("");
        overwriting = false;
    }
    std::cout << message.toStdString() << std::endl;
}

void Util::overwrite(const QString &message)
{
    if(overwriteEnabled)
    {
        overwriting = true;
        std::cout << '\r';
        for(int i = 0 ; i < 80 ; i++)
        {
            std::cout << ' ';
        }
        std::cout << '\r' << message.toStdString() << std::flush;
    }
    else
    {
        write(message);
    }
}

void Util::addSuccess(const QString &success)
{
    write(VERT + success + RESET);
}

void Util::addWarning(const QString &warning)
{
    if(warningsEnabled)
        write(QString(JAUNE) + "Warning : " + warning + RESET);
}
void Util::overwriteWarning(const QString &warning)
{
    if(warningsEnabled && overwriteWarningEnabled)
        overwrite(QString(JAUNE)  +  "Warning : " + warning + QString(RESET));
    else
        addWarning(warning);
}

void Util::addError(const QString & error)
{
    write(QString(ROUGE) + "Error : " + error + RESET);
}


void Util::showFile(const QString & path) {
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Util::write(file.readAll());
    } else {
        Util::addError("can not find " + QFileInfo(file).absoluteFilePath());
    }
}

// Return true if directory already exists or on successful creation.
// Return false if can't create directory.
bool Util::createDir(const QString & path) {
    bool ok = true;
    QDir dir(path);
    if (!dir.exists()) {
        ok = dir.mkpath(".");
    }
    return ok;
}

QString Util::getFileName(const QFile &file)
{
    QFileInfo fileInfo(file.fileName());
    return fileInfo.fileName();
}

QString Util::getLineFromConf(const QString & id) {
    QString output = "";
    QFile file("./conf.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Util::addError("can not find the conf file "
                       + QFileInfo(file).absoluteFilePath());
        return QString();
    }
    QXmlStreamReader xml(&file);
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartElement) {
            if(xml.name() == id) {
                output = xml.readElementText();
            }
        }
    }
    if(!output.size())
        Util::addError("can not find config line <" + id + ">");
    return output;
}
