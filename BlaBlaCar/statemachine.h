#ifndef STATEMACHINE_H
#define STATEMACHINE_H
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "state.h"
#include "transition.h"
#include "iterator.h"
#include "exceptions.h"
#include "serializer.h"
#include "allocator.h"
#include "car.h"

namespace StateMachineBlaBlaCar
{
    // fix circular dependency between Iterator.h and StateDiagram.h
    template <typename T, typename Alloc>
    class Iterator;


    template <typename T, typename Alloc = Allocator<T>>
    class StateMachine
    {
    private:
        int _id;
        static int _counter;
        std::vector<State<T>, Alloc> states;
        std::vector<Transition<T>, Alloc> transitions;
        std::vector<Car<void>, Alloc> cars;
        std::vector<std::string, Alloc> passengers;

        template <typename T1, typename Alloc1>
        friend class Iterator;

#pragma region ConditionHelpers

        bool NoIllegalTransitions()
        {
            for (auto i = transitions.begin(); i != transitions.end(); ++i)
            {
                int initial_state_id = (*i).get_initial_state().get_id();
                int final_state_id = (*i).get_final_state().get_id();
                auto initial_state = std::find_if(
                            states.begin(),
                            states.end(),
                            [&initial_state_id](const State<T> & state) { return state.get_id() == initial_state_id; }
                );
                auto final_state = std::find_if(
                            states.begin(),
                            states.end(),
                            [&final_state_id](const State<T> & state) { return state.get_id() == final_state_id; }
                );
                if (initial_state == states.end() || final_state == states.end())
                {
                    return false;
                }
            }

            return true;
        }

        bool StateExists(int id)
        {
            auto state = std::find_if(
                        states.begin(),
                        states.end(),
                        [&id](const State<T> & state) { return state.get_id() == id; }
            );

            if (state == states.end())
                return false;
            else
                return true;
        }

        bool TransitionExists(int id)
        {
            auto transition = std::find_if(
                        transitions.begin(),
                        transitions.end(),
                        [&id](const Transition<T> & transition) { return transition.get_id() == id; }
            );

            if (transition == transitions.end())
                return false;
            else
                return true;
        }

        bool TransitionExists(int initial_state_id, int final_state_id)
        {
            auto transition = std::find_if(
                        transitions.begin(),
                        transitions.end(),
                        [&initial_state_id,&final_state_id](const Transition<T> & transition) { return
                    (const_cast<Transition<T> &>(transition)).get_initial_state().get_id() == initial_state_id &&
                    (const_cast<Transition<T> &>(transition)).get_final_state().get_id() == final_state_id; }
            );

            if (transition == transitions.end())
                return false;
            else
                return true;
        }

#pragma endregion

#pragma region PreConditions

        inline void AssertStateExistsExc(int id) throw(StateNotFoundException)
        {
            if (!StateExists(id))
                throw StateNotFoundException(std::to_string(id));
        }

        inline void AssertStateNotExistsExc(int id) throw(StateAlreadyExistsException)
        {
            if (StateExists(id))
                throw StateAlreadyExistsException(std::to_string(id));
        }

        inline void AssertTransitionExistsExc(int id) throw(TransitionNotFoundException)
        {
            if (!TransitionExists(id))
                throw TransitionNotFoundException(std::to_string(id));
        }

        inline void AssertTransitionExistsExc(int first_state_id, int second_state_id) throw(TransitionNotFoundException)
        {
            if (!TransitionExists(first_state_id, second_state_id))
                throw TransitionNotFoundException("0");
        }

        inline void AssertTransitionNotExistsExc(int first_state_id, int second_state_id) throw(TransitionAlreadyExistsException)
        {
            if (TransitionExists(first_state_id, second_state_id))
                throw TransitionAlreadyExistsException(std::to_string(first_state_id), std::to_string(second_state_id));
        }

#pragma endregion

#pragma region PostConditions

        inline void AssertIllegalTransitions()
        {
            assert(("Illegal transitions exist", NoIllegalTransitions()));
        }

        inline void AssertStateExists(int id)
        {
            assert(("State (id: " + std::to_string(id) + ") doesn't exist", StateExists(id)));
        }

        inline void AssertStateNotExists(int id)
        {
            assert(("State (id: " + std::to_string(id) + ") still exists", !StateExists(id)));
        }

