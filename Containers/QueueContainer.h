/**
 * @file        QueueContainer.h
 * @details     A template queue container class for embedded systems.
 *              The contatiner is implemented without any dynamic allocation feature.
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        July 9, 2021    -> First release
 *              July 10, 2021   -> Doxygen added. Copy constructor added.
 *                              -> Data buffer replaced with uint8_t array to support
 *                                 data types without default constructor.
 *                              -> Default size parameter removed from template arguments
 * @note        Feel free to contact for questions, bugs or any other thing.
 * @copyright   No copyright.
 */

/*** Recursive inclusion preventer ***/
#pragma once

/*** Libraries ***/
#include <cstddef>  // std::size_t
#include <utility>  // std::move, std::swap
#include <cstdint>  // std::uint8_t

/*** Special definitions ***/
// If the C++ version is greater or equal to 2017xx
#if __cplusplus >= 201703l
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

/*** Container Class ***/
template<class T, std::size_t SIZE>
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

    // Copy Constructor
    Queue(const Queue& copyQ);

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
    size_type    sz;          // General size
    size_type    idxFront;    // Index of the front element
    size_type    idxBack;     // Index of the back element
    std::uint8_t data[SIZE * sizeof(T)];  // Contained data

    void IncrementIndex(size_type& index) { index = (index + 1) % SIZE; }
};

/**
 * @brief Default constructor
 */
template<class T, std::size_t SIZE>
Queue<T,SIZE>::Queue()
    : sz(0), idxFront(0), idxBack(SIZE-1)
{ /* No operation */ }

/**
 * @brief Copy constructor
 */
template<class T, std::size_t SIZE>
Queue<T,SIZE>::Queue(const Queue& copyQ)
    : sz(0), idxFront(0), idxBack(SIZE-1)
{
    if(copyQ.empty() == false)
    {
        // Copy construct each element
        for(size_type elemIdx = 0; elemIdx < copyQ.sz; ++elemIdx)
            new(reinterpret_cast<value_type*>(data) + elemIdx) value_type(copyQ.data[(copyQ.idxFront + elemIdx) % SIZE]);

        idxFront    = 0;
        idxBack     = copyQ.sz - 1;
        sz          = copyQ.sz;
    }
}

/**
 * @brief   Returns a constant reference to the front element of Queue
 * @return  Constant lValue reference to the front element
 */
template<class T, std::size_t SIZE>
const T& Queue<T, SIZE>::front() const
{
    return reinterpret_cast<value_type*>(data)[idxFront];
}

/**
 * @brief   Returns a reference to the front element of Queue
 * @return  lValue reference to the front element
 */
template<class T, std::size_t SIZE>
T& Queue<T, SIZE>::front()
{
    return reinterpret_cast<value_type*>(data)[idxFront];
}

/**
 * @brief   Returns a constant reference to the back element of Queue
 * @return  Constant lValue reference to the back element
 */
template<class T, std::size_t SIZE>
const T& Queue<T, SIZE>::back() const
{
    return reinterpret_cast<value_type*>(data)[idxFront];
}

/**
 * @brief   Returns a reference to the back element of Queue
 * @return  lValue reference to the back element
 */
template<class T, std::size_t SIZE>
T& Queue<T, SIZE>::back()
{
    return reinterpret_cast<value_type*>(data)[idxFront];
}

/**
 * @brief   Pushes the element to the Queue
 * @param   value   Constant lValue reference to the object to be pushed
 * @note    Overwrites the oldest element in the Queue.
 */
template<class T, std::size_t SIZE>
void Queue<T, SIZE>::push(const value_type& value)
{
    // Adjust back index
    IncrementIndex(idxBack);

    // Copy construct element at the back
    new(reinterpret_cast<value_type*>(data) + idxBack) value_type(value);

    // Adjust front index
    if(full() == true)  // Queue may be full, overwrite the oldest element
        IncrementIndex(idxFront);

    // Adjust size
    sz += full() ? 0 : 1;
}

/**
 * @brief   Pushes the element to the Queue
 * @param   value   rValue reference to the object to be pushed
 * @note    Overwrites the oldest element in the Queue.
 */
template<class T, std::size_t SIZE>
void Queue<T, SIZE>::push(value_type&& value)
{
    // Adjust back index
    IncrementIndex(idxBack);

    // Move construct element at the backs
    new(reinterpret_cast<value_type*>(data) + idxBack) value_type(std::move(value));

    // Adjust front index
    if(full() == true)  // Queue may be full, overwrite the oldest element
        IncrementIndex(idxFront);

    // Adjust size
    sz += full() ? 0 : 1;
}

/**
 * @brief   Pushes the element by constructing it in-place with the given arguments
 * @param   args  Arguments to be forwarded to the constructor of the new element
 * @note    Overwrites the oldest element in the Queue.
 */
template<class T, std::size_t SIZE>
template <class... Args>
void Queue<T, SIZE>::emplace(Args&&... args)
{
    // Adjust back index
    IncrementIndex(idxBack);

    // In-place construct element with the arguments at the back
    new(reinterpret_cast<value_type*>(data) + idxBack) value_type(std::forward(args...));

    // Adjust front index
    if(full() == true)  // Queue may be full, overwrite the oldest element
        IncrementIndex(idxFront);

    // Adjust size
    sz += full() ? 0 : 1;
}

/**
 * @brief Pops the front element of the Queue
 */
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

/**
 * @brief Swaps the content of two Queues
 * @param swapQ     Queue to be swapped with
 */
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
