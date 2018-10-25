#ifndef __COMMON__DYNAMIC__ARRAY__H__
#define __COMMON__DYNAMIC__ARRAY__H__

#include <stdlib.h>

template <class T>
class DynamicArray {
	public:
		T * _buffer;
		int    _capacity;
		int    _size;
		int    _growRate;
		DynamicArray(int cap, int growRate) {
			_capacity = cap;
			_buffer = (T*)calloc(cap, sizeof(T));
			_growRate = growRate;
			_size = 0;
		}
		~DynamicArray() {
			if (_buffer != NULL) {
				free(_buffer);
				_buffer = NULL;
			}
		}
		void append(T * buf, int len) {
			if (_size + len >= _capacity) {
				_capacity = _size + len + _growRate;
				T * tmp_buffer =
					(T*)realloc(_buffer, _capacity * sizeof(T));
				if (tmp_buffer != NULL)
					_buffer = tmp_buffer;
			}
			memcpy(_buffer+_size, buf, len);
			_size += len;
		}
		void release() {
			if (_buffer != NULL) {
				free(_buffer);
				_buffer = NULL;
			}
		}

		T * getBuffer() {
			return _buffer;
		}
		int getSize() {
			return _size;
		}
};

#endif
