#include "parser.hpp"
#include "util.hpp"
#include <QFile>
#include <QFileInfo>
#include <QRegExp>

Parser::Parser()
{

}

Parser::~Parser()
{

}

void Parser::parseDay(const QDate & date, const bool & force)
{
    // Init
    bool ok = true;
    QString error = "";
    QString htmlFilename = Util::getLineFromConf("dayHtmlFilename");
    htmlFilename.replace("DATE", date.toString("yyyy-MM-dd"));
    QString xmlFilename = Util::getLineFromConf("dayXmlFilename");
    xmlFilename.replace("DATE", date.toString("yyyy-MM-dd"));
    QFile xmlFile;
    QFile htmlFile;
    QXmlStreamWriter xmlWriter;
    // Check file
    if(ok && !force && QFile::exists(xmlFilename))
    {
        ok = false;
        error = "the file already exists "
                + QFileInfo(xmlFilename).absoluteFilePath();
    }
    // Open files
    if(ok)
    {
        htmlFile.setFileName(htmlFilename);
        if (!htmlFile.open(QFile::ReadOnly))
        {
            ok = false;
            error = "cannot open file "
                    + QFileInfo(htmlFile).absoluteFilePath();
        }
    }
    if(ok)
    {
        xmlFile.setFileName(xmlFilename);
        if (!xmlFile.open(QFile::WriteOnly))
        {
            ok = false;
            error = "cannot open file "
                    + QFileInfo(xmlFile).absoluteFilePath();
        }
    }
    //
    if(ok)
    {
        xmlWriter.setDevice(&xmlFile);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("day");
        {
            xmlWriter.writeTextElement("date", date.toString("yyyy-MM-dd"));
            QVector<QString> reunions;
            QRegExp rx("href=\"([^\"]*id=([0-9]*)[^\"]*)\" "
                       "title=\"([^\"]*)\" "
                       "class=\"halfpill\">(R[0-9]+)<");
            int pos = 0;
            QString html = htmlFile.readAll();
            while ((pos = rx.indexIn(html, pos)) != -1)
            {
                pos += rx.matchedLength();
                QString url = rx.cap(1);
                QString zeturfId = rx.cap(2);
                QString name = rx.cap(3);
                QString id = rx.cap(4);
                bool addReunion = true;
                Util::addMessage(url);
                for(int i = 0 ; i < reunions.size() ; i++)
                {
                    if(name == reunions[i])
                    {
                        addReunion = false;
                    }
                }
                if(addReunion)
                {
                    parseReunion(xmlWriter, zeturfId, name, id);
                    reunions.push_back(name);
                }
            }
        }
        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
    }
    // End
    if(ok)
    {
        Util::addMessage("File ready at "
                         + QFileInfo(xmlFile).absoluteFilePath());
    }
    if(!ok)
    {
        Util::addError(error);
    }
}


void Parser::parseReunion(QXmlStreamWriter & xmlWriter,
                          const QString & id,
                          const QString & name,
                          const QString & number)
{
    // Init
    bool ok = true;
    QString error = "";
    QString htmlFilename = Util::getLineFromConf("reunionHtmlFilename");
    htmlFilename.replace("ID", id);
    QFile htmlFile;
    // Open files
    if(ok)
    {
        htmlFile.setFileName(htmlFilename);
        if (!htmlFile.open(QFile::ReadOnly))
        {
            ok = false;
            error = "cannot open file "
                    + QFileInfo(htmlFile).absoluteFilePath();
        }
    }
    // Parsing
    xmlWriter.writeStartElement("reunion");
    {
        xmlWriter.writeTextElement("id", id);
        xmlWriter.writeTextElement("name", name);
        xmlWriter.writeTextElement("number", number);
        QVector<QString> races;
        QRegExp rx("href=\"([^\"]*id=([0-9]*))\" "
                   "title=\""+name+" - ([^\"]*)\" "
                   +"class=\"pill\">&nbsp;"+number+" (C[0-9]+)");
        int pos = 0;
        QString html = htmlFile.readAll();
        while ((pos = rx.indexIn(html, pos)) != -1)
        {
            pos += rx.matchedLength();
            QString url = rx.cap(1);
            QString zeturfId = rx.cap(2);
            QString name = rx.cap(3);
            QString id = rx.cap(4);
            bool addRace = true;
            for(int i = 0 ; i < races.size() ; i++)
            {
                if(name == races[i])
                {
                    addRace = false;
                }
            }
            if(addRace)
            {
                //processRace(xmlWriter, url, zeturfId, name, id);
                races.push_back(name);
            }
        }
    }
    xmlWriter.writeEndElement();
    // End
    if(ok)
    {
        //Util::addMessage("Good");
    }
    if(!ok)
    {
        Util::addError(error);
    }
}
