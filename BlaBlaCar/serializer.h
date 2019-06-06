#ifndef SERIALIZER_H
#define SERIALIZER_H
#pragma once

//#include "exceptions.h"
//#include "state.h"
//#include "transition.h"
#include "allocator.h"
#include "manipulator.h"
#include "statemachine.h"
//#include <cmath>
//#include <sstream>
//#include <iostream>
//#include <fstream>
#include <string>

//#include <QJsonObject>
//#include <QJsonArray>
//#include <QJsonDocument>
//#include <QJsonValue>
//#include <QFile>
//#include <QDebug>
//#include <QDir>

namespace StateMachineBlaBlaCar
{
template <typename T, typename Alloc = Allocator<T>>
class Serializer
{
private:
    Serializer() {
        manipulator = new Manipulator<T, Alloc>();
    }

    Manipulator<T, Alloc> * manipulator;
    static Serializer * m_instance;

public:
    static Serializer & get_instance() {
        if (!m_instance) {
          m_instance = new Serializer();
        }
        return * m_instance;
    }
    void export_to_json(StateMachine<T, Alloc> * stateMachine);
    void import_from_json(StateMachine<T, Alloc> * stateMachine);
    Serializer(const Serializer&) = delete;
    Serializer& operator=(const Serializer&) = delete;
    Serializer(Serializer&&) = delete;
    Serializer& operator=(Serializer&&) = delete;

};

template <typename T, typename Alloc>
void StateMachineBlaBlaCar::Serializer<T, Alloc>::export_to_json(StateMachine<T, Alloc> * stateMachine){
    manipulator->write(stateMachine);
}

template <typename T, typename Alloc>
void StateMachineBlaBlaCar::Serializer<T, Alloc>::import_from_json(StateMachine<T, Alloc> * stateMachine){
    manipulator->read(stateMachine);
}

template <typename T, typename Alloc>
Serializer<T, Alloc> * Serializer<T, Alloc>::m_instance = 0;

}
#endif // SERIALIZER_H
