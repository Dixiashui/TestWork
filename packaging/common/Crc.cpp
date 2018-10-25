#include <math.h>
#include "Crc.h"

int Crc::addBit(int remainder, const unsigned char * buffer, int bufLen, int keyLen, int pos)
{
	int filter = 0x01;
	for(int i = 1; i < keyLen; i++)
		filter = (filter<<1) | 0x01;
	if(pos >= (bufLen)*8)
		return ((remainder << 1) & filter);
	else
	{
		int iByte = pos/8;
		int filterBit = 0x01<<(8 - (pos - (iByte*8-1)));
		return (((remainder << 1) & filter)) | (buffer[iByte]&filterBit)>>(8 - (pos - (iByte*8-1)));
	}	
}

int Crc::calcCrc(const unsigned char * buffer, int bufLen, int key, int keyLen)
{
	//CRC size is the key size  minus 1
	int size = keyLen - 1;
	int remainder, tmp;
	int limit = static_cast<int>(pow(2,size)-1);
	int remainderShiftNumber = 0; // if the last remainder is not divisible, we have to add zeros at the end. 
	
	if(keyLen >= 8 && keyLen <=16)
		remainder = (buffer[0] << (keyLen-8)) | (buffer[1] >> 5 );
	else if(keyLen < 8)
		remainder = buffer[0] >> (8-keyLen);
	if(remainder > limit)	
		remainder ^= key;
	tmp = remainder;
	for(int i = keyLen; i < (bufLen*8+size); i++)
	{
		tmp = addBit(tmp, buffer, bufLen, keyLen, i); // add the next bit.
		remainderShiftNumber++;
		if(tmp > limit)
		{
			tmp ^= key;
			remainder = tmp;
			remainderShiftNumber = 0; // this is not the last remainder yet.
		}		
	}
	
	return remainder << remainderShiftNumber;
}

