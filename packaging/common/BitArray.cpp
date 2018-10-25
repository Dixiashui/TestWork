#include <string.h>
#include "BitArray.h"

BitArrayRaw::BitArrayRaw()
{
    mSize     = 0;
    mBuffer   = NULL;
    mBitCount = 0;
	mPointer  = 0;
	mwPointer = 0;
	mExceeded = false;
}

// Size in bits
BitArrayRaw::BitArrayRaw(unsigned int Size)
{
    mSize = (Size + BYTE_SIZE - 1)/BYTE_SIZE;
    mBuffer = new unsigned char[mSize];
    mBitCount = Size;
	mPointer = 0;
	mwPointer = 0;
	mExceeded = false;
    clear();
}

// SrcSize in bits
BitArrayRaw::BitArrayRaw(const char *Src, unsigned int SrcSize)
{
    mSize = (SrcSize + BYTE_SIZE - 1)/BYTE_SIZE;
    mBuffer = new unsigned char[mSize];
    mBitCount = SrcSize;
    mPointer = 0;
	mwPointer = SrcSize;
	
	memcpy(mBuffer, Src, mSize);
}

BitArrayRaw::~BitArrayRaw()
{
    if (mBuffer != NULL)
        delete []mBuffer;
}

BitArray::BitArray()
{
}

BitArray::BitArray(unsigned int Size) : BitArrayRaw(Size)
{
}

BitArray::BitArray(const char *Src, unsigned int SrcSize) : BitArrayRaw(Src, SrcSize)
{
}

BitArray::~BitArray()
{
}

BitArrayLE::BitArrayLE()
{
}

BitArrayLE::BitArrayLE(unsigned int Size) : BitArrayRaw(Size)
{
}

BitArrayLE::BitArrayLE(const char *Src, unsigned int SrcSize) : BitArrayRaw(Src, SrcSize)
{
}

BitArrayLE::~BitArrayLE()
{
}
void CyclicBitArray::writeBytesToBuffer(const byte* Src, unsigned int SrcSize)
{
    if(this->isPosByteAligned(this->mwPointer))
        this->writeBuffer(Src,SrcSize);
    else
        this->copyDataBitByBit((const void*)Src, Util::getBitSizeFromByteSize(SrcSize));    
}

unsigned int CyclicBitArray::readBytesFromBuffer(byte* dest, unsigned int size)
{
    if(Util::getBitSizeFromByteSize(size) > this->getUsedBitSize())
        size = this->getUsedBitSize() / BYTE_SIZE;
    for(unsigned int i = 0; i < size ; i++)
    {
        dest[i] = this->castBYTE();
    }
    return size;
}
//MSB first
unsigned char CyclicBitArray::castBYTE()
{
    byte ret = 0;
    for(unsigned int i = 0; i < Util::getBitSizeFromByteSize(sizeof(unsigned char)); i++)
    {
        ret |= getBit(0) << CALC_BIT_POS(Util::getBitSizeFromByteSize(sizeof(unsigned char)), i);
        this->shiftPointer((int &)mPointer, 1);
    }
    return ret;
}

bool CyclicBitArray::getBit(unsigned int Pos)
{
    if(Pos >= this->getUsedBitSize())
        this->mExceeded = true;
    
    Pos = (this->mPointer + Pos) % this->mBitCount;
    return GET_BIT(mBuffer, Pos);
}

void CyclicBitArray::shiftPointer(int& p, int offset)
{
    if(offset >= 0)
        Util::increaseWithModulus(p, this->mBitCount, offset);
    else
        Util::decreaseWithModulus(p, this->mBitCount, -offset);
}

bool CyclicBitArray::copyDataBitByBit(const void* Src, unsigned int SrcSize, int pos)
{
  	mExceeded = false;
        if (pos >= 0)
            this->setwPointer((unsigned int)pos);
        
        for(unsigned int i = 0; i < SrcSize; i++)
        {
                if(GET_BIT(static_cast<const unsigned char*>(Src), i)) setBit(mwPointer);
                else clearBit(mwPointer);
                this->shiftPointer((int &)mwPointer, 1);
                if(this->mwPointer == this->mPointer)
                    mExceeded = true;
        }     
        return !mExceeded;
}



