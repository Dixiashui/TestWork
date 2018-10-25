#ifndef BITARRAY_H_INCLUDED
#define BITARRAY_H_INCLUDED

#include <iostream>
#include <math.h>
#include <limits.h>

#include "Util.h"

#undef BIT_0
#undef BIT_1

// uses limit.h's definition
#define BYTE_SIZE (CHAR_BIT)

#define BIT_8 (1 << 7)
#define BIT_7 (1 << 6)
#define BIT_6 (1 << 5)
#define BIT_5 (1 << 4)
#define BIT_4 (1 << 3)
#define BIT_3 (1 << 2)
#define BIT_2 (1 << 1)
#define BIT_1 (1 << 0)

// from MIT
inline unsigned int countBits(unsigned int u)                          
{
    unsigned int uCount = u - ((u >> 1) & 033333333333)	- ((u >> 2) & 011111111111);  
    return ((uCount + (uCount >> 3)) & 030707070707) % 63;
}

// takes the bits at MSB and inverts them with LSB
template<class T>
T invertBits(T u, unsigned int len)
{
	unsigned int tmp = static_cast<unsigned int>(u), tmp2 = 0;
	
	for(unsigned int i = 0; i < len; i++)
	{
		unsigned int bitVal = tmp & (1 << i);
		
		if(bitVal)
			tmp2 |= 1 << (len - 1 - i);
	}
	
	return static_cast<T>(tmp2);
}

class BitArrayRaw
{
public:
    BitArrayRaw();
	// Size - bits
    BitArrayRaw(unsigned int Size);
    // SrcSize - bits
    BitArrayRaw(const char *Src, unsigned int SrcSize);
    virtual ~BitArrayRaw();
    void clear();
    // similar to BitArrayStream, but for a byte buffer
    void attach(unsigned char *Src, unsigned int SrcSize);
    unsigned char *detach();
    // read pointer
	unsigned int getPointer() const;
	void setPointer(unsigned int Pos);
        virtual void shiftPointer(int offset);
	// write pointer
	unsigned int getwPointer() const;
	void setwPointer(unsigned int Pos);
	// i - bit idx (0 being the LSB)
//	void setBitWrapped(unsigned int &Index, unsigned int Size);
    void setBit(unsigned int Pos);
//	void clearBitWrapped(unsigned int &Index, unsigned int Size);
    void clearBit(unsigned int Pos);
	void invertBit(unsigned int Pos);
	void forceBit(unsigned int Pos, bool flag);
        void forceBit(bool flag) 
        {
            forceBit(mwPointer++, flag);
        }
//	bool getBitWrapped(unsigned int Index, unsigned int Size);
    virtual bool getBit(unsigned int Pos);
    // retrieves a 32 bit value from the buffer
    unsigned int castDWORD();
    // retrieves a Size bit value from the buffer (up to 32 bit)
    unsigned int castDWORD(unsigned int Size);
    // retrieves a 16 bit value from the buffer
	unsigned short int castWORD();
	// retrieves a 8 bit value from the buffer
	virtual unsigned char castBYTE();
        unsigned char castByteAt(unsigned int pos);
        // copies backwards (bit(srcEnd-1)  will be copied first, and bit(srcStar) last) from a source bit by bit
	void copyDataBitByBitBackwards(BitArrayRaw & src, unsigned int srcStart, unsigned int srcEnd, unsigned int end);
	// copies a destination bit by bit
	void copyDataBitByBit(BitArrayRaw & src, unsigned int srcStart, unsigned int srcEnd, unsigned int start, unsigned int end = 0);
//	void copyDataBitByBit(BitArrayRaw * src, unsigned int srcStart, unsigned int srcEnd, unsigned int start, unsigned int end);        
	virtual bool copyDataBitByBit(const void *Src, unsigned int SrcSize, int pos = -1);
        // right shift bits from bit(startIdx) to bit(endIdx) by offset
        void rightShiftBits(unsigned int startIdx, unsigned int endIdx, unsigned int offset);
        void leftShiftBits(unsigned int startIdx, unsigned int endIdx, unsigned int offset);
        
	template <class T> bool copyDataBitByBit(T Src, unsigned int SrcSize)
	{
		mExceeded = false;
		
		if(SrcSize + mwPointer <= mBitCount)
		{
			for(unsigned int i = 0; i < SrcSize; ++i)
			{
				if((Src & MASK(SrcSize, i))) mBuffer[mwPointer / BYTE_SIZE] |= MASK(BYTE_SIZE, mwPointer); // setBit
				else mBuffer[mwPointer / BYTE_SIZE] &= ~MASK(BYTE_SIZE, mwPointer); // clearBit

				mwPointer++;
			}

			return true;
		}
		else
    		mExceeded = true;

		return false;
	}
	// copies a destination using a smarter and faster method - NOT TESTED
	bool copyData(const void *Src, unsigned int SrcSize); 
        std::string getBitsStrAt(unsigned int begin, unsigned int length);
	// retrieves the internal buffer
	const unsigned char* getBuffer() const;
	unsigned int getBufferUsedSize() const;
	unsigned int getBitCount() const;
	unsigned int getBufferSize() const;
//	void setBufferSize(unsigned int Size);
	// indicates if the reading or writting pointer has reached or exceeded the limit
	bool eof() const;
	bool weof() const;
	// indicates if a reading or writting operation would exceed the boundaries
	bool exceeded() const;
protected:
	// returns 1 if a bit exists at that location, 0 if not
	virtual inline unsigned int GET_BIT(const unsigned char *buf, unsigned int pos) const { return (buf[pos / BYTE_SIZE] & MASK(BYTE_SIZE, pos)) != 0; };
	// virtual inlines... heh heh... optimization is compiler dependent =P
	virtual unsigned int MASK(unsigned int nbits, unsigned int pos) const = 0;
	virtual unsigned int CALC_BIT_POS(unsigned int storageSize, unsigned int order) const = 0;
//private:
    unsigned char *mBuffer;
    unsigned int mBitCount; // bits in buffer
    unsigned int mSize; // bytes in buffer (buffer size)

