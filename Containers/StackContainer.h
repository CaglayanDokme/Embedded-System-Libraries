/**
 * @file        StackContainer.h
 * @details     A template stack container class for embedded systems.
 *              The contatiner is implemented without any dynamic allocation feature.
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        July 10, 2021 -> First release
 *              July 11, 2021 -> Data buffer replaced with uint8_t array to support
 *                               data types without default constructor.
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
class Stack{
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
    Stack();

    // Copy constructor
    Stack(const Stack& copyStack);

    /*** Element Access ***/
    NODISCARD const_reference top() const;
    NODISCARD reference       top();

    /*** Modifiers ***/
    template <class... Args>
    bool emplace(Args&&... args);
    bool push(const value_type& value);
    bool push(value_type&& value);
    void pop();
    void swap(Stack& swapStack);

    /*** Status Checkers ***/
    NODISCARD bool        empty() const { return (0     == sz); }
    NODISCARD bool        full()  const { return (SIZE  == sz); }
    NODISCARD size_type   size()  const { return sz;            }

private:
    /*** Members ***/
    size_type    sz;          // General size
    size_type    idxTop;      // Index after the top element
    std::uint8_t data[SIZE * sizeof(T)];  // Contained data
};

/**
 * @brief Default constructor
 */
template<class T, std::size_t SIZE>
Stack<T, SIZE>::Stack()
    : sz(0), idxTop(0)
{ /* No operation */ }

/**
 * @brief Copy constructor
 * @param copyStack     Source stack for copying
 */
template<class T, std::size_t SIZE>
Stack<T, SIZE>::Stack(const Stack& copyStack)
    : sz(0), idxTop(0)
{
    if(copyStack.empty() == false)
    {
        // Copy construct each element
        for(size_type elemIdx = 0; elemIdx < copyStack.sz; ++elemIdx)
            new(reinterpret_cast<value_type*>(data) + elemIdx) value_type(copyStack.data[elemIdx]);

        idxTop  = copyStack.idxTop;
        sz      = copyStack.sz;
    }
}

/**
 * @brief   Returns a constant reference to the top element of the Stack
 * @return  Constant lValue reference to the front element
 */
template<class T, std::size_t SIZE>
const T& Stack<T, SIZE>::top() const
{
    if(empty())
        return reinterpret_cast<value_type*>(data)[0];

    return reinterpret_cast<value_type*>(data)[idxTop-1];
}

/**
 * @brief   Returns a reference to the top element of the Stack
 * @return  lValue reference to the front element
 */
template<class T, std::size_t SIZE>
T& Stack<T, SIZE>::top()
{
    if(empty())
        return reinterpret_cast<value_type*>(data)[0];

    return reinterpret_cast<value_type*>(data)[idxTop-1];
}

/**
 * @brief Pushes the given element to the top of the Stack
 * @param value     Reference to the value to be copied
 */
template<class T, std::size_t SIZE>
bool Stack<T, SIZE>::push(const value_type& value)
{
    if(full())
        return false;

    new(reinterpret_cast<value_type*>(data) + idxTop) value_type(value);

    ++idxTop;
    ++sz;

    return true;
}

/**
 * @brief Pushes the given element to the top of the Stack
 * @param value     rValue Reference to the value to be moved
 */
template<class T, std::size_t SIZE>
bool Stack<T, SIZE>::push(value_type&& value)
{
    return emplace(std::move(value));
}

/**
 * @brief Pushes the given element to the top of the Stack by constructing it in-place
 * @param value     Arguments for constructing the new element
 */
template<class T, std::size_t SIZE>
template <class... Args>
bool Stack<T, SIZE>::emplace(Args&&... args)
{
    if(full())
        return false;

    new(reinterpret_cast<value_type*>(data) + idxTop) value_type(std::forward<Args>(args)...);

    ++idxTop;
    ++sz;

    return true;
}

/**
 * @brief Pops the top element of the stack
 */
template<class T, std::size_t SIZE>
void Stack<T, SIZE>::pop()
{
    if(empty())
        return;

    // Explicitly call the destructor as we used the placement new
    reinterpret_cast<value_type*>(data)[idxTop-1].~value_type();

    --idxTop;
    --sz;
}

/**
 * @brief Swaps the content of two Stacks
 * @param swapStack     Stack to be swapped with
 */
template<class T, std::size_t SIZE>
void Stack<T, SIZE>::swap(Stack& swapStack)
{
    std::swap(sz,       swapStack.sz);
    std::swap(idxTop,   swapStack.idxTop);
    std::swap(data,     swapStack.data);
}
