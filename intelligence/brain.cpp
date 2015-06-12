#include "brain.hpp"
#include "util.hpp"
#include <QXmlStreamReader>
#include <QStringList>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>


Brain::Brain()
{
}


Brain::Brain(const QString & json) :
    neuronCount(-1),
    inputCount(-1),
    weightCount(-1),
    outputCount(-1),
    jobId(-1),
    neurons(),
    neuronBlueprints(),
    inputs(),
    weights(),
    outputs(),
    go(false),
    currentProblemId(0),
    id(-1),
    attempts(0.0f),
    score(0.0f),
    ratio(0.0f),
    json()
{
    load(json);
    initNeurons();
}


Brain::~Brain()
{

}


void Brain::compute(const QVector<float> & inputs)
{
    for(int i = 0 ; i < this->inputs.size() ; i++)
    {
        this->inputs[i] = 0.0f;
    }
    for(int i = 0 ; i < inputs.size() && i < this->inputs.size() ; i++)
    {
        this->inputs[i] = inputs[i];
    }
    for(int i = 0 ; i < neurons.size() ; i++)
    {
        neurons[i].compute();
    }
    for(int i = 0 ; i < outputCount ; i++)
    {
        int i2 = (neurons.size() - outputs.size()) + i;
        outputs[i] = neurons[i2].getOutput();
    }
}


void Brain::preparePrediction()
{

}



QString Brain::getJson()
{
    // Init
    QString jsonStr = "";
    bool ok = true;
    // Weights
    if(ok)
    {
        QString str = "";
        for(int i = 0 ; i < weights.size() ; i++)
        {
            if(i)
            {
                str += ";";
            }
            str += QString::number(weights[i], 'f', 6);
        }
        json["weights"] = str;
    }
    // Ratio
    if(ok)
    {
        json["ratio"] = ratio;
    }
    //
    if(ok)
    {
        QJsonDocument document;
        document.setObject(json);
        jsonStr = document.toJson();
    }
    //
    return jsonStr;
}


QJsonObject Brain::getPrediction(const QString & inputsStr,
                             const QString & separator)
{
    QStringList list = inputsStr.split(separator);
    QVector<float> inputs;
    foreach (QString input, list)
    {
        inputs.push_back(input.toFloat());
    }
    return getPrediction(inputs);
}


QJsonObject Brain::getPrediction(const QVector<float> & inputs)
{
    compute(inputs);
    bool ok = true;
    // New
    QJsonArray outputs;
    QJsonObject prediction;
    QVector<float> sortedRatios;
    QVector<float> sortedIds;
    QVector<float> ratios = this->outputs;
    int size = ratios.size();
    float lastRatioBest = 1.0f;
    for(int i = 0 ; i < size ; i++)
    {
        float ratioBest = -1.0f;
        int idBest = 0;
        for(int j = 0 ; j < ratios.size() ; j++)
        {
            if(ratios[j] > ratioBest)
            {
                ratioBest = ratios[j];
                idBest = j;
            }
        }
        lastRatioBest = ratioBest;
        sortedRatios << ratios.at(idBest);
        sortedIds << idBest+1;
        ratios[idBest] = -1.0f;
    }
    //
    if(ok)
    {
        for(int i = 0 ; i < sortedIds.size() ; i++)
        {
            QJsonObject team;
            team["id"] = sortedIds[i];
            team["rank"] = i+1;
            team["ratio"] = sortedRatios[i];
            outputs.append(team);
        }
    }
    //
    prediction["outputs"] = outputs;
    //
    return prediction;
}


void Brain::load(const QString & jsonStr)
{
    bool ok = true;
    json = QJsonDocument::fromJson(jsonStr.toUtf8()).object();
    QString weightsStr;
    QStringList weightsStrList;
    // neuronCount
    if(ok)
    {
        neuronCount = json["neuronCount"].toInt();
        neuronBlueprints.clear();
        neurons = QVector<Neuron>(neuronCount, Neuron());
    }
    // inputCount
    if(ok)
    {
        inputCount = json["inputCount"].toInt();
        inputs = QVector<float>(inputCount, 0.0f);
    }
    // weightCount
    if(ok)
    {
        weightCount = json["weightCount"].toInt();
        weights = QVector<float>(weightCount, 0.0f);
        weightsStr = json["weights"].toString();
        weightsStrList = weightsStr.split(';');
        if(weightsStrList.size() != weightCount)
        {
            ok = false;
            Util::writeError("problem loading weights, the count ain't right : "
                             + QString::number(weightsStrList.size())
                             + " vs " + QString::number(weightCount));
        }
    }
    // weights
    if(ok)
    {
        weights = QVector<float>(weightCount, 0.0f);
        for(int i = 0 ; i < weightCount ; i++)
        {
            weights[i] = weightsStrList[i].toFloat();
        }

    }
    // outputCount
    if(ok)
    {
        outputCount = json["outputCount"].toInt();
        outputs = QVector<float>(outputCount, 0.0f);
    }
    // neurons
    if(ok)
    {
        QJsonArray neurarr = json["neurons"].toArray();
        for(int i = 0 ; i < neurarr.size() ; i++)
        {
            neuronBlueprints.push_back(NeuronBlueprint(neurarr[i].toObject()));
        }
    }
    // ratio
    if(ok)
    {
        ratio = json["ratio"].toDouble();
    }
}

void Brain::initNeurons()
{
    //
    neurons.clear();
    for(int i = 0 ; i < neuronCount ; i++)
    {
        neurons.push_back(Neuron());
    }
    //
    for(int i = 0 ; i < neurons.size() ; i++)
    {
        NeuronBlueprint blueprint = neuronBlueprints[i];
        for(int j = 0 ;
            j < blueprint.externalInputIds.size() ;
            j++)
        {
            float * a = &inputs[blueprint.externalInputIds[j]];
            neurons[i].addExternalInput(a);
        }
        for(int j = 0 ;
            j < blueprint.neuronalInputIds.size() ;
            j++)
        {
            float * a =
                    neurons[blueprint.neuronalInputIds[j]].getOutputAdress();
            neurons[i].addNeuronalInput(a);
        }
        for(int j = 0 ;
            j < blueprint.weightIds.size() ;
            j++)
        {
            float * a = &weights[blueprint.weightIds[j]];
            neurons[i].addWeight(a);
        }
    }
}
