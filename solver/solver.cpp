#include "solver.hpp"
#include "util.hpp"


void Solver::solve(const QDate & date,
                   const QDate & history)
{
    Util::write("Solving " + date.toString("yyyy-MM-dd")
                + " with history " + history.toString("yyyy-MM-dd"));
    // Init
    bool ok = false;
    QString problems;
    // Prepare problems
    if(ok)
    {
        problems = getProblems(ok);
    }
    //
    if(ok)
    {

    }
}


QString Solver::getProblems(bool & ok)
{
    //
    QString problems;
    //
    if(ok)
    {

    }
    return problems;
}
