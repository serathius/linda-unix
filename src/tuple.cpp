#include <cstring>

#include "../include/tuple.h"
#include "../include/exceptions.h"

char * Tuple<>::get_element(int index)
{
    int pos = 0;
    while(index > 0)
    {
        if (pos >= TUPLE_MAX_SIZE)
            throw IndexOutOfBoundsError();
        switch(this->bytes[pos])
        {
            case 'i': 
                pos += 1 + sizeof(int);
                break;
            case 'f':
                pos += 1 + sizeof(float);
                break;
            case 's':
                pos += 2 + strlen(this->bytes + pos + 1);
                break;
            default:
                throw IndexOutOfBoundsError();
        }
        index--;
    }
    return this->bytes + pos;
}

Tuple<>::Tuple(int index)
{

}

template<>
int Tuple<>::get(int index)
{
    char * element = this->get_element(index);
    if (element[0] != 'i')
        throw TupleWrongTypeError();
    ByteType<int> integer(element + 1);
    return integer.get_value();
}

template<>
float Tuple<>::get(int index)
{
    char * element = this->get_element(index);
    if (element[0] != 'f')
        throw TupleWrongTypeError();
    ByteType<float> f(element + 1);
    return f.get_value();
}

template<>
std::string Tuple<>::get(int index)
{
    char * element = this->get_element(index);
    if (element[0] != 's')
        throw TupleWrongTypeError();
    return std::string(element + 1);
}
