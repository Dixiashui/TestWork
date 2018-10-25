#ifndef CRC_H_
#define CRC_H_

class Crc
{
public:
	static int calcCrc(const unsigned char * buffer, int bufLen, int key, int keyLen);
private:
	static int addBit(int remainder, const unsigned char * buffer, int bufLen, int keyLen, int pos);
};

#endif
