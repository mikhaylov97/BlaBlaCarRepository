#ifndef CAR_H
#define CAR_H
#pragma once

#include <vector>
#include "passenger.h"

namespace StateMachineBlaBlaCar
{
    template <typename Dummy>
    class Car
    {
    private:
        int _id;
        int _state;
        std::vector<Passenger> _passengers;
        std::string _from;
        std::string _to;
        static int _counter;

    public:
        Car()
        {
            this->_id = _counter++;
        }

        Car(int state, std::string from, std::string to)
        {
            this->_id = _counter++;
            this->_state = state;
            this->_from = from;
            this->_to = to;
        }

        Car(int id, int state, std::vector<Passenger> passengers, std::string from, std::string to)
        {
            this->_id = id;
            if (id >= _counter) _counter = id + 1;
            this->_state = state;
            this->_passengers = passengers;
            this->_from = from;
            this->_to = to;
        }

        bool operator==(const Car & c){
            return c.get_id() == this->get_id();
        }
        bool operator==(Car * c){
            return c->get_id() == this->get_id();
        }
        bool operator!=(const Car & c){
            return c.get_id() == this->get_id();
        }
        bool operator!=(Car * c){
            return c->get_id() == this->get_id();
        }

        int get_id() const
        {
            return this->_id;
        }

        void set_id(int id)
        {
            this->_id = id;
        }

        int get_state_id() const
        {
            return this->_state;
        }

        void set_state_id(int state_id)
        {
            this->_state = state_id;
        }

        static void set_counter(int counter)
        {
            _counter = counter;
        }

        std::vector<Passenger> * get_passengers()
        {
            return &(this->_passengers);
        }

        std::string get_from()
        {
            return this->_from;
        }

        void set_from(std::string from)
        {
            this->_from = from;
        }

        std::string get_to()
        {
            return this->_to;
        }

        void set_to(std::string to)
        {
            this->_to = to;
        }

    };

    template <typename Dummy>
    int Car<Dummy>::_counter = 0;
}
#endif // CAR_H
