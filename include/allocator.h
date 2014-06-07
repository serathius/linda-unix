#ifndef ALLOCATOR_H
#define	ALLOCATOR_H

class Allocator
{
private:
    void* actual;
    void* end;
public:
    Allocator(void* start, void* stop);
    void* allocate(unsigned int size);
};

#endif	/* ALLOCATOR_H */

