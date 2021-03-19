/**
 *  @file       ArrayContainer.h
 *  @details    A template container class with an underlying array data structure.
 *              This container is designed for embedded devices with limited resources.
 *              Dynamic allocation is not preferred to prevent fragmentation and some other performance related problems.
 *  @author     Caglayan DOKME, caglayandokme@gmail.com
 *  @date       March 19, 2021 -> First release
 *
 *  @note       Feel free to contact for questions, bugs, improvements or any other thing.
 *  @copyright  No copyright.
 */

/** Recursive inclusion preventer **/
#ifndef ARRAY_CONTAINER_H
#define ARRAY_CONTAINER_H

/** Libraries **/
#include <cstddef>	// For size_t
#include <cstring>	// For memcpy

/** Special definitions **/
#if __cplusplus >= 201703l	// If the C++ version is greater or equal to 2017xx
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

/*** Container Class ***/
template<class T, size_t SIZE>
class Array{
public:
	/*** Constructors and Destructors ***/
	Array() noexcept = default;							// Default constructor
	Array(const T& fillValue) noexcept;					// Fill constructor
	Array(const Array& copyArr) noexcept;  				// Copy constructor
    Array(const T* const source, const size_t size);    // Construct with C-Style array

	~Array() = default;

	/*** Iterators ***/
	using iterator 			= T*;
	using const_iterator 	= const T*;

	NODISCARD iterator begin() 					{ return data; 			}
	NODISCARD iterator end() 					{ return data + SIZE; 	}
	NODISCARD const_iterator cbegin() const 	{ return data; 			}
	NODISCARD const_iterator cend() const		{ return data + SIZE; 	}

	/*** Operators ***/
	NODISCARD const T& operator[](const size_t index) const { return data[index]; }
	NODISCARD T& operator[](const size_t index) 			{ return data[index]; }

	/*** Status Checkers ***/
	NODISCARD constexpr size_t getSize() { return size; }

private:
	T data[SIZE];
	const size_t size = SIZE;
};

/**
 * @brief	Fill constructor fills every element with a copy of the given one.
 * @param 	fillValue	Reference value for filling.
 */
template<class T, size_t SIZE>
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
template<class T, size_t SIZE>
Array<T, SIZE>::Array(const Array<T, SIZE>& copyArr) noexcept
{
	memcpy(data, copyArr.data, sizeof(T) * SIZE);
}

/**
 * @brief	Construct the container with the given C-Style array.
 * @param 	source		Source buffer
 * @param 	sourceSize	Number of elements in the source
 * @note	In case of an inequality between size values, the lower one is chosen.
 */
template<class T, size_t SIZE>
Array<T, SIZE>::Array(const T* const source, const size_t sourceSize)
{
	if(source != nullptr)
		memcpy(data, source, sizeof(T) * ((sourceSize < SIZE) ? sourceSize : SIZE));
}

#endif // Recursive inclusion preventer