    unsigned int mPointer;
    unsigned int mwPointer;
	
    bool mExceeded;
};


class BitArray : public BitArrayRaw
{
public:
	BitArray();
	// Size - bits
    BitArray(unsigned int Size);
    // SrcSize - bits
    BitArray(const char *Src, unsigned int SrcSize);
    virtual ~BitArray();
protected:
	// virtual inlines... heh heh... optimization is compiler dependent =P
	/*
	 * In a structure of N bits, creates a mask for a position
	 * pos=0 is the MSB in the structure of N bits.
	 */
	virtual inline unsigned int MASK(unsigned int nbits, unsigned int pos) const { return (1 << ((nbits - 1) - (pos % nbits))); }
	virtual inline unsigned int CALC_BIT_POS(unsigned int storageSize, unsigned int order) const { return storageSize - order - 1; }
};

class BitArrayLE : public BitArrayRaw
{
public:
	BitArrayLE();
	// Size - bits
    BitArrayLE(unsigned int Size);
    // SrcSize - bits
    BitArrayLE(const char *Src, unsigned int SrcSize);
    virtual ~BitArrayLE();
protected:
	virtual inline unsigned int MASK(unsigned int nbits, unsigned int pos) const { return (1 << (pos % nbits)); }
	virtual inline unsigned int CALC_BIT_POS(unsigned int /*storageSize*/, unsigned int order) const { return order; }
};

// be careful when using inherited functions in this class
// only functions declared here are tested
// If other functions needs to be inherited, 
// please check the implementation in the base class,
// and make it virtual if necessary
class CyclicBitArray : public BitArray
{
public:
    CyclicBitArray():BitArray() {;}
    CyclicBitArray(unsigned int Size):BitArray(Size) {;}
    virtual ~CyclicBitArray() { if(this->mBuffer != NULL) delete[] this->mBuffer;}
    
    void writeBytesToBuffer(const byte *Src, unsigned int SrcSize);
    unsigned int readBytesFromBuffer(byte *dest, unsigned int size);
    unsigned int getUsedBitSize() { return this->mBitCount - this->getUnusedBitSize();}
    void setBitsConsumed(int num) { this->shiftPointer((int &)this->mPointer, num);}

    virtual bool getBit(unsigned int Pos);
    virtual unsigned char castBYTE();
    virtual void shiftPointer(int& p, int offset);
    virtual bool copyDataBitByBit(const void *Src, unsigned int SrcSize, int pos = -1);    
private:    
    unsigned int getUnusedBitSize();
    void writeBuffer(const byte *Src, unsigned int SrcSize);
    bool isPosByteAligned(unsigned int pos) { return pos % BYTE_SIZE == 0; }
    unsigned int getByteIndexFromBitIndex(unsigned int i) { return (i / BYTE_SIZE);}
    
};

/*
 * A bit stream implemented on the bitarray
 * uses the direct reference to a variable.
 * works similarly to the template version of copyDataBitByBit.
 * idx=0 is the MSB.
 */
template <class T>
class BitArrayStream
{
public:
	BitArrayStream(T & ref) : mRef(ref)
	{
		structureSize = sizeof(T) * BYTE_SIZE;
	}
	
	// allows the usage of n bits in any structure (20 bits in a 32 bit data ie.)
	BitArrayStream(T & ref, unsigned int n) : mRef(ref)
	{
		structureSize = n;
	}

	// should throw bad range excep
	bool get(unsigned int idx) const
	{
		if(idx > (structureSize - 1))
			return false;
		
		return (mRef & MASK(structureSize, idx)) != 0;
	}
	
	void set(unsigned int idx)
	{
		if(idx < structureSize)
			mRef |= MASK(structureSize, idx);
	}
	
	void clear(unsigned int idx)
	{
		if(idx < structureSize)
			mRef &= ~MASK(structureSize, idx);
	}
	
	void invert(unsigned int idx)
	{
		if(idx < structureSize)
			mRef ^= MASK(structureSize, idx);
	}
	
	T getValue(unsigned int start, unsigned int end) const
	{		
		if(end < structureSize && start < structureSize)
			return (mRef & MASKPOS(start, end, structureSize)) >> POSATMSB(structureSize, end);
		
		return 0;
	}
	
	// copies (end minus start) bits from Src (right to left)
	template<class Y> bool copyData(unsigned int start, unsigned int end, Y src)
	{
		if(end < structureSize && start < structureSize)
		{
			T srcMask = (src & MASKSIZE(end - start + 1)) << POSATMSB(structureSize, end);
			T zeroMask = mRef & MASKPOS(start, end, structureSize);
			
			// remove the values at the desired location
			mRef -= zeroMask;
			// insert the new ones
			mRef |= srcMask;
		}
		
		return false;
	}
private:
	// bit 0 is always the MSB
	inline unsigned int POSATMSB(unsigned int nbits, unsigned int i) const { return (nbits - 1) - i; }
	inline unsigned int MASKSIZE(unsigned int BitSize) const { return static_cast<T>(pow(2, BitSize)) - 1; }
	inline unsigned int MASKPOS(unsigned int start, unsigned int end, unsigned int nbits) const { return MASKSIZE(end - start + 1) << (POSATMSB(nbits, end)); }
	inline unsigned int MASK(unsigned int nbits, unsigned int pos) const { return (1 << POSATMSB(nbits, pos)); }
private:
	T & mRef;
	unsigned int structureSize;
};

#endif
