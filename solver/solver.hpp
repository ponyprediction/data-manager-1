#pragma once
#include <QDate>
#include <QJsonArray>
#include <QJsonObject>


class Solver
{

public:

    static void solve(const QDate & date,
                      const QString & type,
                      const QString & brainId);
private:

    static QJsonArray getProblems(const QDate & date,
                                  const QString & type,
                                  bool & ok);

    static QJsonObject getProblem(const QString & id,
                                  bool & ok);

};
