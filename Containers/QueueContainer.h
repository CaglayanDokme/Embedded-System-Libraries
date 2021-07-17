/**
 * @file        QueueContainer.h
 * @details     A template queue container class for embedded systems.
 *              The container is implemented without any dynamic allocation feature.
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        July 9, 2021    -> First release
 *              July 10, 2021   -> Doxygen added. Copy constructor added.
 *                              -> Data buffer replaced with uint8_t array to support
 *                                 data types without default constructor.
 *                              -> Default size parameter removed from template arguments
 *              July 11, 2021   -> Index incrementor method optimized, mod operation removed
 *              July 15, 2021   -> Data storage replaced with std::aligned_storage
 *                                 capacity() method added.
 *              July 16, 2021   -> Missing destructor implemented.
 *              July 17, 2021   -> lvalue ref-qualifier added to assignment operator.
 *                              -> Helper function at(..) added to avoid repetition of casting.
 *
 * @note        Feel free to contact for questions, bugs or any other thing.
 * @copyright   No copyright.
 */

/*** Recursive inclusion preventer ***/
#pragma once

/*** Libraries ***/
#include <cstddef>      // std::size_t
#include <utility>      // std::move, std::swap
#include <type_traits>  // std::aligned_storage

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
    static_assert(SIZE != 0, "Queue capacity cannot be zero!");

public:
    /*** C++ Standard Named Requirements for Containers ***/
    using value_type        = T;
    using reference         = T&;
    using const_reference   = const T&;
    using iterator          = T*;
    using const_iterator    = const T*;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    using aligned_data      = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

    /*** Constructors and Destructor ***/
    // Default constructor
    Queue();

    // Copy Constructor
    Queue(const Queue& copyQ);

    // Destructor
    ~Queue();

    /*** Element Access ***/
    NODISCARD const_reference front() const;
    NODISCARD reference       front();
    NODISCARD const_reference back() const;
    NODISCARD reference       back();

    /*** Modifiers ***/
    template <class... Args>
    bool emplace(Args&&... args);
    bool push(const value_type& value);
    bool push(value_type&& value);

    void pop();
    void swap(Queue& swapQ);

    /*** Status Checkers ***/
    NODISCARD bool      empty()     const { return (0     == sz); } // true if the Queue is empty
    NODISCARD bool      full()      const { return (SIZE  == sz); } // true if the Queue is full
    NODISCARD size_type size()      const { return sz;            } // Current size of the Queue
    NODISCARD size_type capacity()  const { return SIZE;          } // Maximum capacity of the Queue
    NODISCARD size_type available() const { return (SIZE - sz);   } // Available slots in Queue

    /*** Operators ***/
    bool operator==(const Queue& compQ) const;
    bool operator!=(const Queue& compQ) const;
    Queue& operator=(const Queue& sourceQ) &;

private:
    /*** Members ***/
    size_type    sz;            // General size
    size_type    idxFront;      // Index of the front element
    size_type    idxBack;       // Index of the back element
    aligned_data data[SIZE];    // Stored data

    /*** Helper functions ***/
    static void IncrementIndex(size_type& index) // Increments any index by not violating the range
    {
        index = (SIZE-1 == index) ? 0 : index+1;
    }

    NODISCARD const_reference at(const size_type elemIdx) const
    {
        return reinterpret_cast<const_reference>(data[elemIdx]);
    }

    NODISCARD reference at(const size_type elemIdx)
    {
        return reinterpret_cast<reference>(data[elemIdx]);
    }
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
    *this = copyQ;
}

/**
 * @brief   Destructor
 * @note    Calls the destructor of each element explicitly
 */
template<class T, std::size_t SIZE>
Queue<T,SIZE>::~Queue()
{
    while(!empty())
        pop();
}

/**
 * @brief   Returns a constant lValue reference to the front element of Queue
 * @return  Constant lValue reference to the front element
 */
template<class T, std::size_t SIZE>
const T& Queue<T, SIZE>::front() const
{
    return at(idxFront);
}

/**
 * @brief   Returns an lValue reference to the front element of Queue
 * @return  lValue reference to the front element
 */
template<class T, std::size_t SIZE>
T& Queue<T, SIZE>::front()
{
    return at(idxFront);
}

/**
 * @brief   Returns a constant lValue reference to the back element of Queue
 * @return  Constant lValue reference to the back element
 */
template<class T, std::size_t SIZE>
const T& Queue<T, SIZE>::back() const
{
    return at(idxBack);
}

