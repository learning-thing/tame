#pragma once
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <utility>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

template<typename T> class tArray {
	size_t _capacity;
	size_t _size = 0;
	T *_array;
	size_t *map;//Array of pointers to actual values
	size_t resizeCount = 0;
	bool wasAnyRemoved = false;
	size_t emptySpot = 0;
	protected:
		unsigned int calcSizeIncrease() {
			return _capacity*2;
		}
		size_t smallesAvailablePointer() {
			//find the smallest unused spot
			size_t smallest = size();
			if (wasAnyRemoved) {
				if (emptySpot != 0) {
					smallest = emptySpot-1;
					emptySpot = 0;
				}
			}
			//std::cout << "Empty spot is: " << emptySpot << "\n";
			return MAX(smallest, 0);
		}
		void autoResize() {
			//if we hit max capacity, increase it
			if (_size == _capacity) {
				size_t newSize = _capacity+(1+calcSizeIncrease());
				//printf("At max capacity: resizing to %llu\n", newSize);

				T* newArray = new T[newSize];
				size_t* newMap = new size_t[newSize];

				for (size_t i = 0; i < _size; i++) {
					newArray[i] = std::move(_array[i]);
					newMap[i] = map[i];
				}

				delete[] _array;
				delete[] map;

				_array = newArray;
				map = newMap;
				_capacity = newSize;
				resizeCount++;
			}
		}
	public:
		tArray(size_t maxSize) : _capacity(maxSize) {
			_array = new T[_capacity];
			map = new size_t[_capacity];
		}
		~tArray() {
			delete[] _array;
			delete[] map;
		}
		void pushBack(T d) {
			//find the smallest unused spot
			autoResize();
			size_t smallest = smallesAvailablePointer();
			//printContent();
			//std::cout << "Smallest free spot determined to be: " << smallest << "\n";
			//Add the new locaiton to the list
			map[_size] = smallest;
			//Set the value in the actual array
			_array[smallest] = d;
			_size++;
		}
		//empty the list
		void clear() {
			memset(map, 0, size());//Actually just forgets the mappig
			_size = 0;
		}
		//Swap two items in the list by position
		void swap(size_t positionA, size_t positionB) {
			//Simply swap the locations in the list
			std::swap(map[positionA], map[positionB]);
		}
		//Add item to the very front
		void pushFront(T item) {
			size_t smallest = smallesAvailablePointer();
			//printf("Smallest available: %llu\n", smallest);
			_size++;
			autoResize();
			for (int i = size(); i > 1; i--) map[i] = map[i-1];//shift the map

			map[1] = map[0];
			map[0] = smallest;
			//printMap();
			_array[map[0]] = item;
		}
		void pop_front() {
			remove(0);
		}
		void pop_back() {
			_size--;
			wasAnyRemoved = true;
		}
		void remove(size_t pos) {
			emptySpot = map[pos] +1;
			for (int i = pos; i < size()-1; i++) { map[i] = map[i+1]; }
			_size--;
			wasAnyRemoved = true;
		}
		inline T& operator[](size_t i) {
			assert(i < _size);
			return _array[map[i]];
		}
		bool empty() { return _size == 0; }
		size_t size() { return _size; }
		/** @briefPrint the (internally mapped) content
		*/
		size_t capacity() { return _capacity; }
		//Print content (mapped)
		void printContent() {
			std::cout << "Mapped content: [";
			for (int i = 0; i < size(); i++) {
				std::cout << _array[map[i]] << ((i < size()-1) ? ", " : "");
			}
			std::cout << "]" << "\n";
		}
		//Print the map mapping the content
		void printMap() {
			std::cout << "Map:            [";
			for (int i = 0; i < size(); i++) {
				std::cout << map[i] << ((i < size()-1) ? ", " : "");
			}
			std::cout << "]" << "\n";

		}
		//print the raw content of the array
		void printunMapped() const {
			std::cout << "Unmapped:       [";
			for (int i = 0; i < size(); i++) {
				std::cout << _array[i] << ((i < size()-1) ? ", " : "");
			}
			std::cout << "]" << "\n";

		}
		size_t reAllocCount() const {
			return resizeCount;
		}
};