void CyclicBitArray::writeBuffer(const byte* Src, unsigned int SrcSize)
{
    this->mExceeded = false;
    if (Util::getBitSizeFromByteSize(SrcSize) > this->getUnusedBitSize())
        this->mExceeded = true;
    unsigned int byteIndex = this->getByteIndexFromBitIndex(this->mwPointer);
    if(SrcSize <= (this->mSize - byteIndex))
    {
        memcpy(this->mBuffer + byteIndex, Src, SrcSize);
    }
    else
    {
        memcpy(this->mBuffer + byteIndex, Src, this->mSize - byteIndex);
        memcpy(this->mBuffer, Src + this->mSize - byteIndex, SrcSize -(this->mSize - byteIndex));
    }
    this->shiftPointer((int &)this->mwPointer, Util::getBitSizeFromByteSize(SrcSize));
}

unsigned int CyclicBitArray::getUnusedBitSize()
{
    if(this->mwPointer >= this->mPointer)
        return this->mBitCount - (this->mwPointer - this->mPointer);
    return this->mPointer - this->mwPointer;
}

void BitArrayRaw::clear()
{
    memset(mBuffer, 0, mSize);
}
    
void BitArrayRaw::attach(unsigned char *Src, unsigned int SrcSize)
{
    mSize = (SrcSize + BYTE_SIZE - 1)/BYTE_SIZE;
    mBuffer = Src;
    mBitCount = SrcSize;
    mPointer = 0;
    mwPointer = SrcSize;
}

unsigned char * BitArrayRaw::detach()
{
    unsigned char *b;

    b = mBuffer;
    
    mSize     = 0;
    mBuffer   = NULL;
    mBitCount = 0;
    mPointer  = 0;
    mwPointer = 0;
    mExceeded = false;

    return b;
}

// the pointer will always signal the end of the bit buffer (after last bit).
// the pointer acts like an index in the bit buffer.
// but all read/write operations move the pointer, so it should always be saved
// and then re-set after the desired operation is done.
unsigned int BitArrayRaw::getPointer() const
{
	return this->mPointer;
}

void BitArrayRaw::setPointer(unsigned int Pos)
{
	this->mPointer = Pos;
}

void BitArrayRaw::shiftPointer(int offset)
{
    if((int)this->mPointer + offset < 0)
        this->setPointer(0);
    else if((int)this->mPointer + offset >= this->mBitCount)
        this->setPointer(this->mBitCount - 1);
    else
        this->setPointer((unsigned int)((int)this->mPointer + offset));
}
// the write pointer will always signal the end of the bit buffer (after last bit).
// acts like an index.
unsigned int BitArrayRaw::getwPointer() const
{
	return this->mwPointer;
}

void BitArrayRaw::setwPointer(unsigned int Pos)
{
	this->mwPointer = Pos;
}

const unsigned char* BitArrayRaw::getBuffer() const
{
	return this->mBuffer;
}

unsigned int BitArrayRaw::getBufferUsedSize() const
{
	return (mwPointer + BYTE_SIZE - 1)/BYTE_SIZE;
}

unsigned int BitArrayRaw::getBitCount() const
{
	return mBitCount;
}

unsigned int BitArrayRaw::getBufferSize() const
{
	return mSize;
}

//void BitArrayRaw::setBufferSize(unsigned int Size)
//{
//	mSize = Size;
//}

bool BitArrayRaw::eof() const
{
	return (mPointer >= mBitCount);
}

bool BitArrayRaw::weof() const
{
	return (mwPointer >= mBitCount);
}

bool BitArrayRaw::exceeded() const
{
	return mExceeded;
}

////Start to set bit from the beginning if Index exceed buffer size
////Index value increases when return
//void BitArrayRaw::setBitWrapped(unsigned int &Index, unsigned int Size)
//{
//	if (Index < Size)
//	{
//		this->setBit(Index);
//		Index ++;
//	}
//	else
//	{
//		Index -= Size;
//		this->setBit(Index - Size);
//		Index ++;
//	}
//}

// should throw bad range exceptions
// bit0 is the LSB in all of these functions (in a little endian system :P)
void BitArrayRaw::setBit(unsigned int Pos)
{
	mExceeded = false;
	
    if(Pos >= 0 && Pos < mBitCount)
        mBuffer[Pos / BYTE_SIZE] |= MASK(BYTE_SIZE, Pos);
    else
    	mExceeded = true;
}

