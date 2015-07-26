#include "neuron-blueprint.hpp"
#include <QDebug>

NeuronBlueprint::NeuronBlueprint()
{

}

NeuronBlueprint::NeuronBlueprint(const QJsonObject & json)
{
    externalInputIds.clear();
    neuronalInputIds.clear();
    brainalInputIds.clear();
    weightIds.clear();
    // externalInputIds
    {
        QString str = json["externalInputIds"].toString();
        if(str.size())
        {
            QStringList strList = str.split(';');
            for(int i = 0 ; i < strList.size() ; i++)
                addExternalInputId(strList[i].toInt());
        }
    }
    // neuronalInputIds
    {
        QString str = json["neuronalInputIds"].toString();
        if(str.size())
        {
            QStringList strList = str.split(';');
            for(int i = 0 ; i < strList.size() ; i++)
                addNeuronalInputId(strList[i].toInt());
        }
    }
    // brainalInputIds
    {
        QString str = json["brainalInputIds"].toString();
        if(str.size())
        {
            QStringList strList = str.split(';');
            for(int i = 0 ; i < strList.size() ; i++)
                addBrainalInputId(strList[i].toInt());
        }
    }
    // weightIds
    {
        QString str = json["weightIds"].toString();
        if(str.size())
        {
            QStringList strList = str.split(';');
            for(int i = 0 ; i < strList.size() ; i++)
                addWeightId(strList[i].toInt());
        }
    }
}

NeuronBlueprint::~NeuronBlueprint()
{

}

