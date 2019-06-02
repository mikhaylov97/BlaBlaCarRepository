#ifndef TRANSITION_H
#define TRANSITION_H
#pragma once

#include <string>
#include <state.h>

namespace StateMachineBlaBlaCar
{
    template <typename T>
    class Transition
    {
    private:
        int _id;
        std::string _name;

        State<T> _initial_state;
        State<T> _final_state;
        bool _one_way;

        static int _counter;

    public:
        Transition()
        {
            _id = _counter++;
        }

        Transition(std::string name, State<T> initial_state, State<T> final_state, bool one_way) : _name(name), _one_way(one_way)
        {
            _initial_state = initial_state;
            _final_state = final_state;
            _id = _counter++;
        }

        Transition(std::string name, State<T> initial_state, State<T> final_state, bool one_way, int id) :
            _name(name), _one_way(one_way), _initial_state(initial_state), _final_state(final_state), _id(id)
        {
            if (id >= _counter)
                ++id;
        }

        bool operator==(const Transition<T> & transition){
            return transition.get_id() == this->get_id();
        }
        bool operator==(Transition<T> * transition){
            return transition->get_id() == this->get_id();
        }
        bool operator!=(const Transition<T> & transition){
            return transition.get_id() != this->get_id();
        }
        bool operator!=(Transition<T>* transition){
            return transition->get_id() != this->get_id();
        }

        int get_id() const
        {
            return _id;
        }

        void set_id(int id)
        {
            this->_id = id;
        }

        State<T> get_initial_state()
        {
            return _initial_state;
        }

        void set_initial_state(State<T> initial_state)
        {
            this->_initial_state = initial_state;
        }

        State<T> get_final_state()
        {
            return _final_state;
        }

        void set_final_state(State<T> final_state)
        {
            this->_final_state = final_state;
        }

        std::string get_name() const
        {
            return _name;
        }

        void set_name(std::string name)
        {
            this->_name = name;
        }

        bool is_one_way() const
        {
            return _one_way;
        }

        void set_one_way(bool one_way)
        {
            this->_one_way = one_way;
        }
    };

    template <typename T>
    int Transition<T>::_counter = 0;
}

#endif // TRANSITION_H
