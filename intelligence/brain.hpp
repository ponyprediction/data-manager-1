#pragma once


#include "neuron.hpp"
#include "neuron-blueprint.hpp"
#include <QJsonObject>
#include <QThread>
#include <QVector>
#include <QThread>


class Brain
{

    friend class Job;

public:
    Brain();
    Brain(const QString & json);
    ~Brain();
    const float & getRatio(){return ratio;}
    const int & getId(){return id;}
    QString getJson();

    QJsonObject getPrediction(const QVector<float> & inputs);
    QJsonObject getPrediction(const QString & inputsStr, const QString & separator);

private:

    void load(const QString & json);
    void compute(const QVector<float> & inputs);
    void preparePrediction();
    void initNeurons();

    int neuronCount;
    int inputCount;
    int weightCount;
    int outputCount;
    int jobId;

    QVector<Neuron> neurons;
    QVector<NeuronBlueprint> neuronBlueprints;

    QVector<float> inputs;
    QVector<float> weights;
    QVector<float> outputs;

    bool go;
    int currentProblemId;
    int id;

    int result;

    float attempts;
    float score;
    float ratio;

    QJsonObject json;

};
