/**
 * @file        QueueContainer.h
 * @details     A template queue container class for embedded systems.
 *              The contatiner is implemented without any dynamic allocation feature.
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        July 9, 2021 -> First release
 * @note        Feel free to contact for questions, bugs or any other thing.
 * @copyright   No copyright.
 */

/*** Recursive inclusion preventer ***/
#pragma once

/*** Libraries ***/
#include <cstddef>  // std::size_t
#include <utility>  // std::move, std::swap

/*** Special definitions ***/
// If the C++ version is greater or equal to 2017xx
#if __cplusplus >= 201703l
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

/*** Container Class ***/
template<class T, std::size_t SIZE = 128>
class Queue{
public:
    /*** C++ Standard Named Requirements for Containers ***/
    using value_type        = T;
    using reference         = T&;
    using const_reference   = const T&;
    using iterator          = T*;
    using const_iterator    = const T*;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;

    /*** Constructors and Destructor ***/
    // Default constructor
    Queue();

    /*** Element Access ***/
    NODISCARD const_reference front() const;
    NODISCARD reference       front();
    NODISCARD const_reference back() const;
    NODISCARD reference       back();

    /*** Modifiers ***/
    void push(const value_type& value);
    void push(value_type&& value);

    template <class... Args>
    void emplace(Args&&... args);
    void pop();
    void swap(Queue& swapQ);

    /*** Status Checkers ***/
    NODISCARD bool        empty() const { return (0     == sz); }
    NODISCARD bool        full()  const { return (SIZE  == sz); }
    NODISCARD size_type   size()  const { return sz;            }

private:
    /*** Members ***/
    size_type  sz;          // General size
    size_type  idxFront;    // Index of the front element
    size_type  idxBack;     // Index of the back element
    value_type data[SIZE];  // Contained data

    void IncrementIndex(size_type& index) { index = (index + 1) % SIZE; }
};

template<class T, std::size_t SIZE>
Queue<T,SIZE>::Queue()
    : sz(0), idxFront(0), idxBack(SIZE-1)
{ /* No operation */ }

/**
 * @brief   Returns a constant reference to the front element of Queue
 * @return  Constant lValue reference to the front element
 */
template<class T, std::size_t SIZE>
const T& Queue<T, SIZE>::front() const
{
    return data[idxFront];
}

/**
 * @brief   Returns a reference to the front element of Queue
 * @return  lValue reference to the front element
 */
template<class T, std::size_t SIZE>
T& Queue<T, SIZE>::front()
{
    return data[idxFront];
}

/**
 * @brief   Returns a constant reference to the back element of Queue
 * @return  Constant lValue reference to the back element
 */
template<class T, std::size_t SIZE>
const T& Queue<T, SIZE>::back() const
{
    return data[idxBack];
}

/**
 * @brief   Returns a reference to the back element of Queue
 * @return  lValue reference to the back element
 */
template<class T, std::size_t SIZE>
T& Queue<T, SIZE>::back()
{
    return data[idxBack];
}

template<class T, std::size_t SIZE>
void Queue<T, SIZE>::push(const value_type& value)
{
    // Adjust back index
    IncrementIndex(idxBack);

    // Copy value
    data[idxBack] = value;

    // Adjust front index
    if(full() == true)  // Queue may be full, overwrite the oldest element
        IncrementIndex(idxFront);

    // Adjust size
    sz += full() ? 0 : 1;
}

template<class T, std::size_t SIZE>
void Queue<T, SIZE>::push(value_type&& value)
{
    // Adjust back index
    IncrementIndex(idxBack);

    // Copy value
    data[idxBack] = std::move(value);

    // Adjust front index
    if(full() == true)  // Queue may be full, overwrite the oldest element
        IncrementIndex(idxFront);

    // Adjust size
    sz += full() ? 0 : 1;
}

template<class T, std::size_t SIZE>
template <class... Args>
void Queue<T, SIZE>::emplace(Args&&... args)
{
    // Adjust back index
    IncrementIndex(idxBack);

    // Copy value
    data[idxBack] = std::forward(args...);

    // Adjust front index
    if(full() == true)  // Queue may be full, overwrite the oldest element
        IncrementIndex(idxFront);

    // Adjust size
    sz += full() ? 0 : 1;
}

template<class T, std::size_t SIZE>
void Queue<T, SIZE>::pop()
{
    if(empty() == false)
    {
        IncrementIndex(idxFront);

        // Decrement size
        --sz;
    }
}

template<class T, std::size_t SIZE>
void Queue<T, SIZE>::swap(Queue& swapQ)
{
    if(swapQ.empty() == false)
    {
        std::swap(data,     swapQ.data);
        std::swap(idxBack,  swapQ.idxBack);
        std::swap(idxFront, swapQ.idxFront);
        std::swap(sz,       swapQ.sz);
    }
}
