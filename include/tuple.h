#ifndef TUPLE_H
#define	TUPLE_H

#include <string>
#include <cstring>
#include "exceptions.h"

const int TUPLE_MAX_SIZE = 255;

union IntBytes
{
    char bytes[sizeof(int)];
    int value;
};

union FloatBytes
{
    char bytes[sizeof(float)];
    float value;
};

template<typename... Types> class Tuple;

template<> 
class Tuple<>
{
protected:
    char bytes[TUPLE_MAX_SIZE] = {'\0'};
    char *  get_element(int);     
    Tuple(int index);
    
public:
    template <typename T> 
    T get(int);
};

template<typename... Tail>
class Tuple <int, Tail...> : public Tuple<Tail...>
{
protected:
    Tuple(int index, const int& value, const Tail&... tail) 
    : Tuple<Tail...>((int)(index + 1 + sizeof(int)), tail...)
    {
        this->bytes[index] = 'i';
        IntBytes integer;
        integer.value = value;
        for(int i=0; i<sizeof(int); ++i)
        {
            this->bytes[index + 1 + i] = integer.bytes[i];
        }
    }
public:
    Tuple(const int& value, const Tail&... tail) : Tuple(0, value, tail...)
    {
        
    }
};

template<typename... Tail>
class Tuple <float, Tail...> : public Tuple<Tail...>
{
protected:
    Tuple(int index, const float& value, const Tail&... tail) 
    : Tuple<Tail...>((int)(index + 1 + sizeof(int)), tail...)
    {
        this->bytes[index] = 'f';
        FloatBytes f;
        f.value = value;
        for(int i=0; i<sizeof(float); ++i)
        {
            this->bytes[index + 1 + i] = f.bytes[i];
        }
    }
public:
    Tuple(const float& value, const Tail&... tail) : Tuple(0, value, tail...)
    {
        
    }
};

template<typename... Tail>
class Tuple <std::string, Tail...> : public Tuple<Tail...>
{
protected:
    Tuple(int index, const std::string& value, const Tail&... tail) 
    : Tuple<Tail...>((int)(index + 2 + value.length()), tail...)
    {
        this->bytes[index] = 's';
        strcpy(this->bytes + index + 1, value.c_str());
    }
    
public:
    Tuple(const std::string& value, const Tail&... tail) 
        : Tuple(0, value, tail...)
    {
        
    }
};
#endif	/* TUPLE_H */
