#ifndef SEGMENT_H
#define	SEGMENT_H

#include "tuple.h"

template <int SIZE> 
class Segment
{
private:        
    Tuple tuple;
    byte rest[SIZE - sizeof(Tuple)];
public:
    Segment(const Tuple& tuple);
    ~Segment();
    Tuple get_tuple();
}

#endif	/* SEGMENT_H */