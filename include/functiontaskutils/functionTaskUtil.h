#ifndef FUNCTION_TASK_UTILS_H
#define FUNCTION_TASK_UTILS_H

#include <tuple>
#include <type_traits>
#include <vector>

namespace FunctionTaskUtils{

    template<typename T> struct is_vector{static constexpr bool value = false;};
    template<typename T> struct is_vector<std::vector<T>>{static constexpr bool value = true;};

    template<typename T>
    struct typeVector{};

    //should only be used after checking for is_vector to avoid wierd errors
    template<typename T>
    struct typeVector<std::vector<T>>{
        using type = T;
    };


    template<typename T>
    size_t getSizeOfVector(const T & t){return size_t(-1);};

    template<typename T>
    size_t getSizeOfVector(const std::vector<T> & t){return t.size();};




    template <typename T>
    struct argumentsToDividedTask{
        bool is_vec = false;
        size_t size;
        size_t current_offset = 0;
        T* t;
    };

    template<typename T>
    struct typeArgument{};


    template <typename T>
    struct argumentsToDividedTask<std::vector<T>>{
        bool is_vec = true;
        size_t size;
        size_t current_offset = 0;
        typeVector<std::vector<T>>::type* t;

    };

    template<typename T>
    argumentsToDividedTask<T> getArgument(argumentsToDividedTask<T> & t){return t;};

    template<typename T>
    size_t getSizeOfArgument(const T & t){return size_t(-1);};

    template<typename T>
    size_t getSizeOfArgument(const argumentsToDividedTask<T> & t){return t.size;};


    template <typename T>
    struct is_argumentToDividedTask{static constexpr bool value = false;};


    template <typename T>
    struct is_argumentToDividedTask<argumentsToDividedTask<T>>{static constexpr bool value = true;};

    template <typename T>
    argumentsToDividedTask<T> divideTask(T & t, size_t sizeOfDiv, size_t offset){
        argumentsToDividedTask<T> r;
        r.size = 0;
        r.t = &t;
        return r; 
    }

    template <typename T>
    argumentsToDividedTask<std::vector<T>> divideTask(std::vector<T> & t, size_t sizeOfDiv, size_t offset){
        argumentsToDividedTask<std::vector<T>> r;
        r.size = sizeOfDiv;
        r.t = ((&t[0]) + offset);
        return r;
    }

    template <typename T>
    T getResultVector(T& t){
        return t;
    }

    template <typename T>
    std::vector<T> getResultVector(std::vector<T>* t){
        return *t;
    }
    
}


#endif