        inline void AssertTransitionExists(int id)
        {
            assert(("Transition (id: " + std::to_string(id) + ") doesn't exist", TransitionExists(id)));
        }

        inline void AssertTransitionNotExists(int id)
        {
            assert(("Transition (id: " + std::to_string(id) + ") still exists", !TransitionExists(id)));
        }

        inline void AssertTransitionNotExists(int first_state_id, int second_state_id)
        {
            assert(("Transition with first_state_id [" + std::to_string(first_state_id) + "] and secon_ state_id [" + std::to_string(second_state_id) + "] still exists", !TransitionExists(first_state_id, second_state_id)));
        }

#pragma endregion

    public:
        StateMachine()
        {
            _id = _counter++;
        }
        ~StateMachine() {}

        void set_id(int id)
        {
            this->_id = id;
            if (_counter <= id) _counter = ++id;
        }

        void add_transition_with_id(int first_state_id, int second_state_id, std::string name, bool one_way, int id) throw(StateNotFoundException)
        {
            auto initial_state = std::find_if(states.begin(), states.end(),
                [&first_state_id](const State<T> & state) { return state.get_id() == first_state_id; });
            auto final_state = std::find_if(states.begin(), states.end(),
                [&second_state_id](const State<T>& state) { return state.get_id() == second_state_id; });
            if (initial_state == states.end() || final_state == states.end())
            {
                throw StateNotFoundException(_id);
            }
            Transition<T> * transition = new Transition<T>(name, *initial_state, *final_state, one_way, id);
            transitions.push_back(*transition);
        }

        State<T> & find_state_by_name(std::string state_name)
        {
            auto state = std::find_if(
                        states.begin(),
                        states.end(),
                        [&state_name](const State<T> & state) { return state.get_value().compare(state_name) == 0; }
            );
            return *state;
        }

        void add_state(State<T> state) throw(StateAlreadyExistsException)
        {
            AssertStateNotExistsExc(state.get_id());

            states.push_back(state);

            AssertStateExists(state.get_id());
        }        

        void delete_state(std::string state_name)
        {
            auto state = std::find_if(
                        states.begin(),
                        states.end(),
                        [&state_name](const State<T> & state) { return state.get_value().compare(state_name) == 0; }
            );
            if (state != states.end())
            {
                delete_state((*state).get_id());
            }
        }

        void delete_state(int state_id)
        {
            AssertStateExistsExc(state_id);

            states.erase(
                std::remove_if(
                    states.begin(),
                    states.end(),
                    [&state_id](State<T> state) { return state.get_id() == state_id; }
                ),
                states.end()
            );

            transitions.erase(
                std::remove_if(
                    transitions.begin(),
                    transitions.end(),
                    [&state_id](Transition<T> transition) { return transition.get_initial_state().get_id() == state_id || transition.get_final_state().get_id() == state_id; }
                ),
                transitions.end()
            );

            AssertStateNotExists(state_id);
            AssertIllegalTransitions();
        }

        void add_passenger_login(std::string login)
        {
            passengers.push_back(login);
        }

        void add_car(Car<void> car)
        {
            cars.push_back(car);
        }

        void add_transition(Transition<T> transition) throw(StateAlreadyExistsException)
        {
            AssertTransitionNotExistsExc(transition.get_id());

            transitions.push_back(transition);

            AssertTransitionExists(transition.get_id());
        }

        void add_transition(int first_state_id, int second_state_id, std::string name, bool one_way)
                    throw(StateNotFoundException, TransitionAlreadyExistsException)
        {
            AssertTransitionNotExistsExc(first_state_id, second_state_id);

            auto initial_state = std::find_if(
                        states.begin(),
                        states.end(),
                        [&first_state_id](const State<T> & state) { return state.get_id() == first_state_id; }
            );
            auto final_state = std::find_if(
                        states.begin(),
                        states.end(),
                        [&second_state_id](const State<T> & state) { return state.get_id() == second_state_id; }
            );
            if (initial_state == states.end() || final_state == states.end())
            {
                throw StateNotFoundException("0");
            }
            Transition<T> * transition = new Transition<T>(name, *initial_state, *final_state, one_way);
            transitions.push_back(*transition);

            AssertTransitionExists(transition->get_id());
        }

