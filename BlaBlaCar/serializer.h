#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "exceptions.h"
#include "state.h"
#include "transition.h"
#include <cmath>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include <QDir>

namespace StateMachineBlaBlaCar
{
template <class T>
class Serializer
{
private:
    Serializer() {}
    QString path_to_file = "../BlaBlaCar/graph.json";
    static Serializer * m_instance;

public:
    static Serializer & get_instance() {
        if (!m_instance) {
          m_instance = new Serializer();
        }
        return * m_instance;
    }
    void export_to_json(T * stateMachine);
    void import_from_json(T * stateMachine);
    Serializer(const Serializer&) = delete;
    Serializer& operator=(const Serializer&) = delete;
    Serializer(Serializer&&) = delete;
    Serializer& operator=(Serializer&&) = delete;

};

//template <class T>
//static Serializer & get_instance() {
//  if (!Serializer::m_instance) {
//    Serializer::m_instance = new Serializer();
//  }
//  return * Serializer::m_instance;
//}

template <class T>
void StateMachineBlaBlaCar::Serializer<T>::export_to_json(T * stateMachine){
    QFile file(path_to_file);
    if (file.exists())
        file.remove();
    if (!file.open(QIODevice::WriteOnly)) {
        throw SerializationException(path_to_file.toStdString());
    }

    std::ostringstream ostr;
    ostr << *stateMachine;

    // save diagram to file
    QTextStream out(&file);
    out << QString::fromStdString(((std::ostringstream&)ostr).str());
    file.close();

//    delete stateMachine;
}

template <class T>
void StateMachineBlaBlaCar::Serializer<T>::import_from_json(T * stateMachine){
    QFile file(path_to_file);
    if (!file.open(QIODevice::ReadOnly)) {
        throw SerializationException(path_to_file.toStdString());
    }

    // read diagram from file
    QTextStream in(&file);
    std::istringstream istr(in.readAll().toStdString());
    istr >> &stateMachine;
    file.close();
//    delete stateDiagram;
}

template <typename T>
Serializer<T> * Serializer<T>::m_instance = 0;

}
#endif // SERIALIZER_H