void BitArrayRaw::invertBit(unsigned int Pos)
{
	mExceeded = false;
	
	if(Pos >= 0 && Pos < mBitCount)
		mBuffer[Pos / BYTE_SIZE] ^= MASK(BYTE_SIZE, Pos);
	else
    	mExceeded = true;
}

////Start to clear bit from the beginning if Index exceed buffer size
////Index value increases when return
//void BitArrayRaw::clearBitWrapped(unsigned int &Index, unsigned int Size)
//{
//	if (Index < Size)
//	{
//		this->clearBit(Index);
//		Index ++;
//	}
//	else
//	{
//		Index -= Size;
//		this->clearBit(Index - Size);
//		Index ++;
//	}
//}

// deveria lan�ar excecao com bad range
void BitArrayRaw::clearBit(unsigned int Pos)
{
	mExceeded = false;
	
    if(Pos >= 0 && Pos < mBitCount)
        mBuffer[Pos / BYTE_SIZE] &= ~MASK(BYTE_SIZE, Pos);
    else
    	mExceeded = true;
}

void BitArrayRaw::forceBit(unsigned int Pos, bool flag)
{
	mExceeded = false;
	
    if(Pos >= 0 && Pos < mBitCount)
    {
    	if(flag)
    		mBuffer[Pos / BYTE_SIZE] |= MASK(BYTE_SIZE, Pos);
    	else
        	mBuffer[Pos / BYTE_SIZE] &= ~MASK(BYTE_SIZE, Pos);
    }
    else
    	mExceeded = true;
}

////Start to get bit from the beginning if Index exceed buffer size
//bool BitArrayRaw::getBitWrapped(unsigned int Index, unsigned int Size)
//{
//	if (Index < Size)
//	{
//		return this->getBit(Index);
//	}
//	else
//	{
//		return this->getBit(Index - Size);
//	}
//}

// deveria lan�ar excecao com bad range
bool BitArrayRaw::getBit(unsigned int Pos)
{
	mExceeded = false;
    	
    // returns 1 or 0, instead of mask or zero
    if(Pos >= 0 && Pos < mBitCount)
    	return GET_BIT(mBuffer, Pos);
	else
    	mExceeded = true;
    
	return 0;
}

unsigned int BitArrayRaw::castDWORD()
{
	mExceeded = false;
	
	unsigned int Size = sizeof(unsigned int) * BYTE_SIZE;
	if(Size + mPointer <= mBitCount)
	{
		unsigned int tmp = 0;
		
		for(unsigned int i = 0; i < Size; i++)
		{
			tmp |= getBit(mPointer) << CALC_BIT_POS(Size, i);
			mPointer++;
		}
		
		return tmp;
	}
	else
    	mExceeded = true;
	
	return 0;
}

// reads a dword of Size bits (maximum of 32) from the buffer
unsigned int BitArrayRaw::castDWORD(unsigned int Size)
{
	mExceeded = false;
	
	if(Size + mPointer <= mBitCount)
	{
		unsigned int tmp = 0;
		
		for(unsigned int i = 0; i < Size; i++)
		{
			tmp |= getBit(mPointer) << CALC_BIT_POS(Size, i);
			mPointer++;
		}
		
		return tmp;
	}
	else
    	mExceeded = true;
	
	return 0;
}

unsigned short int BitArrayRaw::castWORD()
{
	mExceeded = false;
	
	unsigned int Size = sizeof(unsigned short int) * BYTE_SIZE;
	if(Size + mPointer <= mBitCount)
	{
		unsigned short int tmp = 0;
		
		for(unsigned int i = 0; i < Size; i++)
		{
			tmp |= getBit(mPointer) << CALC_BIT_POS(Size, i);
			mPointer++;
		}
		
		return tmp;
	}
	else
    	mExceeded = true;
	
	return 0;
}

unsigned char BitArrayRaw::castBYTE()
{
	mExceeded = false;
	
	unsigned int Size = sizeof(unsigned char) * BYTE_SIZE;
	if(Size + mPointer <= mBitCount)
	{
		unsigned char tmp = 0;
		
		for(unsigned int i = 0; i < Size; i++)
		{
			tmp |= getBit(mPointer) << CALC_BIT_POS(Size, i);
			mPointer++;
		}
		
		return tmp;
	}
	else
    	mExceeded = true;
	
	return 0;
}

unsigned char BitArrayRaw::castByteAt(unsigned int pos)
{
    if(pos < mBitCount)
        this->setPointer(pos);
    return this->castBYTE();
}

