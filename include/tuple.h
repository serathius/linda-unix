#ifndef LINDA_TUPLE_H
#define	LINDA_TUPLE_H

#include <string>
#include <cstring>
#include "exceptions.h"


template <typename T>
class ByteType
{
    union Type
    {
        char bytes[sizeof(int)];
        T value;
    };
    Type value;
    
public:
    ByteType(T value)
    {
        this->value.value = value;
    }
    ByteType(const char* value)
    {
       for(int i=0; i<sizeof(T); ++i)
        {
            this->value.bytes[i] = value[i];
        }
    }
    T get_value()
    {
        return this->value.value;
    }
    void get_bytes(char * value)
    {
        for(int i=0; i<sizeof(T); ++i)
        {
            value[i] = this->value.bytes[i];
        }
    }
};

template class ByteType<int>;
template class ByteType<float>;

static const int TUPLE_MAX_SIZE = 255;
template <typename... Elements> class TuplePattern;

class GenericTuple
{
    friend class TuplePattern<>;
protected:
    char bytes[TUPLE_MAX_SIZE] = {'\0'};
    
    char *  get_element(int);   

public:
    template <typename T> 
    T get(int);
};

template<typename... Types> class Tuple;

template<> class Tuple<> : public GenericTuple
{
protected:
    Tuple(int index);
};

template<typename... Tail>
class Tuple <int, Tail...> : public Tuple<Tail...>
{
protected:
    Tuple(int index, const int& value, const Tail&... tail) 
    : Tuple<Tail...>((int)(index + 1 + sizeof(int)), tail...)
    {
        this->bytes[index] = 'i';
        ByteType<int> integer(value);
        integer.get_value();
        integer.get_bytes(this->bytes + index + 1);
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
        ByteType<float> f(value);
        f.get_bytes(this->bytes + index + 1);
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
