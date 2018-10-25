#ifndef DATABUFFER_H_
#define DATABUFFER_H_

#include <common/commonDefinitions.h>
#include <string.h>

#include "TwLog.h"

/**
 * @brief An implementation of a cyclic buffer.
 *
 * This class implements a cyclic buffer with an array of fixed size. This
 * class is used to facilitate communication in a consumer/producer model.
 *
 */
class DataBuffer
{
public:
	DataBuffer(int buffer_size);
	virtual ~DataBuffer();
	
	// returns the offset of a listener who just read the buffer
	int GetRealOffset(int amount_copy);
	
	// reads the buffer
	int Read(byte *destiny, unsigned int max_size);
	// writes to buffer
	int Write(byte *source, unsigned int source_size);
	// discards everything and reset the buffer's offsets
	void Clear();
	
	// "discards" previous data, moving the start offset
	int MoveStartOffset(int offset);
	
	/**
	 * @brief returns the offset of the buffer's end
	 */
	int GetEndOffset() {
		return this->end_offset;
	};
	
        int GetStartOffset(){
            return this->start_offset;
        }
	
	// returns the amount of bytes actually used on the buffer
	unsigned int DataSize(); 		
private:
	/// size of the buffer
	unsigned int size;
	/// buffer's start
	unsigned int start_offset;
	/// buffer's end
	unsigned int end_offset;
	/// buffer
	byte *buffer;
        TwLog log;
};

#endif /*DATABUFFER_H_*/
