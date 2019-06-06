#ifndef MANIPULATOR_H
#define MANIPULATOR_H
#pragma once

#include "allocator.h"
#include "statemachine.h"
#include "exceptions.h"
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

namespace StateMachineBlaBlaCar {

    template <typename T, typename Alloc = Allocator<T>>
    class Manipulator
        {
        private:
            #if (defined (_WIN32) || defined (_WIN64))
            QString path_to_file = "../BlaBlaCar/graph.json";
            #elif (defined (Q_OS_MAC) || defined (Q_OS_OSX) || defined (Q_OS_MACX))
            QString path_to_file = "../../../../BlaBlaCar/graph.json";
            #endif

        public:
            Manipulator() {}

            void read(StateMachine<T, Alloc> * stateMachine);
            void write(StateMachine<T, Alloc> * stateMachine);

            friend std::ostream & operator<<(std::ostream & os, const StateMachine<T, Alloc> & machine);
            friend std::istream & operator>>(std::istream & is, StateMachine<T, Alloc> ** machine) throw(ParsingException);
            friend std::ostringstream & operator<<(std::ostringstream & os, const StateMachine<T, Alloc> & machine);
            friend std::istringstream & operator>>(std::istringstream & is, StateMachine<T, Alloc>** machine) throw(ParsingException);
        };

    template <typename T, typename Alloc>
    void StateMachineBlaBlaCar::Manipulator<T, Alloc>::read(StateMachine<T, Alloc> * stateMachine)
    {
        QFile file(path_to_file);
        if (!file.open(QIODevice::ReadOnly)) {
            throw SerializationException(path_to_file.toStdString());
        }

        // read diagram from file
        QTextStream in(&file);
        std::istringstream istr(in.readAll().toStdString());
        istr >> &stateMachine;
        file.close();
    }

    template <typename T, typename Alloc>
    void StateMachineBlaBlaCar::Manipulator<T, Alloc>::write(StateMachine<T, Alloc> * stateMachine)
    {
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
    }

    template <typename T, typename Alloc>
    std::ostream & operator<<(std::ostream & os, const StateMachine<T, Alloc> & machine)
    {
        os << const_cast<StateMachine<T, Alloc>*>(&machine)->to_json() << std::endl;
        return os;
    }

    template <typename T, typename Alloc>
    std::istream & operator>>(std::istream & is, StateMachine<T, Alloc> ** machine) throw(ParsingException)
    {
        std::string jsonStr;
        is >> jsonStr;
        machine->from_json(jsonStr);
        return is;
    }

    template <typename T, typename Alloc>
    std::ostringstream & operator<<(std::ostringstream & os, const StateMachine<T, Alloc> & machine)
    {
        os << const_cast<StateMachine<T, Alloc>*>(&machine)->to_json() << std::endl;
        return os;
    }

    template <typename T, typename Alloc>
    std::istringstream & operator>>(std::istringstream & is, StateMachine<T, Alloc>** machine) throw(ParsingException)
    {
        std::string jsonStr;
        is >> jsonStr;
        (*machine)->from_json(jsonStr);
        return is;
    }
}

#endif // MANIPULATOR_H
