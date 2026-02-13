#pragma once
#include <cassert>
#include <cstddef>
#include <vector>

/*
 * Ultimate dynamic Array:
 * Any type
 * O+2 access time
 * Arena Allocated as far as possible
 */

 /*
  * Indexes:
  * 0: unused
  * >0: position for this id
  */

template<typename T> class tArray {
	T _array[100];
	std::size_t **map;//Array of pointers to actual values
	size_t size = 0;
	size_t capacity = 100;
	protected:
		unsigned int calcSizeIncrease() {
			return capacity*.5;
		}
	public:
		void pushBack(T d) {
			//if we hit max capacity
			if (size >= capacity) {
				//Reallocate
				return;
			}
			_array[size] = d;
			size++;
		}
		//empty the list
		void clear() {

		}
		//Swap too items in the list by position
		void swap(size_t positionA, size_t positionb) {

		}
		//Insert an item at a given position (updating all position values after)
		void insertAt(size_t afterWhat) {

		}
		//Add item to the very front
		void pushFront(T d) {

		}
		//find by value (WARNING: SLOW
		void find(T d) {

		}
		inline T& operator[](size_t i) {
			assert(i < size);
			return _array[map[i % size]];
		}
		bool isEmpty() {
			return size == 0;
		}
};
