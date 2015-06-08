#include "util.hpp"
#include <QFile>
#include <QXmlStreamReader>
#include <iostream>
#include <QFileInfo>
#include <QDir>
#include "VT100.h"
#include <stdexcept>

bool Util::writeEnabled = true;
bool Util::overwriteEnabled = true;
bool Util::successEnabled = true;
bool Util::successOverwriteEnabled = true;
bool Util::warningsEnabled = true;
bool Util::warningOverwriteEnabled = true;
bool Util::errorEnabled = true;
bool Util::errorOverwriteEnabled = true;
bool Util::isOverwriting = false;

QString Util::confFileName = "./data-manager.conf";

void Util::init()
{
    //
    bool ok = true;
    QStringList paths;
    //
    if(ok)
    {
        writeEnabled = getLineFromConf("writeEnabled", &ok).toInt();
        overwriteEnabled = getLineFromConf("overwriteEnabled", &ok).toInt();
        successEnabled = getLineFromConf("successEnabled", &ok).toInt();
        successOverwriteEnabled = getLineFromConf(
                    "successOverwriteEnabled", &ok).toInt();
        warningsEnabled = getLineFromConf("warningsEnabled", &ok).toInt();
        warningOverwriteEnabled = getLineFromConf(
                    "warningOverwriteEnabled", &ok).toInt();
        errorEnabled = getLineFromConf("errorEnabled", &ok).toInt();
        errorOverwriteEnabled = getLineFromConf("errorOverwriteEnabled", &ok).toInt();
    }
    //
    if(ok)
    {
        paths << Util::getLineFromConf("pathToDaysHtml", &ok);
        paths << Util::getLineFromConf("pathToReunionsHtml", &ok);
        paths << Util::getLineFromConf("pathToStartsHtml", &ok);
        paths << Util::getLineFromConf("pathToOddsHtml", &ok);
        paths << Util::getLineFromConf("pathToArrivalsHtml", &ok);
        paths << Util::getLineFromConf("pathToStartsJson", &ok);
        paths << Util::getLineFromConf("pathToArrivalsJson", &ok);
        paths << Util::getLineFromConf("pathToTrainingSets", &ok);
        paths << Util::getLineFromConf("pathToRaces", &ok);
    }
    //
    if(ok)
    {
        foreach(QString path, paths)
        {
            if(!Util::createDir(path))
            {
                ok = false;
                Util::writeError("cannot create " + path);
                break;
            }
        }
    }
    //
    if(!ok)
    {
        throw std::runtime_error("Could not properly initialize the program.");
    }
}


void Util::showFile(const QString & path) {
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Util::write(file.readAll());
    } else {
        Util::writeError("can not find " + QFileInfo(file).absoluteFilePath());
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


QString Util::getLineFromConf(const QString & id, bool * ok)
{
    QString output = "";
    QFile file(confFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Util::writeError("can not find the conf file "
                         + QFileInfo(file).absoluteFilePath());
        return QString();
    }
    QXmlStreamReader xml(&file);
    while (!xml.atEnd())
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == id)
            {
                output = xml.readElementText();
            }
        }
    }
    if(!output.size())
    {
        if(ok)
        {
            *ok = false;
        }
        Util::writeError("can not find config line <" + id + ">");
    }
    return output;
}


void Util::write(const QString & message)
{
    if(isOverwriting)
    {
        overwrite("");
        isOverwriting = false;
    }
    std::cout << message.toStdString() << std::endl;
}


void Util::writeSuccess(const QString &message)
{
    if(successEnabled)
        write(VERT + message + RESET);
}


void Util::writeWarning(const QString &message)
{
    if(warningsEnabled)
        write(QString(JAUNE) + "Warning : " + message + RESET);
}


void Util::writeError(const QString & message)
{
    if(errorEnabled)
        write(QString(ROUGE) + "Error : " + message + RESET);
}


void Util::overwrite(const QString &message)
{
    if(overwriteEnabled)
    {
        isOverwriting = true;
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


void Util::overwriteSuccess(const QString &message)
{
    if(successEnabled && successOverwriteEnabled)
        overwrite(QString(VERT) + message + QString(RESET));
    else
        writeSuccess(message);
}


void Util::overwriteWarning(const QString &message)
{
    if(warningsEnabled && warningOverwriteEnabled)
        overwrite(QString(JAUNE)  +  "Warning : " + message + QString(RESET));
    else
        writeWarning(message);
}


void Util::overwriteError(const QString &message)
{
    if(warningsEnabled && warningOverwriteEnabled)
        overwrite(QString(JAUNE)  +  "Error : " + message + QString(RESET));
    else
        writeWarning(message);
}
