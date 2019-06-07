#ifndef OXTRA_STATICLIST_H
#define OXTRA_STATICLIST_H

#include <cstdlib>

namespace oxtra {
	template <class T>
	class StaticList {
	private:
		T* _buffer;
		size_t _size_left;

	public:
		StaticList(size_t num_elements);

		/**
		 * Add a new element to the list ensuring that the memory is consecutive from start to the element.
		 * @param start The start of the section. May be null.
		 * @param element The element to add.
		 * @return The (new) start of the section.
		 */
		const T* add(const T* const start, const T& element);
	};
}

#endif