        void delete_transition(int initial_state_id, int final_state_id)
        {
            AssertTransitionExistsExc(initial_state_id, final_state_id);

            auto transition = std::find_if(
                        transitions.begin(),
                        transitions.end(),
                        [&initial_state_id, &final_state_id](Transition<T> & transition) { return transition.get_initial_state().get_id() == initial_state_id && transition.get_final_state().get_id() == final_state_id; }
            );
            if (transition == transitions.end())
            {
                throw TransitionNotFoundException(initial_state_id + "/" + final_state_id);
            }

            int transition_id = (*transition).get_id();
            transitions.erase(
                std::remove_if(
                    transitions.begin(),
                    transitions.end(),
                    [&transition_id](const Transition<T> & transition) { return transition.get_id() == transition_id; }
                ),
                transitions.end()
            );

            AssertTransitionNotExists(initial_state_id, final_state_id);
        }

        void delete_transition(int transition_id)
        {
            AssertTransitionExistsExc(transition_id);

            transitions.erase(
                std::remove_if(
                    transitions.begin(),
                    transitions.end(),
                    [&transition_id](const Transition<T> & transition) { return transition.get_id() == transition_id; }
                ),
                transitions.end()
            );

            AssertTransitionNotExists(transition_id);
        }

        int get_id() const
        {
            return _id;
        }

        State<T> * get_states()
        {
            return states.data();
        }

        Transition<T> * get_transitions()
        {
            return transitions.data();
        }

        std::vector<Car<void>> * get_cars_vector()
        {
            return &cars;
        }

        std::vector<std::string> * get_all_logins()
        {
            return &passengers;
        }

        std::vector<State<T>, Alloc> * get_states_vector()
        {
            return &states;
        }

        std::vector<Transition<T>, Alloc> * get_transitions_vector()
        {
            return &transitions;
        }

        int get_states_number()
        {
            return states.size();
        }

        int get_trantitions_number()
        {
            return transitions.size();
        }

        void print()
        {
            std::cout << "\nStates:\n";

            for (auto i = states.begin(); i != states.end(); ++i)
                std::cout << (*i).get_value() << ' ';

            std::cout << "\nTransitions:\n";

            for (auto i = transitions.begin(); i != transitions.end(); ++i)
                std::cout << (*i).get_name() << ' ' << (*i).get_initial_state().get_value() << ' ' << (*i).get_final_state().get_value() << " id:" << (*i).get_id() << std::endl;
        }

        Iterator<T, Alloc> begin()
        {
            int first_id = 0;

            auto first_state = std::min_element(
                        states.begin(),
                        states.end(),
                        [](const State<T> & first, const State<T> & second) {  return first.get_id() < second.get_id(); }
            );

            return Iterator<T, Alloc>(&(*this), &(*first_state));
        }

        Iterator<T, Alloc> end()
        {
            return Iterator<T, Alloc>(&(*this), nullptr);
        }

        std::string to_json()
        {
            QJsonObject json;
            QJsonArray jStates;
            for(auto stateIterator = states.begin(); stateIterator != states.end(); ++stateIterator)
            {
                QJsonObject obj;
                obj["id"] = stateIterator->get_id();
                obj["value"] = QString::fromStdString(stateIterator->get_value());
        //        obj["pos_x"] = std::trunc(iter->get_pos_x() * 100) / 100;
        //        obj["pos_y"] = std::trunc(iter->get_pos_y() * 100) / 100;
                jStates.push_back(obj);
            }

            QJsonArray jTransitions;
            for(auto transitionIterator = transitions.begin(); transitionIterator != transitions.end(); ++transitionIterator)
            {
                QJsonObject obj;
                obj["id"] = transitionIterator->get_id();
                obj["name"] = QString::fromStdString(transitionIterator->get_name());
                obj["initialStateId"] = transitionIterator->get_initial_state().get_id();
                obj["finalStateId"] = transitionIterator->get_final_state().get_id();

        //        obj["pos_x"] = std::trunc(iter->get_pos_x() * 100) / 100;
        //        obj["pos_y"] = std::trunc(iter->get_pos_y() * 100) / 100;
                jTransitions.push_back(obj);
            }
            json["states"] = jStates;
            json["transitions"] = jTransitions;

            QJsonDocument json_doc(json);
            QString json_string = json_doc.toJson().simplified();
            json_string.replace(" ", "");

            return json_string.toStdString();
        }

