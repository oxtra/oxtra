#ifndef OXTRA_FIXED_ARRAY_H
#define OXTRA_FIXED_ARRAY_H

#include <cstdlib>
#include <memory>

namespace utils {

	/**
	 * An array with a fixed size that is known when created at runtime.
	 * @tparam T The type of the elements in the array.
	 */
	template <class T>
	class FixedArray {
	public:
		explicit FixedArray(size_t size)
				: _elements{new T[size]}, _size{size}
		{
			// default construct each object
			for (auto&& obj : *this)
			{
				obj = T{};
			}
		}

		FixedArray(const FixedArray&) = delete;
		FixedArray(FixedArray&&) = delete;

		FixedArray& operator=(const FixedArray&) = delete;
		FixedArray& operator=(FixedArray&&) = delete;

		/**
		 *
		 * @return The size of the array.
		 */
		size_t size() const {
			return _size;
		}

		/**
		 *
		 * @param index The index of the element.
		 * @return The element at the index.
		 */
		const T& operator[](size_t index) const {
			return _elements[index];
		}

		/**
		 *
		 * @param index The index of the element.
		 * @return THe element at the index.
		 */
		T& operator[](size_t index) {
			return _elements[index];
		}

		using iterator = T *;

		/**
		 *
		 * @return The iterator for the beginning.
		 */
		iterator begin() {
			return &_elements[0];
		}

		/**
		 *
		 * @return The iterator for the end.
		 */
		iterator end() {
			return &_elements[_size];
		}

		using const_iterator = const T*;

		/**
		 *
		 * @return The iterator for the beginning.
		 */
		const_iterator begin() const {
			return &_elements[0];
		}

		/**
		 *
		 * @return The iterator for the end.
		 */
		const_iterator end() const {
			return &_elements[_size];
		}

	private:
		std::unique_ptr<T[]> _elements;
		size_t _size;
	};
}

#endif //OXTRA_FIXED_ARRAY_H
