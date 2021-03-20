/**
 *  @file       ArrayContainer.h
 *  @details    A template container class with an underlying array data structure.
 *              This container is designed for embedded devices with limited resources.
 *              Dynamic allocation is not preferred to prevent fragmentation and some other performance related problems.
 *  @author     Caglayan DOKME, caglayandokme@gmail.com
 *  @date       March 19, 2021 	-> First release
 *  			March 20, 2021 	-> Comparison operators overloaded.
 *  							-> Initializer list constructor added.
 *
 *  @note       Feel free to contact for questions, bugs, improvements or any other thing.
 *  @copyright  No copyright.
 */

/** Recursive inclusion preventer **/
#ifndef ARRAY_CONTAINER_H
#define ARRAY_CONTAINER_H

/** Libraries **/
#include <cstddef>			// For size_t
#include <cstring>			// For memcpy, memcmp
#include <initializer_list>	// For initializer_list constructor

/** Special definitions **/
#if __cplusplus >= 201703l	// If the C++ version is greater or equal to 2017xx
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

/*** Template Helper Functions for Container Operations ***/
/**
 * @brief 	Helps comparing two different or equal type of arrays with the same size.
 * @param 	leftData	Left operand of comparison operator
 * @param 	rightData	Right operand of comparison operator
 * @param 	size		Comparison range in number of elements
 * @return	true 		If all elements in the specified are equal
 */
template<class T, class _T>
static bool CompareHelper(const T* leftData, const _T* rightData, const std::size_t size)
{
	if(leftData == reinterpret_cast<const T*>(rightData))	// Self comparison
		return true;

	/* Comparing with std::memcmp is not eligible because although the size of
	 * individual elements might be unequal (e.g. double(8) and int(4)),
	 * their values can be equal (e.g. int(65) = double(65.0))*/
	for(size_t index = 0; index < size; ++index)
		if(leftData[index] != rightData[index])	// Compare each element
			return false;

	return true;
}

/*** Container Class ***/
template<class T, std::size_t SIZE>
class Array{
public:
	/*** Constructors and Destructors ***/
	Array() noexcept = default;											// Default constructor
	Array(const T& fillValue) noexcept;									// Fill constructor
	Array(const Array& copyArr) noexcept;								// Copy constructor
	Array(const T* const source, const std::size_t size) noexcept;		// Construct with C-Style array
	Array(std::initializer_list<T> initializerList);					// Initializer_list constructor

	~Array() = default;

	/*** Iterators ***/
	using iterator 			= T*;
	using const_iterator 	= const T*;

	NODISCARD iterator begin() 					{ return data; 			}
	NODISCARD iterator end() 					{ return data + SIZE; 	}
	NODISCARD const_iterator cbegin() const 	{ return data; 			}
	NODISCARD const_iterator cend() const		{ return data + SIZE; 	}

	/*** Operators ***/
	NODISCARD const T& operator[](const std::size_t index) const { return data[index]; }	// Subscript for non-assignable reference
	NODISCARD T& operator[](const std::size_t index) 			{ return data[index]; }	// Subscript for assignable reference

	template<class _T>	// Compare any kind of arrays
	NODISCARD bool operator==(const Array<_T, SIZE>& rightArr) const noexcept;
	template<class _T>	// Compare any kind of arrays by unequality
	NODISCARD bool operator!=(const Array<_T, SIZE>& rightArr) const noexcept;

	/*** Status Checkers ***/
	NODISCARD constexpr std::size_t getSize() const noexcept		{ return SIZE;				}	// Returns total number of elements
	NODISCARD constexpr std::size_t getSizeRaw() const noexcept	{ return SIZE * sizeof(T);	}	// Return actual size in bytes

private:
	T data[SIZE];
};

/**
 * @brief	Fill constructor fills every element with a copy of the given one.
 * @param 	fillValue	Reference value for filling.
 */
template<class T, std::size_t SIZE>
Array<T, SIZE>::Array(const T& fillValue) noexcept
{
	for(T& element : data)
		element = fillValue;
}

/**
 * @brief	Copy constructor copies elements from another array.
 * @param 	copyArr	Source array.
 * @note	Copying is done via a standard library function to provide special optimizations.
 */
template<class T, std::size_t SIZE>
Array<T, SIZE>::Array(const Array& copyArr) noexcept
{
	memcpy(data, copyArr.data, sizeof(T) * SIZE);
}

/**
 * @brief	Construct the container with the given C-Style array.
 * @param 	source		Source buffer
 * @param 	sourceSize	Number of elements in the source
 * @note	In case of an inequality between size values, the lower one is chosen.
 */
template<class T, std::size_t SIZE>
Array<T, SIZE>::Array(const T* const source, const std::size_t sourceSize)
{
	if(source != nullptr)
		std::memcpy(data, source, sizeof(T) * ((sourceSize < SIZE) ? sourceSize : SIZE));
}

/**
 * @brief Constructs the array with brace-enclosed initializer list.
 * @param initializerList	Source list
 */
template<class T, std::size_t SIZE>
Array<T, SIZE>::Array(std::initializer_list<T> initializerList)
{
	std::size_t index = 0;
	for(const T& element : initializerList)
		data[index++] = element;
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
	return CompareHelper(this->cbegin(), rightArr.cbegin(), SIZE);
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

#endif // Recursive inclusion preventer