        void from_json(std::string jsonStr) throw(ParsingException)
        {
            QString qjson_string = QString::fromStdString(jsonStr);
            QJsonDocument json_doc = QJsonDocument::fromJson(qjson_string.toUtf8());
            QJsonObject json = json_doc.object();

            QJsonArray jStates = json["states"].toArray();
            for(auto stateIterator = jStates.begin(); stateIterator != jStates.end(); ++stateIterator)
            {
                QJsonObject obj = stateIterator->toObject();
                State<T> new_state = State<T>(obj["value"].toString().toStdString(), obj["id"].toInt());
                QJsonArray jChildStates = obj["states"].toArray();
                if (jChildStates.size() != 0)
                {
                    for(auto childStateIterator = jChildStates.begin(); childStateIterator != jChildStates.end(); ++childStateIterator)
                    {
                        QJsonObject child_state_obj = childStateIterator->toObject();
                        new_state.add_state(State<T>(child_state_obj["value"].toString().toStdString(), child_state_obj["id"].toInt()));
                    }
                }
                add_state(new_state);
            }

            QJsonArray jTransitions = json["transitions"].toArray();
            for(auto transitionIterator = jTransitions.begin(); transitionIterator != jTransitions.end(); ++transitionIterator)
            {
                QJsonObject obj = transitionIterator->toObject();
                add_transition(obj["initialStateId"].toInt(), obj["finalStateId"].toInt(), obj["name"].toString().toStdString(), false);
            }

            QJsonArray jCars = json["cars"].toArray();
            for(auto carIterator = jCars.begin(); carIterator != jCars.end(); ++carIterator)
            {
                QJsonObject obj = carIterator->toObject();
                int id = obj["id"].toInt();
                int active_state_id = obj["activeStateId"].toInt();
                QJsonArray jCarPassengers = obj["passengers"].toArray();
                std::vector<Passenger> car_passengers;
                for(auto carPassengerIterator = jCarPassengers.begin(); carPassengerIterator != jCarPassengers.end(); ++carPassengerIterator)
                {
                    std::string passenger = carPassengerIterator->toString().toStdString();
                    QJsonArray jPassengers = json["passengers"].toArray();
                    auto passenger_obj = std::find_if(jPassengers.begin(),
                                 jPassengers.end(),
                                 [&passenger](const QJsonValueRef & ref){return ref.toObject()["login"].toString().toStdString().compare(passenger) == 0;}
                    );
                    if (passenger_obj != jPassengers.end())
                    {
                        add_passenger_login(passenger);
                        car_passengers.push_back(Passenger(passenger, (*passenger_obj).toObject()["activeStateId"].toInt()));
                    }
                }
                add_car(Car<void>(id,active_state_id,car_passengers));
            }
        }

        friend std::ostream & operator<<(std::ostream & os, const StateMachine<T, Alloc> & machine)
        {
            os << const_cast<StateMachine<T, Alloc>*>(&machine)->to_json() << std::endl;
            return os;
        }

        friend std::istream & operator>>(std::istream & is, StateMachine<T, Alloc> ** machine) throw(ParsingException)
        {
            std::string jsonStr;
            is >> jsonStr;
//            * machine = StateMachine<T, Alloc>::FromJson(jsonStr);
            machine->from_json(jsonStr);
            return is;
        }

        friend std::ostringstream & operator<<(std::ostringstream & os, const StateMachine<T, Alloc> & machine)
        {
            os << const_cast<StateMachine<T, Alloc>*>(&machine)->to_json() << std::endl;
            return os;
        }

        friend std::istringstream & operator>>(std::istringstream & is, StateMachine<T, Alloc>** machine) throw(ParsingException)
        {
            std::string jsonStr;
            is >> jsonStr;
//            * machine = StateMachine<T, Alloc>::FromJson(jsonStr);
            (*machine)->from_json(jsonStr);
            return is;
        }
    };

    template <typename T, typename Alloc>
    int StateMachine<T, Alloc>::_counter = 0;
}

#endif // STATEMACHINE_H
