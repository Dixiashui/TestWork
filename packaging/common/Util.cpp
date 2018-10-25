#include "Util.h"
#include <sys/time.h>
#include <stdlib.h>
#include <iomanip>
#include <string>
#include <vector>

/**
 * @brief Converts unsigned int to std::string.
 *
 * @param n Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toString(unsigned int n)
{
    std::ostringstream s;

    s << n;

    return s.str();
}

/**
 * @brief Converts int to std::string.
 *
 * @param n Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toString(int n)
{
    std::ostringstream s;

    s << n;

    return s.str();
}

/**
 * @brief Converts unsigned short to std::string.
 *
 * @param n Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toString(unsigned short n)
{
    std::ostringstream s;

    s << n;

    return s.str();
}

/**
 * @brief Converts short to std::string.
 *
 * @param n Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toString(short n)
{
    std::ostringstream s;

    s << n;

    return s.str();
}

/**
 * @brief Converts unsigned long to std::string.
 *
 * @param n Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toString(unsigned long n)
{
    std::ostringstream s;

    s << n;

    return s.str();
}

/**
 * @brief Converts long to std::string.
 *
 * @param n Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toString(long n)
{
    std::ostringstream s;

    s << n;

    return s.str();
}

/**
 * @brief Converts unsigned char to std::string.
 *
 * @param n Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toString(unsigned char n)
{
    std::ostringstream s;

    s << static_cast<unsigned short> (n);

    return s.str();
}

/**
 * @brief Converts char to std::string.
 *
 * @param n Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toString(char n)
{
    std::ostringstream s;

    s << static_cast<short> (n);

    return s.str();
}

/**
 * @brief Converts bool to std::string.
 *
 * @param n Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toString(bool b)
{
    std::ostringstream s;

    s.setf(std::ios_base::boolalpha);
    s << b;

    return s.str();
}

/**
 * @brief Converts double to std::string.
 *
 * @param n Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toString(double n)
{
    std::ostringstream s;

    s << n;

    return s.str();
}

/**
 * @brief Converts unsigned int to hexadecimal std::string.
 *
 * @param n    Value to be converted.
 * @param fill true fills std::string with zeros on the left, false does not fill.
 *
 * @returns Value as std::string.
 */
std::string Util::toHexString(unsigned int n, bool fill)
{
    std::ostringstream s;

    confHexStream(s, fill, sizeof (n));
    s << n;

    return s.str();
}

/**
 * @brief Converts int to hexadecimal std::string.
 *
 * @param n    Value to be converted.
 * @param fill true fills std::string with zeros on the left, false does not fill.
 *
 * @returns Value as std::string.
 */
std::string Util::toHexString(int n, bool fill)
{
    std::ostringstream s;

    confHexStream(s, fill, sizeof (n));
    s << n;

    return s.str();
}

/**
 * @brief Converts unsigned short to hexadecimal std::string.
 *
 * @param n    Value to be converted.
 * @param fill true fills std::string with zeros on the left, false does not fill.
 *
 * @returns Value as std::string.
 */
std::string Util::toHexString(unsigned short n, bool fill)
{
    std::ostringstream s;

    confHexStream(s, fill, sizeof (n));
    s << n;

    return s.str();
}

/**
 * @brief Converts short to hexadecimal std::string.
 *
 * @param n    Value to be converted.
 * @param fill true fills std::string with zeros on the left, false does not fill.
 *
 * @returns Value as std::string.
 */
std::string Util::toHexString(short n, bool fill)
{
    std::ostringstream s;

    confHexStream(s, fill, sizeof (n));
    s << n;

    return s.str();
}

/**
 * @brief Converts unsigned long to hexadecimal std::string.
 *
 * @param n    Value to be converted.
 * @param fill true fills std::string with zeros on the left, false does not fill.
 *
 * @returns Value as std::string.
 */
std::string Util::toHexString(unsigned long n, bool fill)
{
    std::ostringstream s;

    confHexStream(s, fill, sizeof (n));
    s << n;

    return s.str();
}

