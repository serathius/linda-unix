#ifndef TUPLE_H
#define	TUPLE_H

#include <string>
#include "allocator.h"

enum TupleElementType
{
    INT, FLOAT, STRING
};

class TupleElement
{
protected:
    const unsigned short short int type;
    unsigned short short int get_type();
public:
    virtual ~TupleElement();
};

class TupleInt : public TupleElement
{
    const int value;
    
public:
    TupleInt(int);
    int get_value();
};

class TupleFloat : public TupleElement
{
    const float value;
    
public:
    TupleFloat(float);
    float get_value();
};

class TupleString : public TupleElement
{
    const char* value;   
public:
    virtual ~TupleString();
    const char* get_value();
};

class DynamicTupleString : public TupleString
{
public:
    DynamicTupleString(std::string);
    DynamicTupleString(const char*);
    DynamicTupeString(const DynamicTupleString&);
    ~DynamicTupleString();
};

class StaticTupleString : public TupleString
{
public:
    StaticTupleString(std::string, Allocator&);
    StaticTupleString(const char*, Allocator&);
    StaticTupleString(const StaticTupleString&);
};

class Tuple
{
    bool deletable;
    unsigned int size;
    TupleElement* elements;
    Tuple(TupleElements * elements, unsigned int size, Allocator&);
    
public:
    Tuple(TupleElements* elements, unsigned int size);
    ~Tuple();
    unsigned short short int get_type(int);
    int get_int(int);
    float get_float(int);
    std::string get_string(int);
    char* get_cstring(int);
};


#endif	/* TUPLE_H */

