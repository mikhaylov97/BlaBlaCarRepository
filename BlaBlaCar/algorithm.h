#ifndef ALGORITHM_H
#define ALGORITHM_H
#pragma once

#include "statemachine.h"

namespace StateMachineBlaBlaCar
{
    template <typename T, typename Alloc>
    std::vector<State<T>> get_unreachable_states(StateMachine<T, Alloc> & machine)
    {
        // accessible states
        std::vector<int> states_ids;
        for (auto it = (&machine)->begin(); it != (&machine)->end(); it++)
            states_ids.push_back((*it).get_id());

        std::vector<State<T> > unreachable_states;
        for (auto it = machine.get_states_vector()->begin(); it != machine.get_states_vector()->end(); it++)
        {
            if (std::find(states_ids.begin(), states_ids.end(), (*it).get_id()) == states_ids.end())
                unreachable_states.push_back((*it));
        }

        return unreachable_states;
    }

    template <typename T, typename Alloc>
    std::vector<State<T>> get_unreachable_substates(StateMachine<T, Alloc> & machine)
    {
        std::vector<State<T>> * unreachable_substates = new std::vector<State<T>>();
        for (auto stateIterator = machine.get_states_vector()->begin(); stateIterator != machine.get_states_vector()->end(); ++stateIterator)
        {
            if (stateIterator->get_states()->size() > 0)
            {
                std::vector<Transition<T>> transitions = machine.find_all_transitions_for_superstate(*stateIterator);
                for (auto substateIterator = stateIterator->get_states()->begin(); substateIterator != stateIterator->get_states()->end(); ++substateIterator)
                {
                    int substate_id = substateIterator->get_id();
                    auto transition = std::find_if(
                                transitions.begin(),
                                transitions.end(),
                                [&substate_id] (Transition<T> t) { return t.get_final_state().get_id() == substate_id; }
                    );

                    if (transition == transitions.end())
                    {
                        unreachable_substates->push_back(*substateIterator);
                    }
                }
                //delete enter substate (it is always available)
                int enter_id = stateIterator->get_states()->front().get_id();
                unreachable_substates->erase(
                    std::remove_if(
                        unreachable_substates->begin(),
                        unreachable_substates->end(),
                        [&enter_id](State<T> state) { return state.get_id() == enter_id; }
                    ),
                    unreachable_substates->end()
                );
            }
        }

        return *unreachable_substates;
    }
}

#endif // ALGORITHM_H
