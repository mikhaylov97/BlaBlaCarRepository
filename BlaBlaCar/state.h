#ifndef STATE_H
#define STATE_H
#pragma once

namespace StateMachineBlaBlaCar
{
    template <typename T>
    class State
    {
    private:
        T _value;
        int _id;
        static int _counter;

    public:
        State()
        {
            _id = _counter++;
        }

        State(const T& value) : _value(value)
        {
            _id = _counter++;
        }

        State(const State<T>& state)
        {
            _value = state.get_value();
            _id = state.get_id();
        }

        State(const T& value, int id): _value(value), _id(id)
        {
            if (id >= _counter)
                ++id;
        }

        bool operator==(const State<T> & state){
            return state.get_id() == this->get_id();
        }
        bool operator==(State<T> * state){
            return state->get_id() == this->get_id();
        }
        bool operator!=(const State<T> & state){
            return state.get_id() != this->get_id();
        }
        bool operator!=(State<T>* state){
            return state->get_id() != this->get_id();
        }

        T get_value() const
        {
            return this->_value;
        }

        void set_value(T value)
        {
            this->_value = value;
        }

        int get_id() const
        {
            return this->_id;
        }

        void set_id(int id)
        {
            this->_id = id;
        }

    };

    template <typename T>
    int State<T>::_counter = 0;
}

#endif // STATE_H
