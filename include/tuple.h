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
private:
    unsigned short type;
    
protected:
    TupleElement(unsigned short);

public:
    unsigned short int get_type();
    virtual ~TupleElement();
};

class TupleInt : public TupleElement
{
    int value;
    
public:
    TupleInt(int);
    int get_value();
};

class TupleFloat : public TupleElement
{
    float value;
    
public:
    TupleFloat(float);
    float get_value();
};

class StaticTupleString : public TupleElement
{
protected:
    char* value;  
    
public:
    StaticTupleString(std::string, Allocator&);
    StaticTupleString(const char*, Allocator&);
    virtual ~StaticTupleString();
    char* get_value();
};

class TupleString : public StaticTupleString
{
public:
    TupleString(std::string);
    TupleString(const char*);
    ~TupleString();
};

class Tuple
{
    bool deletable;
    unsigned int size;
    TupleElement* elements;
    Tuple(TupleElement * elements, unsigned int size, Allocator&);
    
public:
    Tuple(TupleElement* elements, unsigned int size);
    ~Tuple();
    unsigned short int get_type(int);
    int get_int(int);
    float get_float(int);
    std::string get_string(int);
    char* get_cstring(int);
};

#endif	/* TUPLE_H */