/**
 * @brief Converts long to hexadecimal std::string.
 *
 * @param n    Value to be converted.
 * @param fill true fills std::string with zeros on the left, false does not fill.
 *
 * @returns Value as std::string.
 */
std::string Util::toHexString(long n, bool fill)
{
    std::ostringstream s;

    confHexStream(s, fill, sizeof (n));
    s << n;

    return s.str();
}

/**
 * @brief Converts unsigned char to hexadecimal std::string.
 *
 * @param n    Value to be converted.
 * @param fill true fills std::string with zeros on the left, false does not fill.
 *
 * @returns Value as std::string.
 */
std::string Util::toHexString(unsigned char n, bool fill)
{
    std::ostringstream s;

    confHexStream(s, fill, sizeof (n));
    s << static_cast<unsigned short> (n);

    return s.str();
}

/**
 * @brief Converts char to hexadecimal std::string.
 *
 * @param n    Value to be converted.
 * @param fill true fills std::string with zeros on the left, false does not fill.
 *
 * @returns Value as std::string.
 */
std::string Util::toHexString(char n, bool fill)
{
    std::ostringstream s;

    confHexStream(s, fill, sizeof (n));
    s << static_cast<unsigned short> (static_cast<unsigned char> (n));

    return s.str();
}

/**
 * @brief Converts a buffer to hexadecimal std::string.
 * 
 * @param buffer Buffer to be converted.
 * @param len Size of buffer in bytes.
 * 
 * @returns Value as std::string.
 */
std::string Util::toHexString(const unsigned char *buffer, unsigned int len)
{
    std::ostringstream s;

    for (unsigned int i = 0; i < len; i++)
        s << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<unsigned int> (buffer[i]) << " ";

    return s.str();
}

/**
 * @brief Converts unsigned int to binary std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toBinString(unsigned int n)
{
    std::ostringstream s;
    std::bitset<sizeof (n) * 8 > b(n);

    s << b;

    return s.str();
}

/**
 * @brief Converts int to binary std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toBinString(int n)
{
    std::ostringstream s;
    std::bitset<sizeof (n) * 8 > b(n);

    s << b;

    return s.str();
}

/**
 * @brief Converts unsigned short to binary std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toBinString(unsigned short n)
{
    std::ostringstream s;
    std::bitset<sizeof (n) * 8 > b(n);

    s << b;

    return s.str();
}

/**
 * @brief Converts short to binary std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toBinString(short n)
{
    std::ostringstream s;
    std::bitset<sizeof (n) * 8 > b(n);

    s << b;

    return s.str();
}

/**
 * @brief Converts unsigned long to binary std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toBinString(unsigned long n)
{
    std::ostringstream s;
    std::bitset<sizeof (n) * 8 > b(n);

    s << b;

    return s.str();
}

/**
 * @brief Converts long to binary std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toBinString(long n)
{
    std::ostringstream s;
    std::bitset<sizeof (n) * 8 > b(n);

    s << b;

    return s.str();
}

/**
 * @brief Converts unsigned char to binary std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toBinString(unsigned char n)
{
    std::ostringstream s;
    std::bitset<sizeof (n) * 8 > b(n);

    s << b;

    return s.str();
}

/**
 * @brief Converts char to binary std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toBinString(char n)
{
    std::ostringstream s;
    std::bitset<sizeof (n) * 8 > b(n);

    s << b;

    return s.str();
}

/**
 * @brief Converts a buffer to binary std::string.
 * 
 * @param buffer Buffer to be converted.
 * @param len Size of buffer in bytes.
 * 
 * @returns Value as std::string.
 */
std::string Util::toBinString(const unsigned char *buffer, unsigned int len)
{
    std::ostringstream s;

    for (unsigned int i = 0; i < len; i++)
        for (int j = 7; j != -1; j--)
            s << static_cast<unsigned int> ((buffer[i] & (1 << j)) >> j);

    return s.str();
}

