/**
 * @file        StackContainer.h
 * @details     A template stack container class for embedded systems.
 *              The container is implemented without any dynamic allocation feature.
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        July 10, 2021 -> First release
 *              July 11, 2021 -> Data buffer replaced with uint8_t array to support
 *                               data types without default constructor.
 *              July 15, 2021 -> Data storage replaced with std::aligned_storage
 *                            -> capacity() method added.
 *              July 16, 2021 -> Member variable sz removed as it is easily derivable from idxTop
 *                            -> Missing destructor added.
 *                            -> lvalue ref-qualifier added to assignment operator.
 *                            -> Helper function at(..) added to avoid repetition of casting.
 *
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
template<class T, std::size_t SIZE>
class Stack{
    static_assert(SIZE != 0, "Stack capacity cannot be zero!");

public:
    /*** C++ Standard Named Requirements for Containers ***/
    using value_type        = T;
    using reference         = T&;
    using const_reference   = const T&;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    using aligned_data      = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

    /*** Constructors and Destructor ***/
    // Default constructor
    Stack() = default;

    // Copy constructor
    Stack(const Stack& copyStack);

    // Destructor
    ~Stack();

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

    /*** Operators ***/
    bool operator==(const Stack& compStack) const;
    bool operator!=(const Stack& compStack) const;
    Stack& operator=(const Stack& sourceStack) &;

    /*** Status Checkers ***/
    NODISCARD bool      empty()     const { return (0     == idxTop); } // true if the Stack is empty
    NODISCARD bool      full()      const { return (SIZE  == idxTop); } // true if the Stack is full
    NODISCARD size_type size()      const { return idxTop;            } // Current size of the Stack
    NODISCARD size_type capacity()  const { return SIZE;              } // Maximum capacity of the Stack
    NODISCARD size_type available() const { return (SIZE - size());   } // Available slots in Stack

private:
    /*** Members ***/
    size_type    idxTop{0};     // Index after the top element
    aligned_data data[SIZE];    // Contained data

    /*** Helper Functions ***/
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
 * @brief Copy constructor
 * @param copyStack     Source stack for copying
 */
template<class T, std::size_t SIZE>
Stack<T, SIZE>::Stack(const Stack& copyStack)
{
    *this = copyStack;
}

/**
 * @brief Destructor
 */
template<class T, std::size_t SIZE>
Stack<T, SIZE>::~Stack()
{
    while(!empty())
        pop();
}

/**
 * @brief   Returns a constant reference to the top element of the Stack
 * @return  Constant lValue reference to the front element
 * @note    The referenced data is not valid if the Stack is empty
 */
template<class T, std::size_t SIZE>
const T& Stack<T, SIZE>::top() const
{
    if(empty())
        return at(0);

    return at(idxTop-1);
}

/**
 * @brief   Returns a reference to the top element of the Stack
 * @return  lValue reference to the front element
 * @note    The referenced data is not valid if the Stack is empty
 */
template<class T, std::size_t SIZE>
T& Stack<T, SIZE>::top()
{
    if(empty())
        return at(0);

    return at(idxTop-1);
}

/**
 * @brief   Pushes the given element to the top of the Stack
 * @param   value   Reference to the value to be copied
 * @return  true    If the element is pushed successfully
 */
template<class T, std::size_t SIZE>
bool Stack<T, SIZE>::push(const value_type& value)
{
    if(full())
        return false;

    // Copy construct element at the top
    new(data + idxTop) value_type(value);

    ++idxTop;

    return true;
}

/**
 * @brief   Pushes the given element to the top of the Stack
 * @param   value   rValue Reference to the value to be moved
 * @return  true    If the element is pushed successfully
 */
template<class T, std::size_t SIZE>
bool Stack<T, SIZE>::push(value_type&& value)
{
    return emplace(std::move(value));
}

/**
 * @brief   Pushes the given element to the top of the Stack by constructing it in-place
 * @param   args    Arguments for constructing the new element
 * @return  true    If the element is pushed successfully
 */
template<class T, std::size_t SIZE>
template <class... Args>
bool Stack<T, SIZE>::emplace(Args&&... args)
{
    if(full())
        return false;

    new(data + idxTop) value_type(std::forward<Args>(args)...);

    ++idxTop;

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
    at(idxTop-1).~value_type();

    --idxTop;
}

/**
 * @brief Swaps the content of two Stacks
 * @param swapStack     Stack to be swapped with
 */
template<class T, std::size_t SIZE>
void Stack<T, SIZE>::swap(Stack& swapStack)
{
    std::swap(idxTop,   swapStack.idxTop);
    std::swap(data,     swapStack.data);
}

/**
 * @brief   Comparison operator
 * @param   compStack   Stack to be compared with.
 * @return  true        If both Stacks are equal.
 */
template<class T, std::size_t SIZE>
bool Stack<T, SIZE>::operator==(const Stack& compStack) const
{
    if(compStack.size() != size())
        return false;

    for(size_type elemIdx = 0; elemIdx < idxTop; ++elemIdx)
        if(compStack.at(elemIdx) != at(elemIdx))
            return false;

    return true;
}

/**
 * @brief   Incomparison operator
 * @param   compStack   Stack to be compared with.
 * @return  true        If Stacks are not equal.
 */
template<class T, std::size_t SIZE>
bool Stack<T, SIZE>::operator!=(const Stack& compStack) const
{
    return !(*this == compStack);
}

/**
 * @brief   Copy assignment operator
 * @param   sourceStack     Stack to be copied from
 * @return  lValue reference to the left Stack to support cascaded operations
 */
template<class T, std::size_t SIZE>
Stack<T, SIZE>& Stack<T, SIZE>::operator=(const Stack& sourceStack) &
{
    while(!empty())
        pop();

    if(!sourceStack.empty())
    {
        for(size_type elemIdx = 0; elemIdx < sourceStack.idxTop; ++elemIdx)
            new(data + elemIdx) value_type(sourceStack.at(elemIdx));

        idxTop  = sourceStack.idxTop;
    }

    return *this;
}
