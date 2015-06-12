#pragma once
#include <QDate>
#include <QJsonArray>
#include <QJsonObject>


class Solver
{

public:

    static void solve(const QDate & date,
                      const QDate & history, const QString & brainId);
private:

    static QJsonArray getProblems(const QDate & date,
                                  const QDate & history,
                                  bool & ok);

    static QJsonObject getProblem(const QString & id,
                                  const QDate & dateStartHistory,
                                  const QDate & dateEndHistory,
                                  bool & ok);

};