/**
 * @brief Converts unsigned int to octal std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toOctString(unsigned int n)
{
    std::ostringstream s;

    confOctStream(s);
    s << n;

    return s.str();
}

/**
 * @brief Converts int to octal std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toOctString(int n)
{
    std::ostringstream s;

    confOctStream(s);
    s << n;

    return s.str();
}

/**
 * @brief Converts unsigned short to octal std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toOctString(unsigned short n)
{
    std::ostringstream s;

    confOctStream(s);
    s << n;

    return s.str();
}

/**
 * @brief Converts short to octal std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toOctString(short n)
{
    std::ostringstream s;

    confOctStream(s);
    s << n;

    return s.str();
}

/**
 * @brief Converts unsigned long to octal std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toOctString(unsigned long n)
{
    std::ostringstream s;

    confOctStream(s);
    s << n;

    return s.str();
}

/**
 * @brief Converts long to octal std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toOctString(long n)
{
    std::ostringstream s;

    confOctStream(s);
    s << n;

    return s.str();
}

/**
 * @brief Converts unsigned char to octal std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toOctString(unsigned char n)
{
    std::ostringstream s;

    confOctStream(s);
    s << static_cast<unsigned short> (n);

    return s.str();
}

/**
 * @brief Converts char to octal std::string.
 *
 * @param n    Value to be converted.
 *
 * @returns Value as std::string.
 */
