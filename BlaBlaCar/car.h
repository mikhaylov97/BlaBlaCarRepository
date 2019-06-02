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
        static int _counter;

    public:
        Car()
        {
            this->_id = _counter++;
        }

        Car(int id, int state, std::vector<Passenger> passengers)
        {
            this->_id = id;
            this->_state = state;
            this->_passengers = passengers;
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

        std::vector<Passenger> * get_passengers()
        {
            return &(this->_passengers);
        }

    };

    template <typename Dummy>
    int Car<Dummy>::_counter = 0;
}
#endif // CAR_H
