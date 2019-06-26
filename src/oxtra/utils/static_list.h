#ifndef OXTRA_STATICLIST_H
#define OXTRA_STATICLIST_H

#include <cstdlib>
#include <stdexcept>
#include <algorithm>
#include <string.h>

namespace utils {
	template<class T>
	class StaticList {
	public:
		using Allocator = void* (*)(size_t);

	private:
		T* _buffer;
		size_t _size_left;
		size_t _max_elements;
		Allocator _allocator;

	public:
		StaticList(size_t num_elements, Allocator allocator = [](size_t size) -> void* { return new uint8_t[size]; }) {
			_size_left = num_elements;
			_max_elements = num_elements;

			_allocator = allocator;

			_buffer = static_cast<T*>(_allocator(_max_elements * sizeof(T)));
		}

		/**
		 * Add a new element to the list ensuring that the memory is consecutive from start to the element.
		 * @param start The start of the section. May be null.
		 * @param element The element to add.
		 * @return The (new) start of the section.
		 */
		const T* add(const T* start, const T& element) {
			return add(start, &element, 1);
		}

		/**
		 * Add new elements to the list ensuring that the memory is consecutive from start to the last element.
		 * @param start The start of the section. May be null.
		 * @param elements Pointer to a list of elements to add.
		 * @param num_elements The number of elements to add.
		 * @return The (new) start of the section.
		 */
		const T* add(const T* start, const T* elements, size_t num_elements) {
			const size_t store_count = (start == nullptr) ? 0 : _buffer - start;

			if (num_elements + store_count > _max_elements) {
				throw std::invalid_argument("Number of elements may not be larger than max elements");
			}

			// keep track of where the new start is
			T* actual_start;
			if (store_count > 0) {
				actual_start = const_cast<T*>(start);
			} else {
				actual_start = _buffer;
			}

			if (_size_left < num_elements) {
				actual_start = reallocate(start, store_count);
				_buffer = actual_start + store_count;

				_size_left = _max_elements;
			}

			std::copy(elements, elements + num_elements, _buffer);

			_buffer += num_elements;
			_size_left -= num_elements;

			return actual_start;
		}

		/**
		 * Allocates a new entry inside the static list.
		 * @param start The start of the section. It is ensured that memory is consecutive from start to the new element.
		 * @return A reference to the new entry.
		 */
		T& allocate_entry() {
			if (_size_left <= 0) {
				_buffer = static_cast<T*>(_allocator(_max_elements * sizeof(T)));
				_size_left = _max_elements;
			}

			auto& new_entry = *_buffer;
			_buffer++;
			return new_entry;
		}

	private:
		/**
		 * Create a new buffer copying parts of an existing buffer into the new one.
		 * @param start The start of the buffer that will be used as start for copying.
		 * @param elements The number of elements that are currently stored inside the buffer.
		 * @return The start address of the new buffer. The number of elements have to be added manually.
		 */
		T* reallocate(const T* start, const size_t elements) {
			T* new_buffer = static_cast<T*>(_allocator(_max_elements * sizeof(T)));
			if (elements > 0) {
				std::copy(start, start + elements, new_buffer);
			}

			return new_buffer;
		}
	};
}

#endif