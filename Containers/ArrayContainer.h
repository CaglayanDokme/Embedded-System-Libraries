/**
 *  @file       ArrayContainer.h
 *  @details    A template container class with an underlying array data structure.
 *              This container is designed for embedded devices with limited resources.
 *              Dynamic allocation is not preferred to prevent fragmentation and some other performance related problems.
 *              Code size optimizations is handled with helper functions.
 *  @author     Caglayan DOKME, caglayandokme@gmail.com
 *  @date       March 19, 2021 	-> First release
 *  			March 20, 2021 	-> Comparison operators overloaded.
 *  							-> Initializer list constructor added.
 *  							-> Helper functions added to decrease the code size.
 *  			March 21, 2021 	-> Helper functions removed as the template functions are implicitly declared as inline.
 *  							-> Position based fill operation added.
 *
 *  @note       Feel free to contact for questions, bugs, improvements or any other thing.
 *  @copyright  No copyright.
 */

/** Recursive inclusion preventer **/
#ifndef ARRAY_CONTAINER_H
#define ARRAY_CONTAINER_H

/** Libraries **/
#include <cstddef>			// For size_t
#include <initializer_list>	// For initializer_list constructor

/** Special definitions **/
#if __cplusplus >= 201703l	// If the C++ version is greater or equal to 2017xx
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

/*** Container Class ***/
template<class T, std::size_t SIZE>
class Array{
	static_assert(SIZE != 0, "Array size cannot be zero!");

public:
	/*** Constructors and Destructors ***/
	Array() noexcept = default;											// Default constructor
	Array(const T& fillValue) noexcept;									// Fill constructor

	template<class _T, std::size_t _SIZE>
	Array(const Array<_T, _SIZE>& copyArr) noexcept;					// Copy constructor

	template<class _T>
	Array(const _T* const source, const std::size_t size);				// Construct with C-Style array of any type

	template<class _T>
	Array(std::initializer_list<_T> initializerList);					// Initializer_list constructor

	~Array() = default;

	/*** Iterators ***/
	using iterator 			= T*;
	using const_iterator 	= const T*;

	NODISCARD iterator begin() 					{ return data; 			}
	NODISCARD iterator end() 					{ return data + SIZE; 	}
	NODISCARD const_iterator cbegin() const 	{ return data; 			}
	NODISCARD const_iterator cend() const		{ return data + SIZE; 	}

	/*** Operators ***/
	NODISCARD const T& operator[](const std::size_t index) const 	{ return data[index]; }	// Subscript for non-assignable reference
	NODISCARD T& operator[](const std::size_t index) 				{ return data[index]; }	// Subscript for assignable reference

	template<class _T>	// Compare any kind of arrays
	NODISCARD bool operator==(const Array<_T, SIZE>& rightArr) const noexcept;
	template<class _T>	// Compare any kind of arrays by unequality
	NODISCARD bool operator!=(const Array<_T, SIZE>& rightArr) const noexcept;

	template<class _T, std::size_t _SIZE>	// Copy assignment operator
	Array& operator=(const Array<_T, _SIZE>& copyArr) noexcept;

	/*** Operations ***/
	template<class _T>
	Array& Fill(const _T& fillValue) noexcept;

	template<class _T>
	Array& Fill(const _T& fillValue, const std::size_t startPos, const std::size_t endPos = SIZE) noexcept;

	template<class _T>
	Array& Fill(const _T& fillValue, iterator startPos, iterator endPos = end()) noexcept;

	template<class RuleT>
	Array& FillWithRule(const RuleT& predicate);

	/*** Status Checkers ***/
	NODISCARD constexpr std::size_t getSize() const noexcept		{ return SIZE;				}	// Returns total number of elements
	NODISCARD constexpr std::size_t getSizeRaw() const noexcept	{ return SIZE * sizeof(T);	}	// Return actual size in bytes

private:
	T data[SIZE];
};

/**
 * @brief	Fill constructor fills every element with a copy of the given one
 * @param 	fillValue	Reference value for filling.
 */
template<class T, std::size_t SIZE>
Array<T, SIZE>::Array(const T& fillValue) noexcept
{
	for(T& element : *this)
		element = fillValue;
}

/**
 * @brief	Copy constructor copies elements from another array
 * @param 	copyArr	Source array
 * @note	The source array can be of different type and size
 * @note	Copy size is determined as the lower one of size attributes
 */
template<class T, std::size_t SIZE>
template<class _T, std::size_t _SIZE>
Array<T, SIZE>::Array(const Array<_T, _SIZE>& copyArr) noexcept
{
	typename Array<_T, _SIZE>::const_iterator it = copyArr.cbegin();

	for(T& element : *this)
	{
		element = *it;

		if(it == copyArr.cend())
			break;
	}
}

/**
 * @brief	Construct the container with the given C-Style array
 * @param 	source		Source buffer
 * @param 	sourceSize	Number of elements in the source
 * @note	In case of an inequality between size values, the lower one is chosen
 * @note	noexcept exception specifier is not used due to the possibility
 * 			of wrong sourceSize inputs.
 */
template<class T, std::size_t SIZE>
template<class _T>
Array<T, SIZE>::Array(const _T* const source, const std::size_t sourceSize)
{
	if(source != nullptr)
	{
		for(std::size_t index = 0; (index < SIZE) && (index < sourceSize); ++ index)
			data[index] = source[index];
	}
}

