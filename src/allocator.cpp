#include "../include/allocator.h"
#include "../include/exceptions.h"


Allocator::Allocator(void* start, void* stop)
{
    this->actual = start;
    this->end = stop;
}

void* Allocator::allocate(unsigned int size)
{
    if (this->actual + size > this->end)
        throw TupleTooLargeError();
    this->actual += size;
    return this->actual;
}