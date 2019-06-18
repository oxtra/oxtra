#ifndef OXTRA_STATICLIST_H
#define OXTRA_STATICLIST_H

#include <cstdlib>

namespace utils {
	template<class T>
	class StaticList {
	private:
		T* _buffer;
		size_t _size_left;

	public:
		StaticList(size_t num_elements) {

		}

		/**
		 * Add a new element to the list ensuring that the memory is consecutive from start to the element.
		 * @param start The start of the section. May be null.
		 * @param element The element to add.
		 * @return The (new) start of the section.
		 */
		const T* add(const T* start, const T& element) {
			return nullptr;
		}

		/**
		 * Add new elements to the list ensuring that the memory is consecutive from start to the last element.
		 * @param start The start of the section. May be null.
		 * @param elements Pointer to a list of elements to add.
		 * @param num_elements The number of elements to add.
		 * @return The (new) start of the section.
		 */
		const T* add(const T* start, const T* elements, size_t num_elements) {
			return nullptr;
		}

		/**
		 * Allocates a new entry inside the static list.
		 * @return A reference to the new entry.
		 */
		T& allocate_entry() {

		}
	};
}

#endif