/**
 * @brief Constructs the array with brace-enclosed initializer list.
 * @param initializerList	Source list
 */
template<class T, std::size_t SIZE>
template<class _T>
Array<T, SIZE>::Array(std::initializer_list<_T> initializerList)
{
	std::size_t index = 0;
	for(const _T& element : initializerList)
	{
		data[index++] = element;

		if(index == SIZE)
			break;
	}
}

/**
 * @brief	Comparison operator compares two arrays of different types
 * @param 	rightArr	Array of another type to be compared with
 * @return	true 		If the elements are equal
 * @note	Comparing arrays of different size is restricted.
 * 			A compile time error might occur.
 */
template<class T, std::size_t SIZE>
template<class _T>
NODISCARD bool Array<T, SIZE>::operator==(const Array<_T, SIZE>& rightArr) const noexcept
{
	if(this->cbegin() == reinterpret_cast<const_iterator>(rightArr.cbegin()))	// Self comparison
			return true;

	typename Array<_T, SIZE>::const_iterator itRight = rightArr.cbegin();

	/* Comparing with std::memcmp is not eligible because although the size of
	 * individual elements might be unequal (e.g. double(8) and int(4)),
	 * their values can be equal (e.g. int(65) = double(65.0))*/
	for(const T& element : *this)
	{
		if(element != *itRight)
			return false;

		++itRight;
	}

	return true;
}

/**
 * @brief	Comparison operator compares two arrays of different types
 * @param 	rightArr	Array of another type to be compared with
 * @return	false 		If the elements are equal
 * @note	Comparing arrays of different size is restricted.
 * 			A compile time error might occur.
 */
template<class T, std::size_t SIZE>
template<class _T>
NODISCARD bool Array<T, SIZE>::operator!=(const Array<_T, SIZE>& rightArr) const noexcept
{
	return !(this->operator==(rightArr));
}

/**
 * @brief	Copy assignment operator
 * @param 	copyArr	Source array
 * @return	lValue reference to the left array to support cascaded calls.
 */
template<class T, std::size_t SIZE>
template<class _T, std::size_t _SIZE>
Array<T, SIZE>& Array<T, SIZE>::operator=(const Array<_T, _SIZE>& copyArr) noexcept
{
	if(this->cbegin() == reinterpret_cast<const_iterator>(copyArr.cbegin()))	// Check self copy
		return *this;

	typename Array<_T, SIZE>::const_iterator itRight = copyArr.cbegin();

	for(T& element : *this)
	{
		element = *itRight;
		++itRight;
	}

	return *this;
}

/**
 * @brief	Fills the array with exact copies of the given fill value.
 * @param 	fillValue 	Reference fill value
 * @return	lValue reference to the left array to support cascaded calls.
 */
template<class T, std::size_t SIZE>
template<class _T>
Array<T, SIZE>& Array<T, SIZE>::Fill(const _T& fillValue) noexcept
{
	for(T& element : *this)
		element = fillValue;

	return *this;
}

/**
 * @brief	Fills a sub-range of array with exact copies of the given fill value.
 * @param 	fillValue	Reference fill value
 * @param 	startPos	Start position for filling
 * @param 	endPos		End position for filling(excluded)
 * @return	lValue reference to the left array to support cascaded calls.
 */
template<class T, std::size_t SIZE>
template<class _T>
Array<T, SIZE>& Array<T, SIZE>::Fill(const _T& fillValue, const std::size_t startPos, const std::size_t endPos) noexcept
{
	for(std::size_t index = startPos; (index < SIZE) && (index < endPos); ++index)
		data[index] = fillValue;

	return *this;
}

/**
 * @brief	Fills a sub-range of array with exact copies of the given fill value using iterators.
 * @param 	fillValue	Reference fill value
 * @param 	startPos	Start iterator position for filling
 * @param 	endPos		End iterator position for filling(excluded)
 * @return	lValue reference to the left array to support cascaded calls.
 */
template<class T, std::size_t SIZE>
template<class _T>
Array<T, SIZE>& Array<T, SIZE>::Fill(const _T& fillValue, iterator startPos, iterator endPos) noexcept
{
	if(startPos < begin())	// Manual address input might violate the address range
		return *this;

	if(startPos >= endPos)
		return *this;

	for(iterator it = startPos; (it < end()) && (it < endPos); ++it)
		*it = fillValue;

	return *this;
}

/**
 * @brief	Position based fill operation
 * @param 	predicate	Rule for calculating the element value using its position.
 * @return	lValue reference to the left array to support cascaded calls.
 *
 * @note 	Example usage for this function:
 *          userArr.Fill([](const std::size_t pos) {return (pos * pos);});
 *
 * @note    For more examples, refer to:
 *          github.com/CaglayanDokme/CPP-Exercises/blob/main/FuncWithLambdaArg.cpp
 */
template<class T, std::size_t SIZE>
template<class RuleT>
Array<T, SIZE>& Array<T, SIZE>::FillWithRule(const RuleT& predicate)
{
	for(std::size_t index = 0; index < SIZE; ++index)
		data[index] = predicate(index);

	return *this;
}

#endif // Recursive inclusion preventer
