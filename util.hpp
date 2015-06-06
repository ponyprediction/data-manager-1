#ifndef UTIL_HPP
#define UTIL_HPP

#include <QString>
#include <QFile>

class Util {
public:
    static void init();
    static QString getLineFromConf(const QString & id);
    static void addSuccess(const QString & success);
    static void addWarning(const QString & warning);
    static void overwriteWarning(const QString & warning);
    static void addError(const QString & message);
    static void showFile(const QString & path);
    static bool createDir(const QString & path);
    static QString getFileName(const QFile & file);

    static void write(const QString & message);
    static void overwrite(const QString &message);

    static bool minorMessagesEnabled;
    static bool warningsEnabled;
    static bool overwriteEnabled;
    static bool overwriteWarningEnabled;

    static bool overwriting;

private:

    static QString confFileName;

};

#endif // UTIL_HPP
