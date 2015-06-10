#pragma once
#include <QDate>


class Solver
{

public:

    static void solve(const QDate & date,
                      const QDate & history);
private:

    static QString getProblems(bool & ok);

};
