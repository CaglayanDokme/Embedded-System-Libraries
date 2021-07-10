/**
 * @file        StackContainer.h
 * @details     A template stack container class for embedded systems.
 *              The contatiner is implemented without any dynamic allocation feature.
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        July 10, 2021 -> First release
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
    void push(const value_type& value);
    void push(value_type&& value);

    template <class... Args>
    void emplace(Args&&... args);
    void pop();
    void swap(Stack& swapStack);

    /*** Status Checkers ***/
    NODISCARD bool        empty() const { return (0     == sz); }
    NODISCARD bool        full()  const { return (SIZE  == sz); }
    NODISCARD size_type   size()  const { return sz;            }

private:
    /*** Members ***/
    size_type  sz;          // General size
    size_type  idxTop;      // Index after the top element
    value_type data[SIZE];  // Contained data
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
        for(size_type elemIdx = 0; elemIdx < copyStack.sz; ++elemIdx)
            data[elemIdx] = copyStack.data[elemIdx];

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
    return data[idxTop-1];
}

/**
 * @brief   Returns a reference to the top element of the Stack
 * @return  lValue reference to the front element
 */
template<class T, std::size_t SIZE>
T& Stack<T, SIZE>::top()
{
    if(empty() == true)
        return data[0];

    return data[idxTop-1];
}

/**
 * @brief Pushes the given element to the top of the Stack
 * @param value     Reference to the value to be copied
 */
template<class T, std::size_t SIZE>
void Stack<T, SIZE>::push(const value_type& value)
{
    if(full() == true)
        return;

    data[idxTop] = value;

    ++idxTop;
    ++sz;
}

/**
 * @brief Pushes the given element to the top of the Stack
 * @param value     rValue Reference to the value to be moveds
 */
template<class T, std::size_t SIZE>
void Stack<T, SIZE>::push(value_type&& value)
{
    if(full() == true)
        return;

    data[idxTop] = std::move(value);

    ++idxTop;
    ++sz;
}

/**
 * @brief Pushes the given element to the top of the Stack by constructing it in-place
 * @param value     Arguments for constructing the new element
 */
template<class T, std::size_t SIZE>
template <class... Args>
void Stack<T, SIZE>::emplace(Args&&... args)
{
    if(full() == true)
        return;

    data[idxTop] = std::forward(args...);

    ++idxTop;
    ++sz;
}

/**
 * @brief Pops the top element of the stack
 */
template<class T, std::size_t SIZE>
void Stack<T, SIZE>::pop()
{
    if(empty() == true)
        return;

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