/**
 * @brief   Returns an lValue reference to the back element of Queue
 * @return  lValue reference to the back element
 */
template<class T, std::size_t SIZE>
T& Queue<T, SIZE>::back()
{
    return at(idxBack);
}

/**
 * @brief   Pushes the element by constructing it in-place with the given arguments
 * @param   args    Arguments to be forwarded to the constructor of the new element
 * @return  true    If the operation is successful.
 *          false   If the queue was full
 */
template<class T, std::size_t SIZE>
template <class... Args>
bool Queue<T, SIZE>::emplace(Args&&... args)
{
    if(full())
        return false;

    // Adjust back index
    IncrementIndex(idxBack);

    // In-place construct element with the arguments at the back
    new(data + idxBack) value_type(std::forward<Args>(args)...);

    // Adjust front index
    if(full())  // Queue may be full, overwrite the oldest element
        IncrementIndex(idxFront);

    // Adjust size
    sz += full() ? 0 : 1;

    return true;
}

/**
 * @brief   Pushes the element to the Queue
 * @param   value   Constant lValue reference to the object to be pushed
 * @return  true    If the operation is successful.
 *          false   If the queue was full
 */
template<class T, std::size_t SIZE>
bool Queue<T, SIZE>::push(const value_type& value)
{
    if(full())
        return false;

    // Adjust back index
    IncrementIndex(idxBack);

    // Copy construct element at the back
    new(data + idxBack) value_type(value);

    // Adjust front index
    if(full())  // Queue may be full, overwrite the oldest element
        IncrementIndex(idxFront);

    // Adjust size
    sz += full() ? 0 : 1;

    return true;
}

/**
 * @brief   Pushes the element to the Queue
 * @param   value   rValue reference to the object to be pushed
 * @return  true    If the operation is successful.
 *          false   If the queue was full
 */
template<class T, std::size_t SIZE>
bool Queue<T, SIZE>::push(value_type&& value)
{
    return emplace(std::move(value));
}

/**
 * @brief   Pops the front element of the Queue
 * @note    Explicitly calls the destructor of the popped element
 */
template<class T, std::size_t SIZE>
void Queue<T, SIZE>::pop()
{
    if(!empty())
    {
        // Explicitly call the destructor as we used the placement new
        at(idxFront).~value_type();

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
    std::swap(data,     swapQ.data);
    std::swap(idxBack,  swapQ.idxBack);
    std::swap(idxFront, swapQ.idxFront);
    std::swap(sz,       swapQ.sz);
}

/**
 * @brief   Comparison operator
 * @param   compQ   Queue to be compared with.
 * @return  true    If both Queues are equal.
 */
template<class T, std::size_t SIZE>
bool Queue<T, SIZE>::operator==(const Queue& compQ) const
{
    if(compQ.size() != sz)  // Size must be equal
        return false;

    // Element-wise comparison
    size_type index0 = idxFront, index1 = compQ.idxFront;
    for(size_type elemIdx = 0; elemIdx < compQ.size(); ++elemIdx)
    {
        if(compQ.at(index1) != at(index0))
            return false;

        IncrementIndex(index0);
        IncrementIndex(index1);
    }

    // Queues are equal if the function reaches here
    return true;
}

/**
 * @brief   Incomparison operator
 * @param   compQ   Queue to be compared with.
 * @return  true    If Queues are not equal.
 */
template<class T, std::size_t SIZE>
bool Queue<T, SIZE>::operator!=(const Queue& compQ) const
{
    return !(compQ == *this);
}

/**
 * @brief   Copy assignment operator
 * @param   sourceQ     Queue to be copied from
 * @return  lValue reference to the left Queue to support cascaded operations
 */
template<class T, std::size_t SIZE>
Queue<T, SIZE>& Queue<T, SIZE>::operator=(const Queue& sourceQ) &
{
    // Pop all elements first
    while(!empty())
        pop();

    if(!sourceQ.empty())
    {
        // Copy construct each element
        size_type sourceIdx = sourceQ.idxFront;
        for(size_type elemIdx = 0; elemIdx < sourceQ.sz; ++elemIdx, IncrementIndex(sourceIdx))
            new(data + elemIdx) value_type(sourceQ.at(sourceIdx));

        idxFront    = 0;
        idxBack     = sourceQ.sz - 1;
        sz          = sourceQ.sz;
    }

    return *this;
}
