#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#pragma once

#include <iostream>
#include <vector>
#include "state.h"
#include "transition.h"
#include "car.h"

namespace StateMachineBlaBlaCar
{
    class Exception  {
    public:
        Exception() {printError();}
        Exception(const std::string& msg) : _msg(msg) {printError();}
        ~Exception(){}
        std::string getMessage() const {return(_msg);}
        void printError() {
            std::cerr << getMessage() << std::endl;
        }
    private:
        std::string _msg;
    };

    class CarNotFoundException: public Exception {
    private:
        int id;
    public:
        CarNotFoundException(int id): Exception("Car with id [" + std::to_string(id) + "] not found!"){
            this->id = id;
        }
        int getCarId() { return this->id; }
    };

    class CarAlreadyExistsException: public Exception {
    private:
        int id;
        Car<void> * car;
    public:
        CarAlreadyExistsException(int id, Car<void> * car): Exception("Car with id [" + std::to_string(id) + "] already exists!"){
            this->id = id;
            this->car = car;
        }
        int getCarId() { return this->id; }
        Car<void> * getCar() { return this->car; }
    };

    class PassengerNotFoundException: public Exception {
    private:
        std::string login;
    public:
        PassengerNotFoundException(std::string login): Exception("Passenger with login [" + login + "] not found!"){
            this->login = login;
        }
        std::string getPassengerLogin() { return this->login; }
    };

    class PassengerAlreadyExistsException: public Exception
    {
    private:
        std::string login;
        Passenger * passenger;
    public:
        PassengerAlreadyExistsException(std::string login, Passenger * passenger) : Exception("Passenger with login [" + login + "] is already exists.") {
            this->login = login;
            this->passenger = passenger;
        }
        std::string getPassengerLogin() { return this->login; }
        Passenger * getPassenger() { return this->passenger; }
    };

    template <typename T>
    class SubstateNotFoundException: public Exception
    {
    private:
        int id;
        std::string name;
    public:
        SubstateNotFoundException(int id) : Exception("Substate with id [" + std::to_string(id) + "] not found.") {
            this->id = id;
        }
        SubstateNotFoundException(std::string name) : Exception("Substate with name [" + std::to_string(id) + "] not found.") {
            this->name = name;
        }
        int getSubstateId() { return id; }
        std::string getSubstateValue() { return this->name; }
    };

    template <typename T>
    class SubstateAlreadyExistsException: public Exception
    {
    private:
        int id;
        std::string name;
        State<T> * state;
    public:
        SubstateAlreadyExistsException(int id, State<T> * state) : Exception("Substate with id [" + std::to_string(id) + "] is already exists.") {
            this->id = id;
            this->state = state;
        }
        SubstateAlreadyExistsException(std::string name, State<T> * state) : Exception("Substate with name [" + name + "] is already exists.") {
            this->name = name;
            this->state = state;
        }
        int getSubstateId() { return this->id; }
        std::string getStateValue() { return this->name; }
        State<T> * getSubstate() { return this->state; }
    };

    template <typename T>
    class StateNotFoundException: public Exception
    {
    private:
        int id;
        std::string name;
    public:
        StateNotFoundException(int id) : Exception("State with id [" + std::to_string(id) + "] not found.") {
            this->id = id;
        }
        StateNotFoundException(std::string name) : Exception("State with name [" + name + "] not found.") {
            this->name = name;
        }
        int getStateId() { return id; }
        std::string getStateValue() { return this->name; }
    };

    template <typename T>
    class StateAlreadyExistsException: public Exception
    {
    private:
        int id;
        std::string name;
        State<T> * state;
    public:
        StateAlreadyExistsException(int id, State<T> * state) : Exception("State with id [" + std::to_string(id) + "] is already exists.") {
            this->id = id;
            this->state = state;
        }
        StateAlreadyExistsException(std::string name, State<T> * state) : Exception("State with name [" + name + "] is already exists.") {
            this->name = name;
            this->state = state;
        }
        int getStateId() { return this->id; }
        std::string getStateValue() { return this->name; }
        State<T> * getState() { return this->state; }
    };

    template <typename T>
    class TransitionNotFoundException: public Exception
    {
    private:
        int id;
        int first_state_id;
        int second_state_id;
    public:
        TransitionNotFoundException(int id) : Exception("Transition with id [" + std::to_string(id) + "] not found.") {
            this->id = id;
        }
        TransitionNotFoundException(int first_state_id, int second_state_id) : Exception("Transition with initialStateId/finalStateId [" + std::to_string(first_state_id) + "/" + std::to_string(second_state_id) + "] not found.") {
            this->first_state_id = first_state_id;
            this->second_state_id = second_state_id;
        }
        int getTransitionId() { return this->id; }
        int getTransitionInitialStateId() { return this->first_state_id; }
        int getTransitionFinalStateId() { return this->second_state_id; }
    };

    template <typename T>
    class TransitionAlreadyExistsException: public Exception
    {
    private:
        int initial_state_id;
        int final_state_id;
        Transition<T> * transition;
    public:
        TransitionAlreadyExistsException(int initial_state_id, int final_state_id, Transition<T> * transition) : Exception("Transition with initialStateId/finalStateId [" + std::to_string(initial_state_id) + "/" + std::to_string(final_state_id) + "] already exists.") {
            this->initial_state_id = initial_state_id;
            this->final_state_id = final_state_id;
            this->transition = transition;
        }
        int getInitialStateId() { return this->initial_state_id; }
        int getFinalStateId() { return this->final_state_id; }
        Transition<T> * getTransition() { return this->transition; }
    };

    class SerializationException: public Exception {
    private:
       std::string file_name;
    public:
        SerializationException(std::string file_name): Exception("Cannot open/find file [" + file_name + "].") {
            this->file_name = file_name;
        }
        std::string getFileName() { return file_name; }
    };

    template <typename T>
    class IteratorOutOfBoundsException: public Exception
    {
    private:
        State<T> * iterator;

    public:
        IteratorOutOfBoundsException(State<T> * iterator) : Exception("Iterator out of bound exception occured.") {
            this->iterator = iterator;
        }
        State<T> * getIterator() { return this->iterator; }
    };
}

#endif // EXCEPTIONS_H
