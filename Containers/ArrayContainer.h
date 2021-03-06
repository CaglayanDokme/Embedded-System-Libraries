/**
 *  @file       ArrayContainer.h
 *  @details    A template container class with an underlying array data structure.
 *              This container is designed for embedded devices with limited resources.
 *              Dynamic allocation is not preferred to prevent fragmentation and some other performance related problems.
 *              Code size optimizations is handled with helper functions.
 *  @author     Caglayan DOKME, caglayandokme@gmail.com
 *  @date       March 19, 2021 -> First release
 *              March 20, 2021 -> Comparison operators overloaded.
 *                             -> Initializer list constructor added.
 *                             -> Helper functions added to decrease the code size.
 *              March 22, 2021 -> Helper functions removed as the template functions are implicitly declared as inline.
 *                             -> Position based fill operation added.
 *                             -> Rule based fill operation added.
 *              March 23, 2021 -> Type traits added to related functions.
 *                             -> Standard named requirements added to class.
 *              March 26, 2021 -> Exception safety condition changed for assignments and swapping.
 *
 *  @note       Feel free to contact for questions, bugs, improvements or any other thing.
 *  @copyright  No copyright.
 */

/** Recursive inclusion preventer **/
#ifndef ARRAY_CONTAINER_H
#define ARRAY_CONTAINER_H

/** Libraries **/
#include <cstddef>                  // For size_t
#include <initializer_list>         // For initializer_list constructor
#include <type_traits>              // For compile time controls
#include <cassert>                  // For assertions

/** Special definitions **/
#if __cplusplus >= 201703l          // If the C++ version is greater or equal to 2017xx
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

/*** Container Class ***/
template<class T, std::size_t SIZE>
class Array{
    static_assert(SIZE != 0, "Array size cannot be zero!");

public:
    /*** C++ Standard Named Requirements for Containers ***/
    typedef T               value_type;
    typedef std::size_t     size_type;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T*              iterator;
    typedef const T*        const_iterator;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef ptrdiff_t       difference_type;

    /*** Constructors and Destructors ***/
    Array() noexcept(std::is_nothrow_constructible<T>::value) = default;    // Default constructor

    template<class U>    // Fill constructor
    Array(const U& fillValue) noexcept(std::is_nothrow_assignable_v<T&, U>);

    template<class U, std::size_t uSIZE>    // Converting constructor
    Array(const Array<U, uSIZE>& copyArr) noexcept(std::is_nothrow_assignable_v<T&, U>);

    template<class U>   // Construct with C-Style array of any type
    Array(const U* const source, const size_type size) noexcept(std::is_nothrow_assignable_v<T&, U>);

    template<class U>   // Initializer_list constructor
    Array(std::initializer_list<U> initializerList) noexcept(std::is_nothrow_assignable_v<T&, U>);

    ~Array() = default;

    /*** Element Access ***/
    NODISCARD iterator begin() noexcept                 { return data;          }
    NODISCARD const_iterator begin() const noexcept     { return data;          }
    NODISCARD iterator end() noexcept                   { return data + SIZE;   }
    NODISCARD const_iterator end() const noexcept       { return data + SIZE;   }

    NODISCARD const_iterator cbegin() const noexcept    { return data;          }
    NODISCARD const_iterator cend() const noexcept      { return data + SIZE;   }

    NODISCARD reference at(const size_type position)                { assert(position >= SIZE); return data[position]; }
    NODISCARD const_reference at(const size_type position) const    { assert(position >= SIZE); return data[position]; }

    /*** Operators ***/
    NODISCARD const_reference operator[](const size_type index) const     { return data[index]; }    // Subscript for non-assignable reference
    NODISCARD reference operator[](const size_type index)                 { return data[index]; }    // Subscript for assignable reference

    template<class U>    // Compare any kind of arrays
    NODISCARD bool operator==(const Array<U, SIZE>& rightArr) const noexcept;
    template<class U>    // Compare any kind of arrays by unequality
    NODISCARD bool operator!=(const Array<U, SIZE>& rightArr) const noexcept;

    template<class U, size_type uSIZE>    // Copy assignment operator
    Array& operator=(const Array<U, uSIZE>& copyArr) noexcept(std::is_nothrow_assignable_v<T&, U>);

    /*** Operations ***/
    Array& Swap(Array& swapArr) noexcept(std::is_nothrow_swappable_v<T>);

    template<class U>
    Array& Fill(const U& fillValue) noexcept(std::is_nothrow_assignable_v<T&, U>);