void BitArrayRaw::copyDataBitByBitBackwards(BitArrayRaw& src, unsigned int srcStart, unsigned int srcEnd, unsigned int end)
{
    if (end >= mBitCount)
        end = mBitCount;
    for (unsigned int i = srcEnd - 1, j = end - 1; i >= srcStart && j >= 0; i--, j--)
    {
        if (src.getBit(i)) setBit(j);
        else clearBit(j);
    }
}
void BitArrayRaw::copyDataBitByBit(BitArrayRaw & src, unsigned int srcStart, unsigned int srcEnd, unsigned int start, unsigned int end)
{
	for(unsigned int i = srcStart, j = start; i < srcEnd && j < mBitCount; i++, j++)
	{
		if(src.getBit(i)) setBit(j);
		else clearBit(j);
	}
}

//void BitArrayRaw::copyDataBitByBit(BitArrayRaw * src, unsigned int srcStart, unsigned int srcEnd, unsigned int start, unsigned int end)
//{
//	for(unsigned int i = srcStart, j = start; i < srcEnd && j < mBitCount; i++, j++)
//	{
//		if(src->getBit(i)) setBit(j);
//		else clearBit(j);
//	}
//}

bool BitArrayRaw::copyDataBitByBit(const void *Src, unsigned int SrcSize, int pos)
{
	mExceeded = false;
        if (pos >= 0)
            this->setwPointer((unsigned int)pos);
	if(SrcSize + mwPointer <= mBitCount && Src != 0 && SrcSize != 0)
	{
		for(unsigned int i = 0; i < SrcSize; i++)
		{
			if(GET_BIT(static_cast<const unsigned char*>(Src), i)) setBit(mwPointer);
			else clearBit(mwPointer);

			mwPointer++;
		}

		return true;
	}
	else
    	mExceeded = true;

	return false;
}

void BitArrayRaw::rightShiftBits(unsigned int startIdx, unsigned int endIdx, unsigned int offset)
{
    if(endIdx + offset >= mBitCount)
    {
        endIdx -= (endIdx + offset - mBitCount) + 1;
    }
    this->copyDataBitByBitBackwards((*this), startIdx, endIdx + 1, endIdx + offset + 1);
}

void BitArrayRaw::leftShiftBits(unsigned int startIdx, unsigned int endIdx, unsigned int offset)
{
    if(startIdx < offset)
    {
        startIdx += (offset - startIdx);
    }
        
    this->copyDataBitByBit((*this), startIdx, endIdx + 1, startIdx - offset);
}

// for non pointer types. SrcSize in bits
// different from the rest of the class, expects the data to start at the LSB
bool BitArrayRaw::copyData(const void *Src, unsigned int SrcSize)
{
	mExceeded = false;
	
	if(SrcSize + mwPointer <= mBitCount && Src != 0 && SrcSize != 0)
	{
		// caso o ultimo byte do buffer nao esteja completo, faz uma copia bit a bit
		// para completa-lo
		unsigned int BitsToCopy = BYTE_SIZE - (mwPointer % BYTE_SIZE);
		if(BitsToCopy > 0)
		{
			copyDataBitByBit(Src, BitsToCopy);
			SrcSize -= BitsToCopy;
		}

		// copia como bytes apos o bit buffer ter sido completo
		unsigned int BytesToCopy = SrcSize/BYTE_SIZE;
		if(BytesToCopy > 0)
		{
			memcpy(mBuffer + mwPointer, Src, BytesToCopy);
			mwPointer += BytesToCopy * BYTE_SIZE;
			SrcSize -= BytesToCopy * BYTE_SIZE;
		}

		// se houver bits restantes apos o memcpy, copia-os bit a bit
		if(SrcSize > 0)
		{
			copyDataBitByBit(Src, SrcSize);
			mwPointer += SrcSize;
		}

		return true;
	}
	else
    	mExceeded = true;

	return false;
}
std::string BitArrayRaw::getBitsStrAt(unsigned int begin, unsigned int length)
{
    char framebits[length + 1];
    framebits[length] = '\0';
    for (unsigned int i = 0; i < length; i++)
    {
        if (this->getBit(begin + i))
            framebits[i] = '1';
        else
            framebits[i] = '0';
    }
    
    std::string str = std::string(framebits);
    return str;
}