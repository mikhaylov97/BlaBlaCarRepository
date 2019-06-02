#ifndef PASSENGER_H
#define PASSENGER_H
#pragma once

#include <string>

namespace StateMachineBlaBlaCar
{
    class Passenger
    {
    private:
        std::string _login;
        int _state;

    public:
        Passenger(){}

        Passenger(std::string login, int state)
        {
            this->_login = login;
            this->_state = state;
        }

        bool operator==(const Passenger & p){
            return p.get_login().compare(this->get_login()) == 0;
        }
        bool operator==(Passenger * p){
            return p->get_login().compare(this->get_login()) == 0;
        }
        bool operator!=(const Passenger & p){
            return p.get_login().compare(this->get_login()) != 0;
        }
        bool operator!=(Passenger * p){
            return p->get_login().compare(this->get_login()) != 0;
        }

        std::string get_login() const
        {
            return this->_login;
        }

        void set_login(std::string login)
        {
            this->_login = login;
        }

        int get_state_id() const
        {
            return this->_state;
        }

        void set_state_id(int state_id)
        {
            this->_state = state_id;
        }

    };
}
#endif // PASSENGER_H