    template<class U>
    Array& Fill(const U& fillValue, const size_type startPos, const size_type endPos = SIZE) noexcept(std::is_nothrow_assignable_v<T&, U>);

    template<class U>
    Array& Fill(const U& fillValue, iterator startPos, iterator endPos) noexcept(std::is_nothrow_assignable_v<T&, U>);

    template<class RuleT>
    Array& FillWithRule(const RuleT& predicate);

    /*** Status Checkers ***/
    NODISCARD constexpr size_type max_size() const noexcept         { return SIZE;              }    // Return the maximum possible size
    NODISCARD constexpr size_type size() const noexcept             { return SIZE;              }    // Returns total number of elements
    NODISCARD constexpr size_type sizeRaw() const noexcept          { return SIZE * sizeof(T);  }    // Return actual size in bytes
    NODISCARD constexpr bool empty() const noexcept                 { return (SIZE == 0);       }

private:
    value_type data[SIZE];
};

/**
 * @brief   Fill constructor fills every element with a copy of the given one
 * @param   fillValue    Reference value for filling.
 */
template<class T, std::size_t SIZE>
template<class U>
Array<T, SIZE>::Array(const U& fillValue) noexcept(std::is_nothrow_assignable_v<T&, U>)
{
    for(reference element : *this)
        element = fillValue;
}

/**
 * @brief   Copy constructor copies elements from another array
 * @param   copyArr    Source array
 * @note    The source array can be of different type and size
 * @note    Copy size is determined as the lower one of size attributes
 *
 * @attention Unintentional data loss may occur as the types and sizes might not be the same.
 *            It is the user's responsibility to consider etiher data or precision loss.
 */
template<class T, std::size_t SIZE>
template<class U, std::size_t uSIZE>
Array<T, SIZE>::Array(const Array<U, uSIZE>& copyArr) noexcept(std::is_nothrow_assignable_v<T&, U>)
{
    typename Array<U, uSIZE>::const_iterator it = copyArr.cbegin();

    for(reference element : *this)
    {
        element = *it;

        if(++it == copyArr.cend())
            break;
    }
}

/**
 * @brief   Construct the container with the given C-Style array
 * @param   source        Source buffer
 * @param   sourceSize    Number of elements in the source
 * @note    In case of an inequality between size values, the lower one is chosen
 * @note    noexcept exception specifier is not used due to the possibility
 *          of wrong sourceSize inputs.
 */
template<class T, std::size_t SIZE>
template<class U>
Array<T, SIZE>::Array(const U* const source, const size_type sourceSize) noexcept(std::is_nothrow_assignable_v<T&, U>)
{
    if(source != nullptr)
    {
        for(size_type index = 0; (index < SIZE) && (index < sourceSize); ++ index)
            data[index] = source[index];
    }
}

/**
 * @brief   Constructs the array with brace-enclosed initializer list.
 * @param   initializerList    Source list
 */
template<class T, std::size_t SIZE>
template<class U>
Array<T, SIZE>::Array(std::initializer_list<U> initializerList) noexcept(std::is_nothrow_assignable_v<T&, U>)
{
    size_type index = 0;
    for(const U& element : initializerList)
    {
        data[index++] = element;

        if(index == SIZE)
            break;
    }
}

/**
 * @brief   Comparison operator compares two arrays of different types
 * @param   rightArr    Array of another type to be compared with
 * @return  true         If the elements are equal
 * @note    Comparing arrays of different size is restricted.
 *             A compile time error might occur.
 */
template<class T, std::size_t SIZE>
template<class U>
NODISCARD bool Array<T, SIZE>::operator==(const Array<U, SIZE>& rightArr) const noexcept
{
    if(static_cast<const void*>(this) == static_cast<const void*>(&rightArr))    // Self comparison
        return true;

    typename Array<U, SIZE>::const_iterator itRight = rightArr.cbegin();

    /* Comparing with std::memcmp is not eligible because although the size of
     * individual elements might be unequal (e.g. double(8) and int(4)),
     * their values can be equal (e.g. int(65) = double(65.0))*/
    for(const_reference element : data)
    {
        if(element != *itRight)
            return false;

        ++itRight;
    }

    return true;
}

/**
 * @brief   Comparison operator compares two arrays of different types
 * @param   rightArr    Array of another type to be compared with
 * @return  false         If the elements are equal
 * @note    Comparing arrays of different size is restricted.
 *          A compile time error might occur.
 */
