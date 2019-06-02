#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#pragma once

#include <iostream>
#include <vector>
#include "state.h"
#include "transition.h"

namespace StateMachineBlaBlaCar
{
    class Exception  {
    public:
        Exception() {printError();}
        Exception(const std::string& msg) : _msg(msg) {printError();}
        ~Exception(){}
        std::string getMessage( ) const {return(_msg);}
        void printError() {
            std::cout << getMessage( ) << std::endl;
        }
    private:
        std::string _msg;
    };

    class SerializationException: public Exception {
    private:
       std::string file_name;
    public:
        SerializationException(std::string file_name): Exception("Cannot open/find file [" + file_name + "].") {}
    };

    class IncorrectLoginException: public Exception {
    private:
       std::string login;
    public:
        IncorrectLoginException(std::string login): Exception("Entered inccorect user login [" + login + "].") {}
    };

    class StateNotFoundException: public Exception
    {
    private:
        std::string id;

    public:
        StateNotFoundException(std::string id) : Exception("State with id [" + id + "] not found.") {}
    };

    class TransitionNotFoundException: public Exception
    {
    private:
        std::string id;

    public:
        TransitionNotFoundException(std::string id) : Exception("Transition with id [" + id + "] not found.") {}
    };

    template <typename T>
    class IteratorOutOfBoundsException: public Exception
    {
    private:
        State<T> * _iterator;

    public:
        IteratorOutOfBoundsException(State<T> * iterator) : _iterator(iterator) {}
        State<T> * get_iterator() { return this->_iterator; }
    };

    class ParsingException: public Exception {
    private:
        std::string _json_str;

    public:
        ParsingException(std::string json_str) : Exception("Parsing of the string [" + json_str + "] is failed.") {
            this->_json_str = json_str;
        }
        std::string get_json() { return this->_json_str; }
    };

    class StateAlreadyExistsException: public Exception
    {
    private:
        std::string id;

    public:
        StateAlreadyExistsException(std::string id) : Exception("State with id [" + id + "] is already exists.") {}
    };

    class TransitionAlreadyExistsException: public Exception
    {
    private:
        std::string initial_state;
        std::string final_state;

    public:
        TransitionAlreadyExistsException(std::string initial_state_id, std::string final_state_id) :
            Exception ("Transition with initialState/finalState [" + initial_state_id + "/" + final_state_id + "] is already exists.") {}
    };
}

#endif // EXCEPTIONS_H
