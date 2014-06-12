#ifndef EXCEPTIONS_H
#define	EXCEPTIONS_H

#include <exception>


class LindaError : public std::exception
{
public:
    int err;
	LindaError() { err = 0; }
	LindaError(int err) { this->err= err; }
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

class UnknownPattern : public LindaError
{

};

#endif	/* EXCEPTIONS_H */