template<class T, std::size_t SIZE>
template<class U>
NODISCARD bool Array<T, SIZE>::operator!=(const Array<U, SIZE>& rightArr) const noexcept
{
    return !(this->operator==(rightArr));
}

/**
 * @brief   Copy assignment operator
 * @param   copyArr    Source array
 * @return  lValue reference to the left array to support cascaded calls.
 */
template<class T, std::size_t SIZE>
template<class U, std::size_t uSIZE>
Array<T, SIZE>& Array<T, SIZE>::operator=(const Array<U, uSIZE>& copyArr) noexcept(std::is_nothrow_assignable_v<T&, U>)
{
    if(static_cast<const void*>(this) == static_cast<const void*>(&copyArr))    // Check self copy
        return *this;

    typename Array<U, SIZE>::const_iterator itRight = copyArr.cbegin();

    for(reference element : *this)
    {
        element = *itRight;

        if(++itRight == copyArr.cend())
            break;
    }

    return *this;
}

/**
 * @brief   Swaps two arrays of the same size and type
 * @param   swapArr Array to be swapped with
 * @return  lValue reference to the left array to support cascaded calls.
 */
template<class T, std::size_t SIZE>
Array<T, SIZE>& Array<T, SIZE>::Swap(Array<T, SIZE>& swapArr) noexcept(std::is_nothrow_swappable_v<T>)
{
    if(&swapArr == this)        // Self swap control
        return *this;

    value_type temp;
    iterator itLeft = begin(), itRight = swapArr.begin();

    for(; itLeft != end(); ++itLeft, ++itRight)
    {
        temp         = *itLeft;
        *itLeft     = *itRight;
        *itRight     = temp;
    }

    return *this;
}

/**
 * @brief   Fills the array with exact copies of the given fill value.
 * @param   fillValue     Reference fill value
 * @return  lValue reference to the left array to support cascaded calls.
 */
template<class T, std::size_t SIZE>
template<class U>
Array<T, SIZE>& Array<T, SIZE>::Fill(const U& fillValue) noexcept(std::is_nothrow_assignable_v<T&, U>)
{
    for(reference element : *this)
        element = fillValue;

    return *this;
}

/**
 * @brief   Fills a sub-range of array with exact copies of the given fill value.
 * @param   fillValue    Reference fill value
 * @param   startPos    Start position for filling
 * @param   endPos        End position for filling(excluded)
 * @return  lValue reference to the left array to support cascaded calls.
 */
template<class T, std::size_t SIZE>
template<class U>
Array<T, SIZE>& Array<T, SIZE>::Fill(const U& fillValue, const size_type startPos, const size_type endPos) noexcept(std::is_nothrow_assignable_v<T&, U>)
{
    for(size_type index = startPos; (index < SIZE) && (index < endPos); ++index)
        data[index] = fillValue;

    return *this;
}

/**
 * @brief   Fills a sub-range of array with exact copies of the given fill value using iterators.
 * @param   fillValue    Reference fill value
 * @param   startPos    Start iterator position for filling
 * @param   endPos        End iterator position for filling(excluded)
 * @return  lValue reference to the left array to support cascaded calls.
 */
template<class T, std::size_t SIZE>
template<class U>
Array<T, SIZE>& Array<T, SIZE>::Fill(const U& fillValue, iterator startPos, iterator endPos) noexcept(std::is_nothrow_assignable_v<T&, U>)
{
    if((startPos < begin()) || (startPos >= end()))    // Manual address input might violate the address range
        return *this;

    if(startPos >= endPos)
        return *this;

    for(iterator it = startPos; (it < end()) && (it < endPos); ++it)
        *it = fillValue;

    return *this;
}

/**
 * @brief   Position based fill operation
 * @param   predicate    Rule for calculating the element value using its position.
 * @return  lValue reference to the left array to support cascaded calls.
 *
 * @note    Example usage for this function:
 *          userArr.Fill([](const std::size_t pos) {return (pos * pos);});
 *
 * @note    For more examples, refer to:
 *          github.com/CaglayanDokme/CPP-Exercises/blob/main/FuncWithLambdaArg.cpp
 */
template<class T, std::size_t SIZE>
template<class RuleT>
Array<T, SIZE>& Array<T, SIZE>::FillWithRule(const RuleT& predicate)
{
    for(size_type index = 0; index < SIZE; ++index)
        data[index] = predicate(index);

    return *this;
}

#endif // Recursive inclusion preventer
