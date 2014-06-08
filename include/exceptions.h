#ifndef EXCEPTIONS_H
#define	EXCEPTIONS_H

#include <exception>


class LindaError : public std::exception
{
    
};

class TupleTooLargeError : public LindaError
{
    
};

class TupleWrongTypeError : public LindaError
{
    
};

class IndexOutOfBoundsError : public LindaError
{
    
};


#endif	/* EXCEPTIONS_H */