std::string Util::toOctString(char n)
{
    std::ostringstream s;

    confOctStream(s);
    s << static_cast<unsigned short> (static_cast<unsigned char> (n));

    return s.str();
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//int Util::parseInt(std::string s)
//{
//    std::istringstream stream;
//    int n;

//    stream.setf(std::ios_base::dec, std::ios_base::basefield);
//    stream.str(s);
//    stream >> n;
//    
//    return n;
//}

/**
 * @brief Trims all left chars of the std::string.
 *
 * @param s       String.
 * @param trimset Char set to be cut. 
 */
void Util::ltrim(std::string &s, const std::string &trimset)
{
    s.erase(0, s.find_first_not_of(trimset));
}

/**
 * @brief Trims all right chars of the std::string.
 *
 * @param s       String.
 * @param trimset Char set to be cut.
 */
void Util::rtrim(std::string &s, const std::string &trimset)
{
    s.resize(s.find_last_not_of(trimset) + 1);
}

/**
 * @brief Trims all right and left chars of the std::string.
 *
 * @param s       String.
 * @param trimset Char set to be cut.
 */
void Util::trim(std::string &s, const std::string &trimset)
{
    rtrim(s, trimset);
    ltrim(s, trimset);
}

void Util::decreaseWithModulus(int& v, int m, int offset)
{
    assert(v>=0 && m>=0 && offset>=0);
    v -= offset;
    if (v >= 0)
        v = v % m;
    else
        while (v < 0)
            v += m;
}

bool Util::stringContains(std::string s, std::string dest)
{
    return (s.find(dest) != std::string::npos);
}

bool Util::splitStringToVector(std::string s, char splitter, StringVector& vector)
{
    std::istringstream ss(s);
    std::string ele;
    vector.clear();
    while(std::getline(ss, ele, splitter))
        vector.push_back(ele);
    return !vector.empty();
}

std::string Util::processShellCommand(const char* cmd, int length)
{
    char ret[length];
    memset(ret, 0, length);
    std::string s = std::string(cmd) + " 2>&1";
    FILE *stream = popen(s.c_str(), "r");
    fread(ret, sizeof(char), length, stream);
    pclose(stream);
    return std::string(ret);
}

/**
 * @brief Sleeps for the specified number of milliseconds.
 *
 * @param timeout  Timeout in milliseconds
 */
void Util::milliSleep(long timeout)
{
    struct timespec tv;

    time_t sec = timeout / 1000;
    long msec = timeout % 1000;
    long nsec = (1000000) * msec;

    tv.tv_sec = sec;
    tv.tv_nsec = nsec;

    nanosleep(&tv, NULL);
}

/**
 * @brief Fills the timespec structure.
 *
 * @param timeout Time out in milliseconds.
 * @param tv      timespec structure.
 */

void Util::absTimeout(long timeout, timespec &tv)
{
    struct timeval timenow;

    time_t sec = timeout / 1000;
    long msec = timeout % 1000;
    long nsec = (1000000) * msec;

    gettimeofday(&timenow, NULL);

    // get absolute seconds
    tv.tv_sec = timenow.tv_sec + sec;

    long ns = (timenow.tv_usec * 1000) + nsec;
    if (ns >= 1000000000)
    {
        tv.tv_sec++;
        ns -= 1000000000;
    }

    // get absolute nanoseconds
    tv.tv_nsec = ns;
}

bool Util::absTimeEqual(struct timeval &src, struct timeval &dst, int accuracy)
{//check if src > dst or src + accuracy > dst
    int64_t diff;
    if (src.tv_sec + 1 < dst.tv_sec) return false;
    else if (src.tv_sec + 1 == dst.tv_sec)
    {
        diff = dst.tv_usec + 1000000 - src.tv_usec;
    } else if (src.tv_sec == dst.tv_sec)
    {
        if (src.tv_usec >= dst.tv_usec) return true;
        else diff = dst.tv_usec - src.tv_usec;
    } else return true; //src.tv_sec > dst.tv_sec

    if (diff < (int64_t)accuracy) return true;
    else return false;
}

/**
 * @brief Configures a stream to be used in hexadecimal format.
 *
 * @param s     The stream.
 * @param fill  true if fills with zeros on left.
 * @param size  Data size.
 */
void Util::confHexStream(std::ostringstream &s, bool fill, int size)
{
    s.setf(std::ios_base::uppercase);
    s.setf(std::ios_base::hex, std::ios_base::basefield);

    if (fill)
        s << std::setw(size * 2) << std::setfill('0');
}

/**
 * @brief Configures a stream to be used in octal format.
 *
 * @param s     The stream.
 * @param size  Data size.
 */
void Util::confOctStream(std::ostringstream &s)
{
    s.setf(std::ios_base::uppercase);
    s.setf(std::ios_base::oct, std::ios_base::basefield);
}

/**
 * @brief Performs a case insensitive comparation.
 *
 * @param s1  String to be compared.
 * @param s2  String to be compared.
 *
 * returns An integer greater than, equal to, or less than 0, if the 
 *         string s1 is greater than, equal to, or less than the 
 *         string s2, respectively.
 */
int Util::nocaseCmp(const std::string &s1, const std::string &s2)
{
    std::string::const_iterator it1 = s1.begin();
    std::string::const_iterator it2 = s2.begin();

    //stop when either string's end has been reached
    while ((it1 != s1.end()) && (it2 != s2.end()))
    {
        if (::toupper(*it1) != ::toupper(*it2)) //letters differ?
        {
            // return -1 to indicate smaller than, 1 otherwise
            return (::toupper(*it1) < ::toupper(*it2)) ? -1 : 1;
        }

        //proceed to the next character in each string
        ++it1;
        ++it2;
    }

    size_t size1 = s1.size(), size2 = s2.size(); // cache lengths

    //return -1,0 or 1 according to strings' lengths
    if (size1 == size2)
    {
        return 0;
    }

    return (size1 < size2) ? -1 : 1;
}

/*
 * get a file size
 */
int Util::get_file_length(char* file_path)
{
    unsigned long PosCur = 0;
    unsigned long PosBegin = 0;
    unsigned long PosEnd = 0;
    FILE * file_ptr;

    file_ptr = fopen(file_path, "rb");

    if (file_ptr == NULL)
    {
        return 0;
    }

    PosCur = ftell(file_ptr);
    fseek(file_ptr, 0L, SEEK_SET);
    PosBegin = ftell(file_ptr);
    fseek(file_ptr, 0L, SEEK_END);
    PosEnd = ftell(file_ptr);
    fseek(file_ptr, PosCur, SEEK_SET);
    return PosEnd - PosBegin;
}
