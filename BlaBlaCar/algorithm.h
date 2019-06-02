#ifndef ALGORITHM_H
#define ALGORITHM_H
#pragma once

#include "statemachine.h"

namespace StateMachineBlaBlaCar
{
    template <typename T, typename Alloc>
    std::vector<State<T>> get_unreachable_states(StateMachine<T, Alloc>& machine)
    {
        // accessible states
        std::vector<int> states_ids;
        for (auto it = machine.begin(); it != machine.end(); it++)
            states_ids.push_back((*it).get_id());

        std::vector<State<T> > unreachable_states;
        for (auto it = machine.get_states_vector().begin(); it != machine.get_states_vector().end(); it++)
        {
            if (std::find(states_ids.begin(), states_ids.end(), (*it).get_id()) == states_ids.end())
                unreachable_states.push_back((*it));
        }

        return unreachable_states;
    }
}

#endif // ALGORITHM_H
