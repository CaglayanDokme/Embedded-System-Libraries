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
#include <cstddef>

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
	Array() noexcept = default;			// Default constructor
	Array(const T& fillValue) noexcept;	// Fill constructor

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

#endif // Recursive inclusion preventer
