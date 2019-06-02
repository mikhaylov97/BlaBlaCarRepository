#ifndef ITERATOR_H
#define ITERATOR_H
#pragma once

#include "exceptions.h"
#include "statemachine.h"


namespace StateMachineBlaBlaCar
{
    // fix circular dependency between Iterator.h and StateDiagram.h
    template <typename T, typename Alloc>
    class StateMachine;


    // Iterator
    template <typename T, typename Alloc = std::allocator<T> >
    class Iterator
    {
    private:
        State<T> * iterator;
        StateMachine<T, Alloc> & stateMachine;

        std::vector<int> visited_states_indexes;

        State<T> * deepFirstSearch(State<T>*);

    public:
        Iterator(StateMachine<T, Alloc> & stateMachine, State<T> * iterator) : iterator(iterator), stateMachine(stateMachine)
        {
            if (iterator != nullptr)
                visited_states_indexes.push_back(iterator->get_id());
        }

        Iterator<T, Alloc> & operator++();
        Iterator<T, Alloc> operator++(int);

        template<class OtherType>
        bool operator==(const Iterator<OtherType, Alloc> & rhs) const;
        template<class OtherType>
        bool operator!=(const Iterator<OtherType, Alloc> & rhs) const;

        State<T> operator*() const;
        State<T> operator->() const;
    };


    // Pre-increment
    template <typename T, typename Alloc>
    inline Iterator<T, Alloc> & Iterator<T, Alloc>::operator++ ()
    {
        if (iterator == nullptr)
            throw IteratorOutOfBoundsException<T>(iterator);

        iterator = deepFirstSearch(iterator);
        if (iterator != nullptr)
            visited_states_indexes.push_back(iterator->get_id());

        return * this;
    }

    // Post-increment
    template <typename T, typename Alloc>
    inline Iterator<T, Alloc> Iterator<T, Alloc>::operator++ (int)
    {
        if (iterator == nullptr)
            throw IteratorOutOfBoundsException<T>(iterator);

        Iterator<T, Alloc> tmp(*this);

        iterator = deepFirstSearch(iterator);
        if (iterator != nullptr)
            visited_states_indexes.push_back(iterator->get_id());

        return tmp;
    }

    template <typename T, typename Alloc>
    State<T> * Iterator<T, Alloc>::deepFirstSearch(State<T> * initial_state)
    {
        // find all next transitions
        std::vector<Transition<T>> next_transitions;
        auto i = stateMachine.get_transitions().begin();
        while ((i = std::find_if(
                    i,
                    stateMachine.get_transitions().end(),
                    [&initial_state](Transition<T> t) { return t.get_initial_state().get_id() == initial_state->get_id(); }
        )) != stateMachine.get_transitions().end())
        {
            if (std::find(
                        visited_states_indexes.begin(),
                        visited_states_indexes.end(),
                        (*i).get_final_state().get_id()) == visited_states_indexes.end())
                next_transitions.push_back((*i));

            i++;
        }

        // we reached the bottom
        if (next_transitions.empty())
        {
            // go back
            std::vector<Transition<T>> back_transitions;
            auto i = stateMachine.get_transitions().begin();
            while ((i = std::find_if(
                        i,
                        stateMachine.get_transitions().end(),
                [&initial_state](Transition<T> t) { return t.FinalState().Id() == initial_state->Id(); })) != stateMachine.get_transitions().end())
            {
                back_transitions.push_back((*i));
                i++;
            }

            if (back_transitions.empty())
                return nullptr;

            auto back_transition = std::min_element(back_transitions.begin(), back_transitions.end(),
                [](Transition<T> x, Transition<T> y) {  return x.get_initial_state().get_id() < y.get_initial_state().get_id(); });

            auto prev = std::find_if(stateMachine.get_states().begin(), stateMachine.get_states().end(),
                [&back_transition](const State<T> & state) { return state.get_id() == (*back_transition).get_initial_state().get_id(); });

            return deepFirstSearch(&(*prev));
        }

        // find next transition (with minimum state id)
        auto next_transition = std::min_element(next_transitions.begin(), next_transitions.end(),
            [](Transition<T> x, Transition<T> y) {  return x.get_final_state().get_id() < y.get_final_state().get_id(); });

        // find next state
        auto next = std::find_if(stateMachine.get_states().begin(), stateMachine.get_states().end(),
            [&next_transition](const State<T> & state) { return state.get_id() == (*next_transition).get_final_state().get_id(); });

        return &(*next);
    }

    template<typename T, typename Alloc>
    template<class OtherType>
    inline bool Iterator<T, Alloc>::operator==(const Iterator<OtherType, Alloc>& rhs) const
    {
        return iterator == rhs.iterator;
    }

    template<typename T, typename Alloc>
    template<class OtherType>
    inline bool Iterator<T, Alloc>::operator!=(const Iterator<OtherType, Alloc>& rhs) const
    {
        return iterator != rhs.iterator;
    }

    template<typename T, typename Alloc>
    inline State<T> Iterator<T, Alloc>::operator*() const
    {
        if (iterator == nullptr)
            throw IteratorOutOfBoundsException<T>(iterator);

        return *iterator;
    }

    template<typename T, typename Alloc>
    inline State<T> Iterator<T, Alloc>::operator->() const
    {
        if (iterator == nullptr)
            throw IteratorOutOfBoundsException<T>(iterator);

        return *iterator;
    }
}

#endif // ITERATOR_H
