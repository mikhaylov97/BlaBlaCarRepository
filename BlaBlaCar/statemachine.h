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
    template <typename T, typename Alloc>
    class Iterator;


    template <typename T, typename Alloc = Allocator<T>>
    class StateMachine
    {
    private:
        int _id;
        static int _counter;
        std::vector<State<T>, Allocator<State<T>>> states;
        std::vector<Transition<T>, Allocator<Transition<T>>> transitions;
        std::vector<Car<void>> cars;
        std::vector<std::string> passengers;

        template <typename T1, typename Alloc1>
        friend class Iterator;

#pragma region ConditionHelpers

        bool NoIllegalTransitionsForStates()
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

        bool NoIllegalTransitionsForSubstates()
        {
            for (auto i = transitions.begin(); i != transitions.end(); ++i)
            {
                for (auto sub = states.begin(); sub != states.end(); ++sub)
                {
                    if (sub->get_states()->size() > 1)
                    {
                        int initial_state_id = (*i).get_initial_state().get_id();
                        int final_state_id = (*i).get_final_state().get_id();
                        auto initial_state = std::find_if(
                                    sub->get_states()->begin(),
                                    sub->get_states()->end(),
                                    [&initial_state_id](const State<T> & state) { return state.get_id() == initial_state_id; }
                        );
                        auto final_state = std::find_if(
                                    sub->get_states()->begin(),
                                    sub->get_states()->end(),
                                    [&final_state_id](const State<T> & state) { return state.get_id() == final_state_id; }
                        );
                        if (initial_state == sub->get_states()->end() || final_state == sub->get_states()->end())
                        {
                            return false;
                        }
                    }
                }

            }

            return true;
        }

        bool SubstateExists(int id)
        {
            for (auto stateIterator = states.begin(); stateIterator != states.end(); ++stateIterator)
            {
                if (stateIterator->get_states()->size() > 0)
                {
                    for (auto substateIterator = stateIterator->get_states()->begin(); substateIterator != stateIterator->get_states()->end(); ++substateIterator)
                    {
                        if (substateIterator->get_id() == id) return true;
                    }
                }
            }

            return false;
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

        bool StateExists(std::string name)
        {
            auto state = std::find_if(
                        states.begin(),
                        states.end(),
                        [&name](const State<T> & state) { return state.get_value().compare(name) == 0; }
            );

            if (state == states.end())
                return false;
            else
                return true;
        }

        bool SubstateExists(std::string name)
        {
            for (auto stateIterator = states.begin(); stateIterator != states.end(); ++stateIterator)
            {
                if (stateIterator->get_states()->size() > 0)
                {
                    for (auto substateIterator = stateIterator->get_states()->begin(); substateIterator != stateIterator->get_states()->end(); ++substateIterator)
                    {
                        if (substateIterator->get_value().compare(name) == 0) return true;
                    }
                }
            }

            return false;
        }

        bool CarExists(int id)
        {
            auto car = std::find_if(
                        cars.begin(),
                        cars.end(),
                        [&id](const Car<void> & car) { return car.get_id() == id; }
            );

            if (car == cars.end())
                return false;
            else
                return true;
        }

        bool PassengerExists(std::string login)
        {
            for (auto carIterator = cars.begin(); carIterator != cars.end(); ++carIterator)
            {
                auto passenger = std::find_if(
                            carIterator->get_passengers()->begin(),
                            carIterator->get_passengers()->end(),
                            [&login](const Passenger & passenger) { return passenger.get_login().compare(login) == 0; }
                );

                if (passenger != carIterator->get_passengers()->end())
                {
                    return true;
                }
            }

            return false;
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

        inline void AssertStateExistsPreCondition(int id) throw(StateNotFoundException<T>)
        {
            if (!StateExists(id))
                throw StateNotFoundException<T>(id);
        }

        inline void AssertStateExistsPreCondition(std::string name) throw(StateNotFoundException<T>)
        {
            if (!StateExists(name))
                throw StateNotFoundException<T>(name);
        }

        inline void AssertStateNotExistsPreCondition(std::string name) throw(StateAlreadyExistsException<T>)
        {
            if (StateExists(name))
            {
                throw StateAlreadyExistsException<T>(name, find_state_by_name(name));
            }
        }

        inline void AssertStateNotExistsPreCondition(int id) throw(StateAlreadyExistsException<T>)
        {
            if (StateExists(id))
            {
                State<T> state = find_state_by_id(id);
                throw StateAlreadyExistsException<T>(id, &state);
            }
        }

        inline void AssertSubstateExistsPreCondition(int id) throw(StateNotFoundException<T>)
        {
            if (!SubstateExists(id))
                throw SubstateNotFoundException<T>(id);
        }

        inline void AssertSubstateExistsPreCondition(std::string name) throw(StateNotFoundException<T>)
        {
            if (!SubstateExists(name))
                throw SubstateNotFoundException<T>(name);
        }

        inline void AssertSubstateNotExistsPreCondition(int superstate_id, int id) throw(StateNotFoundException<T>)
        {
            if (SubstateExists(id))
            {
                State<T> substate = find_substate_by_id(superstate_id, id);
                throw SubstateAlreadyExistsException<T>(id, &substate);
            }
        }

        inline void AssertSubstateNotExistsPreCondition(std::string name) throw(StateNotFoundException<T>)
        {
            if (SubstateExists(name))
                throw SubstateAlreadyExistsException<T>(name, find_substate_by_id(name));
        }

        inline void AssertTransitionExistsPreCondition(int id) throw(TransitionNotFoundException<T>)
        {
            if (!TransitionExists(id))
                throw TransitionNotFoundException<T>(id);
        }

        inline void AssertTransitionExistsPreCondition(int initial_state_id, int final_state_id) throw(TransitionNotFoundException<T>)
        {
            if (!TransitionExists(initial_state_id, final_state_id))
                throw TransitionNotFoundException<T>(initial_state_id, final_state_id);
        }

        inline void AssertTransitionNotExistsPreCondition(int id) throw(TransitionAlreadyExistsException<T>)
        {
            if (TransitionExists(id))
                throw TransitionAlreadyExistsException<T>(id, find_transition(id));
        }

        inline void AssertTransitionNotExistsPreCondition(int first_state_id, int second_state_id) throw(TransitionAlreadyExistsException<T>)
        {
            if (TransitionExists(first_state_id, second_state_id))
            {
                Transition<T> transition = find_transition(first_state_id, second_state_id);
                throw TransitionAlreadyExistsException<T>(first_state_id, second_state_id, &transition);
            }
        }

        inline void AssertCarExistsPreCondition(int id) throw(CarNotFoundException)
        {
            if (!CarExists(id))
                throw CarNotFoundException(id);
        }

        inline void AssertCarNotExistsPreCondition(int id) throw(CarAlreadyExistsException)
        {
            if (CarExists(id)) {
                Car<void> car = find_car_by_id(id);
                throw CarAlreadyExistsException(id, &car);
            }
        }

        inline void AssertPassengerExistsPreCondition(std::string login) throw(PassengerNotFoundException)
        {
            if (!PassengerExists(login))
                throw PassengerNotFoundException(login);
        }

        inline void AssertPassengerNotExistsPreCondition(std::string login) throw(PassengerAlreadyExistsException)
        {
            if (PassengerExists(login))
                throw PassengerAlreadyExistsException(login, find_passenger_by_login(login));
        }


#pragma endregion

#pragma region PostConditions

        inline void AssertIllegalTransitionsPostCondition()
        {
            assert(("Illegal transitions for superstates exist.", NoIllegalTransitionsForStates() && NoIllegalTransitionsForSubstates()));
        }

        inline void AssertStateExistsPostCondition(int id)
        {
            assert(("Superstate with id [" + std::to_string(id) + "] doesn't exist.", StateExists(id)));
        }

        inline void AssertStateExistsPostCondition(std::string name)
        {
            assert(("Superstate with name [" + name + "] doesn't exist.", StateExists(name)));
        }

        inline void AssertStateNotExistsPostCondition(int id)
        {
            assert(("Superstate with id [" + std::to_string(id) + "] still exists", !StateExists(id)));
        }

        inline void AssertStateNotExistsPostCondition(std::string name)
        {
            assert(("Superstate with name [" + name + "] still exists", !StateExists(name)));
        }

        inline void AssertSubstateExistsPostCondition(int id)
        {
            assert(("Substate with id [" + std::to_string(id) + "] doesn't exist.", SubstateExists(id)));
        }

        inline void AssertSubstateExistsPostCondition(std::string name)
        {
            assert(("Substate with name [" + name + "] doesn't exist.", SubstateExists(name)));
        }

        inline void AssertSubstateNotExistsPostCondition(int id)
        {
            assert(("Substate with id [" + std::to_string(id) + "] still exists", !SubstateExists(id)));
        }

        inline void AssertSubstateNotExistsPostCondition(std::string name)
        {
            assert(("Substate with name [" + name + "] still exists", !SubstateExists(name)));
        }

        inline void AssertTransitionExistsPostCondition(int id)
        {
            assert(("Transition with id [" + std::to_string(id) + "] doesn't exist", TransitionExists(id)));
        }

        inline void AssertTransitionExistsPostCondition(int initial_state_id, int final_state_id)
        {
            assert(("Transition with initialStateId/finalStateId [" + std::to_string(initial_state_id) + "/" + std::to_string(final_state_id) + "] doesn't exist", TransitionExists(initial_state_id, final_state_id)));
        }

        inline void AssertTransitionNotExistsPostCondition(int id)
        {
            assert(("Transition with id [" + std::to_string(id) + "] still exists", !TransitionExists(id)));
        }

        inline void AssertTransitionNotExistsPostCondition(int first_state_id, int second_state_id)
        {
            assert(("Transition with initialStateId/finalStateId [" + std::to_string(first_state_id) + "/" + std::to_string(second_state_id) + "] still exists", !TransitionExists(first_state_id, second_state_id)));
        }

        inline void AssertCarExistsPostCondition(int car_id)
        {
            assert(("Car with id [" + std::to_string(car_id) + "] doesn't exists", CarExists(car_id)));
        }

        inline void AssertCarNotExistsPostCondition(int car_id)
        {
            assert(("Car with id [" + std::to_string(car_id) + "] still exists", !CarExists(car_id)));
        }

        inline void AssertPassengerExistsPostCondition(std::string login)
        {
            assert(("Passenger with login [" + login + "] doesn't exists", PassengerExists(login)));
        }

        inline void AssertPassengerNotExistsPostCondition(std::string login)
        {
            assert(("Passenger with login [" + login + "] still exists", !PassengerExists(login)));
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

        void add_transition_with_id(int first_state_id, int second_state_id, std::string name, bool one_way, int id) throw(StateNotFoundException<T>)
        {
            AssertTransitionNotExistsPreCondition(first_state_id, second_state_id);

            auto initial_state = std::find_if(states.begin(), states.end(),
                [&first_state_id](const State<T> & state) { return state.get_id() == first_state_id; });
            auto final_state = std::find_if(states.begin(), states.end(),
                [&second_state_id](const State<T>& state) { return state.get_id() == second_state_id; });
            if (initial_state == states.end())
            {
                throw StateNotFoundException<T>(first_state_id);
            }
            if (final_state == states.end())
            {
                throw StateNotFoundException<T>(second_state_id);
            }
            Transition<T> * transition = new Transition<T>(name, *initial_state, *final_state, one_way, id);
            transitions.push_back(*transition);

            AssertTransitionExistsPostCondition(transition->get_id());
        }

        Passenger find_passenger_by_login(std::string login)
        {
            AssertPassengerExistsPreCondition(login);

            for(auto carIterator = cars.begin(); carIterator != cars.end(); ++carIterator)
            {
                for(auto passengerIterator = carIterator->get_passengers()->begin(); passengerIterator != carIterator->get_passengers()->end(); ++passengerIterator)
                {
                    if (passengerIterator->get_login().compare(login) == 0)
                    {
                        return *passengerIterator;
                    }
                }
            }

            throw PassengerNotFoundException(login);
        }

        std::vector<Passenger> * find_free_passengers(int except_car_id)
        {
            for(auto carIterator = cars.begin(); carIterator != cars.end(); ++carIterator)
            {
                if (carIterator->get_id() != except_car_id && carIterator->get_state_id() == 2)
                {
                    //car in planing state. Such passengers are free, because they only commited to trip, but not on the way
                    return carIterator->get_passengers();
                }
            }

            std::vector<Passenger> * passengers = new std::vector<Passenger>();
            return passengers;
        }

        std::vector<Passenger> * find_car_passengers(int car_id)
        {
            AssertCarExistsPreCondition(car_id);

            for(auto carIterator = cars.begin(); carIterator != cars.end(); ++carIterator)
            {
                if (carIterator->get_id() == car_id)
                {
                    return carIterator->get_passengers();
                }
            }

            AssertCarExistsPostCondition(car_id);

            std::vector<Passenger> * passengers = new std::vector<Passenger>();
            return passengers;
        }

        Passenger find_car_passenger(int car_id, std::string login) throw (CarNotFoundException, PassengerNotFoundException)
        {
            AssertPassengerExistsPreCondition(login);

            auto car = std::find_if(
                        cars.begin(),
                        cars.end(),
                        [&car_id](const Car<void> & car) { return car.get_id() == car_id; }
            );

            if (car == cars.end())
            {
                throw CarNotFoundException(car_id);
            }

            auto passenger = std::find_if(
                        car->get_passengers()->begin(),
                        car->get_passengers()->end(),
                        [&login](Passenger p) { return p.get_login().compare(login) == 0; }
            );

            if (passenger == car->get_passengers()->end())
            {
                throw PassengerNotFoundException(login);
            }

            AssertPassengerExistsPostCondition(login);

            return *passenger;
        }

        void remove_car_passengers(int car_id) throw (CarNotFoundException)
        {
            AssertCarExistsPreCondition(car_id);

            auto car = std::find_if(
                        cars.begin(),
                        cars.end(),
                        [&car_id](const Car<void> & car) { return car.get_id() == car_id; }
            );

            if (car == cars.end())
            {
                throw CarNotFoundException(car_id);
            }

            car->get_passengers()->clear();

            AssertCarExistsPostCondition(car_id);
        }

        void change_passenger_active_state(int car_id, std::string login, int state_id) throw (CarNotFoundException, PassengerNotFoundException)
        {
            AssertPassengerExistsPreCondition(login);

            auto car = std::find_if(
                        cars.begin(),
                        cars.end(),
                        [&car_id](const Car<void> & car) { return car.get_id() == car_id; }
            );

            if (car == cars.end())
            {
                throw CarNotFoundException(car_id);
            }

            auto passenger = std::find_if(
                        car->get_passengers()->begin(),
                        car->get_passengers()->end(),
                        [&login](Passenger p) { return p.get_login().compare(login) == 0; }
            );

            if (passenger == car->get_passengers()->end())
            {
                throw PassengerNotFoundException(login);
            }

            passenger->set_state_id(state_id);

            AssertPassengerExistsPostCondition(login);
        }

        State<T> & find_state_by_name(std::string state_name)
        {
            AssertStateExistsPreCondition(state_name);

            auto state = std::find_if(
                        states.begin(),
                        states.end(),
                        [&state_name](const State<T> & state) { return state.get_value().compare(state_name) == 0; }
            );

            AssertStateExistsPostCondition(state_name);

            return *state;
        }

        bool is_substate_is_available_for_removing(int substate_id)
        {
            AssertSubstateExistsPreCondition(substate_id);

            for(auto carIterator = cars.begin(); carIterator != cars.end(); ++carIterator)
            {
                for (auto passengerIterator = carIterator->get_passengers()->begin(); passengerIterator != carIterator->get_passengers()->end(); ++passengerIterator)
                {
                    if (passengerIterator->get_state_id() == substate_id)
                    {
                        return false;
                    }
                }
            }

            AssertSubstateExistsPostCondition(substate_id);

            return true;
        }

        State<T> & find_substate_by_name(std::string name, int superstate_id)
        {
            AssertSubstateExistsPreCondition(name);

            auto state = std::find_if(
                        states.begin(),
                        states.end(),
                        [&superstate_id](const State<T> & state) { return state.get_id() == superstate_id; }
            );

            std::vector<State<T>> * substates = (*state).get_states();
            auto substate = std::find_if(
                        substates->begin(),
                        substates->end(),
                        [&name](const State<T> & state) { return state.get_value().compare(name) == 0; }
            );

            AssertSubstateExistsPostCondition(name);

            return *substate;
        }

        State<T> & find_state_by_id(int state_id)
        {
            AssertStateExistsPreCondition(state_id);

            auto state = std::find_if(
                        states.begin(),
                        states.end(),
                        [&state_id](const State<T> & state) { return state.get_id() == state_id; }
            );

            AssertStateExistsPostCondition(state_id);

            return *state;
        }

        bool is_substate_exists_in_other_superstates(int substate_id)
        {
            for(auto stateIterator = states.begin(); stateIterator != states.end(); ++stateIterator)
            {
                if (stateIterator->get_states()->size() > 0)
                {
                    auto substate = std::find_if(
                                stateIterator->get_states()->begin(),
                                stateIterator->get_states()->end(),
                                [&substate_id](State<T> state) { return state.get_id() == substate_id; }
                    );

                    if (substate != stateIterator->get_states()->end())
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        bool is_substate_exists_by_id(int superstate_id, int substate_id)
        {
            auto state = std::find_if(
                        states.begin(),
                        states.end(),
                        [&superstate_id](const State<T> & state) { return state.get_id() == superstate_id; }
            );
            if (state == states.end())
            {
                throw StateNotFoundException<T>(superstate_id);
            }

            auto substate = std::find_if(
                        state->get_states()->begin(),
                        state->get_states()->end(),
                        [&substate_id](State<T> state) { return state.get_id() == substate_id; }
                    );

            if (substate == state->get_states()->end())
            {
                return false;
            }

            return true;
        }

        State<T> & find_substate_by_id(int superstate_id, int substate_id) throw (StateNotFoundException<T>, SubstateNotFoundException<T>)
        {
            AssertSubstateExistsPreCondition(substate_id);

            auto state = std::find_if(
                        states.begin(),
                        states.end(),
                        [&superstate_id](const State<T> & state) { return state.get_id() == superstate_id; }
            );
            if (state == states.end())
            {
                throw StateNotFoundException<T>(superstate_id);
            }

            auto substate = std::find_if(
                        state->get_states()->begin(),
                        state->get_states()->end(),
                        [&substate_id](State<T> state) { return state.get_id() == substate_id; }
                    );

            if (substate == state->get_states()->end())
            {
                throw SubstateNotFoundException<T>(substate_id);
            }

            AssertSubstateExistsPostCondition(substate_id);

            return *substate;
        }

        Transition<T> find_transition(int first_state_id, int second_state_id) throw(TransitionNotFoundException<T>)
        {
            AssertTransitionExistsPreCondition(first_state_id, second_state_id);

            auto transition = std::find_if(
                        transitions.begin(),
                        transitions.end(),
                        [&first_state_id,&second_state_id](const Transition<T> & transition) { return
                    (const_cast<Transition<T> &>(transition)).get_initial_state().get_id() == first_state_id &&
                    (const_cast<Transition<T> &>(transition)).get_final_state().get_id() == second_state_id; }
            );

            if (transition == transitions.end())
            {
                throw TransitionNotFoundException<T>(first_state_id, second_state_id);
            }

            AssertTransitionExistsPostCondition(first_state_id, second_state_id);

            return *transition;

        }

        std::vector<Transition<T>> find_all_transitions_except_substate_transitions()
        {
            std::vector<Transition<T>> * result_transitions = new std::vector<Transition<T>>();
            for (auto tIterator = transitions.begin(); tIterator != transitions.end(); ++tIterator)
            {
                bool is_substate_transition = false;
                for (auto stateIterator = states.begin(); stateIterator != states.end(); ++stateIterator)
                {
                    if (stateIterator->get_states()->size() > 1)
                    {
                        for (auto substateIterator = stateIterator->get_states()->begin(); substateIterator != stateIterator->get_states()->end(); ++substateIterator)
                        {
                            int substate_id = substateIterator->get_id();
                            if (tIterator->get_initial_state().get_id() == substate_id || tIterator->get_final_state().get_id() == substate_id)
                            {
                                is_substate_transition = true;
                                break;
                            }
                        }
                        if (is_substate_transition) break;
                    }
                }
                if (!is_substate_transition)
                {
                    result_transitions->push_back(*tIterator);
                }
            }

            return *result_transitions;
        }

        std::vector<Transition<T>> find_all_transitions_for_superstate(State<T> superstate)
        {
            AssertStateExistsPreCondition(superstate.get_id());

            std::vector<State<T>> * substates = superstate.get_states();
            std::vector<Transition<T>> * matches = new std::vector<Transition<T>>();
            for(auto substateIterator = substates->begin(); substateIterator != substates->end(); ++substateIterator)
            {
                State<T> cur_substate = *substateIterator;
                auto i = transitions.begin(), end = transitions.end();
                while (i != end)
                {
                    i = std::find_if(i, end, [&cur_substate](Transition<T> & t){return cur_substate.get_id() == t.get_initial_state().get_id();});
                  if (i != end)
                  {
                    matches->push_back((*i));
                    i++;
                  }
                }
            }

            AssertStateExistsPostCondition(superstate.get_id());

            return *matches;
        }

        std::vector<State<T>> get_reachable_states_from_current(State<T> state)
        {
            AssertStateExistsPreCondition(state.get_id());

            std::vector<State<T>> matches;
            auto i = transitions.begin(), end = transitions.end();
            while (i != end)
            {
                i = std::find_if(i, end, [&state](Transition<T> & t){return state.get_id() == t.get_initial_state().get_id();});
              if (i != end)
              {
                matches.push_back((*i).get_final_state());
                i++;
              }
            }

            AssertStateExistsPostCondition(state.get_id());

            return matches;
        }

        bool is_superstate_can_be_deleted(std::string name)
        {
            if (cars.size() == 0)
            {
                return true;
            }

            State<T> state = find_state_by_name(name);
            int state_id = state.get_id();
            auto car = std::find_if(
                        cars.begin(),
                        cars.end(),
                        [&state_id](Car<void> car) { return car.get_state_id() == state_id; }
            );

            if (car != cars.end())
            {
                return false;
            }

            return true;
        }

        void add_state(State<T> state) throw(StateAlreadyExistsException<T>)
        {
            AssertStateNotExistsPreCondition(state.get_id());

            states.push_back(state);

            AssertStateExistsPostCondition(state.get_id());
        }

        void add_substate(State<T> substate, int state_id) throw (StateNotFoundException<T>)
        {
            AssertSubstateNotExistsPreCondition(state_id, substate.get_id());

            auto state = std::find_if(states.begin(),
                         states.end(),
                         [&state_id](const State<T> & state){return state.get_id() == state_id;}
            );
            if (state != states.end())
            {
                (*state).add_state(substate);
            } else {
                throw StateNotFoundException<T>(state_id);
            }

            AssertSubstateExistsPostCondition(substate.get_id());
        }

        void delete_substate(int superstate_id, int substate_id)
        {
            AssertStateExistsPreCondition(superstate_id);

            auto state = std::find_if(
                        states.begin(),
                        states.end(),
                        [&superstate_id](const State<T> & state) { return state.get_id() == superstate_id; }
            );

            std::vector<State<T>> * substates = (*state).get_states();

            substates->erase(
                std::remove_if(
                    substates->begin(),
                    substates->end(),
                    [&substate_id](State<T> state) { return state.get_id() == substate_id; }
                ),
                substates->end()
            );

            transitions.erase(
                std::remove_if(
                    transitions.begin(),
                    transitions.end(),
                    [&substate_id](Transition<T> transition) { return transition.get_initial_state().get_id() == substate_id || transition.get_final_state().get_id() == substate_id; }
                ),
                transitions.end()
            );

            AssertStateExistsPostCondition(superstate_id);
        }

        void delete_state(std::string state_name)
        {
            AssertStateExistsPreCondition(state_name);

            auto state = std::find_if(
                        states.begin(),
                        states.end(),
                        [&state_name](const State<T> & state) { return state.get_value().compare(state_name) == 0; }
            );
            if (state != states.end())
            {
                delete_state((*state).get_id());
            }

            AssertStateNotExistsPostCondition(state_name);
        }

        void delete_state(int state_id)
        {
            AssertStateExistsPreCondition(state_id);

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

            AssertStateNotExistsPostCondition(state_id);
        }

        void add_passenger_to_car(int car_id, std::string login)
        {
            AssertCarExistsPreCondition(car_id);
            AssertPassengerExistsPreCondition(login);

            auto car = std::find_if(
                        cars.begin(),
                        cars.end(),
                        [&car_id](const Car<void> car) { return car.get_id() == car_id; }
            );
            if (car == cars.end())
            {
                throw CarNotFoundException(car_id);
            }

            Passenger * new_passenger;
            for(auto carIterator = cars.begin(); carIterator != cars.end(); ++carIterator)
            {
                auto passenger = std::find_if(
                            carIterator->get_passengers()->begin(),
                            carIterator->get_passengers()->end(),
                            [&login](Passenger p) { return p.get_login().compare(login) == 0; }
                        );
                if (passenger != carIterator->get_passengers()->end())
                {
                    new_passenger = new Passenger(passenger->get_login(), passenger->get_state_id());
                    carIterator->get_passengers()->erase(passenger);
                    break;
                }
            }

            car->get_passengers()->push_back(*new_passenger);

            AssertCarExistsPostCondition(car_id);
            AssertPassengerExistsPostCondition(login);
        }

        void delete_passenger(int car_id, std::string login)
        {
            AssertCarExistsPreCondition(car_id);
            AssertPassengerExistsPreCondition(login);

            auto car = std::find_if(
                        cars.begin(),
                        cars.end(),
                        [&car_id](const Car<void> & car) { return car.get_id() == car_id; }
            );
            if (car == cars.end())
            {
                throw CarNotFoundException(car_id);
            }

            car->get_passengers()->erase(
                std::remove_if(
                    car->get_passengers()->begin(),
                    car->get_passengers()->end(),
                    [&login](Passenger passenger) { return passenger.get_login().compare(login) == 0; }
                ),
                car->get_passengers()->end()
            );

            AssertCarExistsPostCondition(car_id);
            AssertPassengerNotExistsPostCondition(login);
        }

        void add_passenger_login(std::string login)
        {
            passengers.push_back(login);
        }

        void change_car_active_state(int car_id, int active_state_id)
        {
            AssertCarExistsPreCondition(car_id);
            AssertStateExistsPreCondition(active_state_id);

            auto car = std::find_if(
                        cars.begin(),
                        cars.end(),
                        [&car_id](Car<void> car) { return car.get_id() == car_id;}
                    );
            if (car != cars.end())
            {
                car->set_state_id(active_state_id);
            }

            AssertCarExistsPostCondition(car_id);
            AssertStateExistsPostCondition(active_state_id);
        }

        Car<void> find_car_by_id(int car_id)
        {
            AssertCarExistsPreCondition(car_id);

            auto car = std::find_if(
                        cars.begin(),
                        cars.end(),
                        [&car_id](Car<void> car) { return car.get_id() == car_id; }
                    );
            if (car == cars.end())
            {
                throw CarNotFoundException(car_id);
            }

            AssertCarExistsPostCondition(car_id);

            return *car;
        }

        void add_car(Car<void> car)
        {
            AssertCarNotExistsPreCondition(car.get_id());

            cars.push_back(car);

            AssertCarExistsPostCondition(car.get_id());
        }

        void delete_car(int car_id)
        {
            AssertCarExistsPreCondition(car_id);

            cars.erase(
                std::remove_if(
                    cars.begin(),
                    cars.end(),
                    [&car_id](Car<void> car) { return car.get_id() == car_id; }
                ),
                cars.end()
            );

            Car<void>::set_counter(cars.size() + 1);
            int i = 1;
            for (auto carIterator = cars.begin(); carIterator != cars.end(); ++carIterator)
            {
                carIterator->set_id(i++);
            }

            AssertCarNotExistsPostCondition(car_id);
        }

        bool is_login_available(std::string login)
        {
            for (auto carIterator = cars.begin(); carIterator != cars.end(); ++carIterator)
            {
                for (auto passengerIterator = carIterator->get_passengers()->begin(); passengerIterator != carIterator->get_passengers()->end(); ++passengerIterator)
                {
                    if (passengerIterator->get_login().compare(login) == 0)
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        int find_first_substate_in_on_the_way_superstate()
        {
            int on_the_way_id = 3;
            auto on_the_way = std::find_if(
                        states.begin(),
                        states.end(),
                        [&on_the_way_id] (State<T> state) { return state.get_id() == on_the_way_id; }
            );

            return on_the_way->get_states()->front().get_id();
        }

        void add_transition(Transition<T> transition) throw(StateAlreadyExistsException<T>)
        {
            AssertTransitionNotExistsPreCondition(transition.get_id());

            transitions.push_back(transition);

            AssertTransitionExistsPostCondition(transition.get_id());
        }

        void add_transition_for_substates(int superstate_id, int first_state_id, int second_state_id, std::string name, bool one_way)
        {
            AssertTransitionNotExistsPreCondition(first_state_id, second_state_id);

            auto stateIt = std::find_if(
                        states.begin(),
                        states.end(),
                        [&superstate_id](const State<T> & state) { return state.get_id() == superstate_id;}
            );
            State<T> superstate = *stateIt;
            std::vector<State<T>> * substates = superstate.get_states();
            //add assert
            auto initial_state = std::find_if(
                        substates->begin(),
                        substates->end(),
                        [&first_state_id](const State<T> & state) { return state.get_id() == first_state_id; }
            );
            auto final_state = std::find_if(
                        substates->begin(),
                        substates->end(),
                        [&second_state_id](const State<T> & state) { return state.get_id() == second_state_id; }
            );
            if (initial_state == substates->end())
            {
                throw SubstateNotFoundException<T>(initial_state->get_id());
            }
            if (final_state == substates->end())
            {
                throw SubstateNotFoundException<T>(final_state->get_id());
            }
            Transition<T> * transition = new Transition<T>(name, *initial_state, *final_state, one_way);
            transitions.push_back(*transition);

            AssertTransitionExistsPostCondition(first_state_id, second_state_id);
        }

        void add_transition(int first_state_id, int second_state_id, std::string name, bool one_way)
                    throw(StateNotFoundException<T>, TransitionAlreadyExistsException<T>)
        {
            AssertTransitionNotExistsPreCondition(first_state_id, second_state_id);

            bool initialStateFound = false;
            bool finalStateFound = false;
            State<T> initialState;
            State<T> finalState;
            for (auto stateIterator = states.begin(); stateIterator != states.end(); ++stateIterator)
            {
                if (stateIterator->get_id() == first_state_id)
                {
                    initialState = *stateIterator;
                    initialStateFound = true;
                }
                if (stateIterator->get_id() == second_state_id)
                {
                    finalState = *stateIterator;
                    finalStateFound = true;
                }
                if (stateIterator->get_states()->size() > 0)
                {
                    for (auto substateIterator = stateIterator->get_states()->begin(); substateIterator != stateIterator->get_states()->end(); ++substateIterator)
                    {
                        if (substateIterator->get_id() == first_state_id)
                        {
                            initialState = *substateIterator;
                            initialStateFound = true;
                        }
                        if (substateIterator->get_id() == second_state_id)
                        {
                            finalState = *substateIterator;
                            finalStateFound = true;
                        }
                    }
                }
            }


            if (!initialStateFound)
            {
                throw StateNotFoundException<T>(initialState.get_id());
            }
            if (!finalStateFound)
            {
                throw StateNotFoundException<T>(finalState.get_id());
            }
            Transition<T> * transition = new Transition<T>(name, initialState, finalState, one_way);
            transitions.push_back(*transition);

            AssertTransitionExistsPostCondition(transition->get_id());
        }

        void delete_transition(int initial_state_id, int final_state_id)
        {
            AssertTransitionExistsPreCondition(initial_state_id, final_state_id);

            auto transition = std::find_if(
                        transitions.begin(),
                        transitions.end(),
                        [&initial_state_id, &final_state_id](Transition<T> & transition) { return transition.get_initial_state().get_id() == initial_state_id && transition.get_final_state().get_id() == final_state_id; }
            );
            if (transition == transitions.end())
            {
                throw TransitionNotFoundException<T>(initial_state_id, final_state_id);
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

            AssertTransitionNotExistsPostCondition(initial_state_id, final_state_id);
        }

        void delete_transition(int transition_id)
        {
            AssertTransitionExistsPreCondition(transition_id);

            transitions.erase(
                std::remove_if(
                    transitions.begin(),
                    transitions.end(),
                    [&transition_id](const Transition<T> & transition) { return transition.get_id() == transition_id; }
                ),
                transitions.end()
            );

            AssertTransitionNotExistsPostCondition(transition_id);
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

        std::vector<State<T>, Allocator<State<T>>> * get_states_vector()
        {
            return &states;
        }

        std::vector<Transition<T>, Allocator<Transition<T>>> * get_transitions_vector()
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
                if (stateIterator->get_states()->size() > 0) {
                    QJsonArray jChildStates;
                    for (auto childStateIterator = stateIterator->get_states()->begin(); childStateIterator != stateIterator->get_states()->end(); ++childStateIterator)
                    {
                        QJsonObject childStateObj;
                        childStateObj["id"] = childStateIterator->get_id();
                        childStateObj["value"] = QString::fromStdString(childStateIterator->get_value());
                        jChildStates.push_back(childStateObj);
                    }
                    obj["states"] = jChildStates;
                }
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

                jTransitions.push_back(obj);
            }

            QJsonArray jCars;
            QJsonArray jPassengers;
            for (auto carIterator = cars.begin(); carIterator != cars.end(); ++carIterator)
            {
                QJsonObject carObj;
                carObj["id"] = carIterator->get_id();
                carObj["activeStateId"] = carIterator->get_state_id();
                carObj["from"] = QString::fromStdString(carIterator->get_from());
                carObj["to"] = QString::fromStdString(carIterator->get_to());
                QJsonArray passengerLogins;
                for (auto passengerIterator = carIterator->get_passengers()->begin(); passengerIterator != carIterator->get_passengers()->end(); ++passengerIterator)
                {
                    std::string login = passengerIterator->get_login();
                    int passenger_state_id = passengerIterator->get_state_id();
                    passengerLogins.push_back(QString::fromStdString(login));

                    QJsonObject passengerObj;
                    passengerObj["login"] = QString::fromStdString(login);
                    passengerObj["activeStateId"] = passenger_state_id;
                    jPassengers.push_back(passengerObj);
                }
                carObj["passengers"] = passengerLogins;
                jCars.push_back(carObj);
            }
            json["states"] = jStates;
            json["transitions"] = jTransitions;
            json["passengers"] = jPassengers;
            json["cars"] = jCars;

            QJsonDocument json_doc(json);
            QString json_string = json_doc.toJson().simplified();
            json_string.replace(" ", "");

            return json_string.toStdString();
        }

        void from_json(std::string jsonStr)
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
                std::string car_from = obj["from"].toString().toStdString();
                std::string car_to = obj["to"].toString().toStdString();
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
                add_car(Car<void>(id,active_state_id,car_passengers,car_from,car_to));
            }
        }

    };

    template <typename T, typename Alloc>
    int StateMachine<T, Alloc>::_counter = 0;
}

#endif // STATEMACHINE_H
