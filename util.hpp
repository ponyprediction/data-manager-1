#ifndef UTIL_HPP
#define UTIL_HPP

#include <QString>
#include <QFile>

class Util {
public:
    static void init();
    static QString getLineFromConf(const QString & id, bool * ok);
    static void showFile(const QString & path);
    static bool createDir(const QString & path);
    static QString getFileName(const QFile & file);

    static void write(const QString & message);
    static void writeSuccess(const QString & message);
    static void writeWarning(const QString & message);
    static void writeError(const QString & message);

    static void overwrite(const QString &message);
    static void overwriteSuccess(const QString & message);
    static void overwriteWarning(const QString & message);
    static void overwriteError(const QString & message);

private:

    static bool writeEnabled;
    static bool overwriteEnabled;
    static bool successEnabled;
    static bool successOverwriteEnabled;
    static bool warningsEnabled;
    static bool warningOverwriteEnabled;
    static bool errorEnabled;
    static bool errorOverwriteEnabled;

    static bool isOverwriting;

    static QString confFileName;

};

#endif // UTIL_HPP
