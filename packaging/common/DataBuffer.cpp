#include "DataBuffer.h"

#define max(A,B) (((A)>(B))?A:B)
#define min(A,B) (((A)<(B))?A:B)

/**
 * @brief The constructor.
 *
 * @param buffer_size The internal buffer's size
 */
DataBuffer::DataBuffer(int buffer_size)
{
	this->size = buffer_size;
	this->buffer = new byte [buffer_size];
	this->start_offset = 0;
	this->end_offset = 0;
}

/**
 *  @brief The destructor.
 */
DataBuffer::~DataBuffer()
{
	delete [] this->buffer;
    this->buffer = NULL;
}

/**
 * @brief Method used to get the amount of space used in the internal
 * buffer.
 *
 * @returns Returns the amount of bytes used in the buffer
 */
unsigned int DataBuffer::DataSize()
{
	if (this->end_offset >= this->start_offset)
		return this->end_offset - this->start_offset;
	else
		return (this->size - this->start_offset) + (this->end_offset); 	
}

/**
 *  @brief Given an offset, calculates its real position in the internal
 *  buffer.
 *
 * @param relative_offset The offset value, relative to the "buffer's"
 * beginning (from start_offset)
 *
 * @returns The real offset position.
 */
int DataBuffer::GetRealOffset(int relative_offset)
{
	return (this->start_offset+relative_offset)%this->size;
}

/**
 * @brief Retrieves data from the buffer.
 *
 * Retrieves data from the buffer and store in destiny. When a call to Read
 * is issued, the buffer's internal start_offset pointer is moved to skip
 * the read data.
 * 
 * @param destiny The array on which the read data should be stored.
 * @param max_size The amount of bytes to read.
 * 
 * @returns The actual amount of read bytes.
 */
int DataBuffer::Read(byte *destiny, unsigned int max_size) 
{
		
	// amount of bytes to copy
	int amount_copy = min(this->DataSize(),max_size);
//        LogToFile::writeIntln("databuf","amount_copy = ", amount_copy);
	
	// where the copy starts
	byte *begin;
	
	// amount of bytes to copy from the beginning of the buffer
	int bytes_from_start;
		
	// because the buffer is cyclic, the beginning will
	// start from the beginning of the buffer, if needed
	begin = this->buffer + this->start_offset % this->size;
	
	if (begin+amount_copy > this->buffer+this->size)
		// if the amount of bytes to copy exceeds the buffer 
		// size, there are bytes to copy from the beginning
		bytes_from_start = begin+amount_copy - (this->buffer+this->size);
	else
		bytes_from_start = 0;
//        LogToFile::writeIntln("databuf","bytes_from_start = ", bytes_from_start);
		 
	// copy the data slices
//        LogToFile::writeIntln("databuf","amount_copy-bytes_from_start = ", amount_copy-bytes_from_start);
	memcpy(destiny, begin, amount_copy-bytes_from_start);
	memcpy(destiny+amount_copy-bytes_from_start, buffer, bytes_from_start);
	
	this->MoveStartOffset(amount_copy);
	
	return amount_copy; 		
}

/**
 * @brief Clear the buffer.
 *
 * Discards the data in the buffer and reset its internal pointers.
 */
void DataBuffer::Clear()
{
	this->start_offset = this->end_offset = 0;
}

/**
 * @brief Moves the start_offset pointer by offset bytes.
 *
 * Moves the internal start_offset pointer, taking in consideration the fact
 * that the buffer is cyclic.
 *
 * @param offset The amount of bytes by which the pointer should be moved.
 *
 * @returns The final start_offset position.
 */
int DataBuffer::MoveStartOffset(int offset)
{
	this->start_offset = (this->start_offset + offset) % this->size;
    return this->start_offset;
}

/**
 * @brief Writes data into the buffer.
 *
 * Writes data into the end of the buffer.
 *
 * @param source The data to write
 * @param source_size The source size, in bytes
 * 
 * @returns The actual amount of written data.
 * @throws When there's not enough space, the DataBuffer will throw an
 * 	exception "Buffer Overflow".
 *
 * @todo Create an Exception class instead of throwing a string.
 */
int DataBuffer::Write(byte *source, unsigned int source_size) 
{
	// the remaining space on the buffer is the size of it - the
	// amount of bytes actually used
	unsigned int remaining_space = this->size - this->DataSize(); 
	
	// ensures that will not copy more than the available space
	unsigned int amount_copy = min(remaining_space,source_size);
	unsigned int bytes_from_end, bytes_from_start;

	if (amount_copy != source_size) {
            log.log(LOG_TW_ERRO,"DataBuffer::Write Buffer overflow amount_copy= %d, source_size=%d", 
						amount_copy, source_size);
		throw "Buffer overflow";
	}
	
	if ((this->size-this->end_offset) > amount_copy) {
		bytes_from_end = amount_copy;
		bytes_from_start = 0;
	}
	else
        {
		bytes_from_end = this->size-this->end_offset;
		bytes_from_start =  amount_copy-bytes_from_end;
	}
	
	// copy the data slices	 
	memcpy(this->buffer+this->end_offset, source, bytes_from_end);
	memcpy(this->buffer, source+bytes_from_end, bytes_from_start);
	
	// changes the offset
	this->end_offset = (this->end_offset + amount_copy) % this->size;
	
	// return the amount of bytes copied
	return amount_copy;	
}
