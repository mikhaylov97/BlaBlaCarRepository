#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#pragma once

#include <memory>
#include <limits>

namespace StateMachineBlaBlaCar
{
    template <typename T>
    class Allocator
    {
    public:
        using value_type = T;

        using pointer = T * ;
        using const_pointer = const T*;

        using size_type = size_t;

        template<typename U>
        struct rebind
        {
            using other = Allocator<U>;
        };

        // default constructors and destructor
        Allocator() = default;
        ~Allocator() = default;
        template<typename U>
        Allocator(const Allocator<U>& other) {}

        // allocate memory
        pointer allocate(size_type num_of_object);

        // deallocate memory
        void deallocate(pointer p, size_type num_of_object);

        // max memory size
        size_type max_size() const;

        // construct object
        template<typename U, typename ... Args>
        void construct(U* p, Args & ...args);

        // destroy object
        template<typename U>
        void destroy(U *p);
    };


    template <typename T>
    typename Allocator<T>::pointer Allocator<T>::allocate(size_type num_of_object)
    {
        return static_cast<pointer>(operator new(sizeof(T) * num_of_object));
    }

    template <typename T>
    void Allocator<T>::deallocate(pointer p, size_type num_of_object)
    {
        operator delete(p);
    }

    template <typename T>
    typename Allocator<T>::size_type Allocator<T>::max_size() const
    {
        return std::numeric_limits<size_type>::max();
    }

    template <typename T>
    template<typename U, typename ... Args>
    void Allocator<T>::construct(U* p, Args & ...args)
    {
        new(p) U(std::forward<Args>(args)...);
    }

    template <typename T>
    template<typename U>
    void Allocator<T>::destroy(U *p)
    {
        p->~U();
    }
}

#endif // ALLOCATOR_H
