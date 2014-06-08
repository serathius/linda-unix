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
    IntBytes integer;
    for(int i=0; i<sizeof(float); ++i)
    {
        integer.bytes[i] = element[i + 1];
    }
    return integer.value;
}

template<>
float Tuple<>::get(int index)
{
    char * element = this->get_element(index);
    if (element[0] != 'f')
        throw TupleWrongTypeError();
    FloatBytes f;
    for(int i=0; i<sizeof(float); ++i)
    {
        f.bytes[i] = element[i + 1];
    }
    return f.value;
}

template<>
std::string Tuple<>::get(int index)
{
    char * element = this->get_element(index);
    if (element[0] != 's')
        throw TupleWrongTypeError();
    return std::string(element + 1);
}
