#include "solver.hpp"
#include "database-manager.hpp"
#include "training-set-creator.hpp"
#include "util.hpp"
#include "intelligence/brain.hpp"
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>

#include <QDebug>


void Solver::solve(const QDate & date,
                   const QString & type,
                   const QString & brainId)
{
    Util::write("Solving " + date.toString("yyyy-MM-dd")
                + " with type " + type);
    bool ok = true;
    QJsonArray problems;
    QString brainFileName = Util::getLineFromConf("pathToBrains", &ok)
            + "/" + brainId;
    Brain brain;
    QString brainJson;
    QFile brainFile(brainFileName);
    //
    if(ok && !brainFile.open(QFile::ReadOnly))
    {
        ok = false;
        Util::writeError("can't find brain at "
                         + QFileInfo(brainFileName).absoluteFilePath());
    }
    // Prepare brain
    if(ok)
    {
        brainJson = brainFile.readAll();
        brain = Brain(brainJson);
    }
    // Prepare problems
    if(ok)
    {
        problems = getProblems(date, type, ok);
    }
    //
    if(ok)
    {
        Util::write("Inserting predictions");
        for(int i = 0 ; i < problems.size() ; i++)
        {
            QJsonObject problem = problems[i].toObject();
            QJsonObject prediction = brain.getPrediction(problem["inputs"].toString(), ";");
            prediction["id"] = problem["id"];
            prediction["date"] = date.toString("yyyy-MM-dd");
            DatabaseManager::insertPrediction(QJsonDocument(prediction),
                                              problem["id"].toString());
        }
    }
}


QJsonArray Solver::getProblems(const QDate & date,
                               const QString & type,
                               bool & ok)
{
    Util::write("Getting problems for " + date.toString("yyyy-MM-dd"));
    QJsonArray problems;
    QStringList ids;
    // Get races ids
    if(ok)
    {
        ids = DatabaseManager::getIdRaces(date);
        if(!ids.size())
        {
            Util::writeError(QString::number(ids.size())
                             + " races found for " + date.toString("yyyy-MM-dd"));
            ok = false;
        }
    }
    // Get problem for each race
    if(ok)
    {
        foreach (QString id, ids)
        {
            QJsonObject problem = getProblem(id, ok);
            if(ok)
            {
                problems << problem;
            }
            else
            {
                ok = true;
            }
        }
    }
    // Send problems
    return problems;
}


QJsonObject Solver::getProblem(const QString & id,
                               bool & ok)
{
    Util::overwrite(id);
    // Init
    QJsonObject json;
    QString inputs;
    //
    if(ok)
    {
        inputs = DatabaseManager::getInputs(id, "default", ok);
    }
    // Prepare object
    if(ok)
    {
        json["inputs"] = inputs;
        json["id"] = id;
    }
    return json;
}
