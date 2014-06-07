#include <cstring>

#include "../include/tuple.h"


TupleElement::TupleElement(unsigned short type)
{
    this->type = type;
}

TupleElement::~TupleElement()
{
    
}

unsigned short TupleElement::get_type()
{
    return this->type;
}

TupleInt::TupleInt(int value) : TupleElement(TupleElementType::INT)
{
    this->value = value;
}

int TupleInt::get_value()
{
    return this->value;
}

TupleFloat::TupleFloat(float value) : TupleElement(TupleElementType::FLOAT)
{
    this->value = value;
}

float TupleFloat::get_value()
{
    return this->value;
}

StaticTupleString::StaticTupleString() 
    : TupleElement(TupleElementType::STRING)
{
    this->value = "";
}

StaticTupleString::StaticTupleString(std::string value, Allocator& alloc)
    : TupleElement(TupleElementType::STRING)
{
    this->value = (char*) alloc.allocate(value.length() + 1);
    strcpy(this->value, value.c_str());
}

StaticTupleString::StaticTupleString(const char * value, Allocator& alloc) 
    : TupleElement(TupleElementType::STRING)
{
    this->value = (char*) alloc.allocate(strlen(value) + 1);
    strcpy(this->value, value);
}

StaticTupleString::~StaticTupleString()
{
    
}

char* StaticTupleString::get_value()
{
    char* str = new char[strlen(this->value) + 1];
    strcpy(str, this->value);
    return str;
}

TupleString::TupleString(std::string value)
{
    this->value = new char[value.length() + 1];
    strcpy(this->value, value.c_str());
}

TupleString::TupleString(const char* value)
{
    this->value = new char[strlen(value) + 1];
    strcpy(this->value, value);
}

TupleString::~TupleString()
{
    delete [] this->value;
}