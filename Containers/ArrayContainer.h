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

template<class T, size_t SIZE>
class Array{
public:
	/*** Constructors and Destructors ***/
	Array() = default;
	~Array() = default;

	/*** Iterators ***/
	using iterator 			= T*;
	using const_iterator 	= const T*;

	iterator begin() 				{ return data; 			}
	iterator end() 					{ return data + SIZE; 	}
	const_iterator cbegin() const 	{ return data; 			}
	const_iterator cend() const		{ return data + SIZE; 	}

	/*** Status Checkers ***/
	constexpr size_t getSize() { return size; }

private:
	T data[SIZE];
	const size_t size = SIZE;
};

#endif // Recursive inclusion preventer
