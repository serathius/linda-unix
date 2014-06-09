#ifndef PATTERN_ELEMENT_H
#define	PATTERN_ELEMENT_H

#include "tuple.h"

enum SignType
{
    Any,
    LessThen,
    LessThenEqual,
    Equal,
    GreatherThen,
    GreatherThenEqual
};

class PatternElement 
{
protected:
    SignType sign;
    char type;
    unsigned short size;
    
    PatternElement(SignType, char, unsigned short);
    
public:
    virtual bool match(const char*) = 0;
    virtual char get_type();
    unsigned short get_size();
};

template<typename T> class TemplatePatternElement;


template<> class TemplatePatternElement<int> : public PatternElement
{
    int value;
    
public:
    TemplatePatternElement(int value) 
        : PatternElement(SignType::Equal, 'i', sizeof(int) + 1)
    {
            this->value = value;
    }
    TemplatePatternElement(SignType sign, const char * value) 
        : PatternElement(sign, 'i', sizeof(int) + 1)
    {
            if(sign == SignType::Any)
                value = 0;
            else
                this->value = atoi(value);
    }
    bool match(const char* bytes)
    {
        if(bytes[0] != this->type)
            return false;
        ByteType<int> byte_value(bytes + 1);
        switch(this->sign)
        {
            case SignType::LessThen:
                return byte_value.get_value() < this->value;
            case SignType::LessThenEqual:
                return byte_value.get_value() <= this->value;
            case SignType::Equal:
                return byte_value.get_value() == this->value;
            case SignType::GreatherThen:
                return byte_value.get_value() > this->value;
            case SignType::GreatherThenEqual:
                return byte_value.get_value() >= this->value; 
            case SignType::Any:
                return true;
        }
    }
};
template<> class TemplatePatternElement<float> : public PatternElement
{
    float value;
    
public:
    TemplatePatternElement(float value) 
        : PatternElement(SignType::Equal, 'f', sizeof(float) + 1)
    {
            this->value = value;
    }
    TemplatePatternElement(SignType sign, const char * value) 
        : PatternElement(sign, 'f', sizeof(float) + 1)
    {
            if(sign == SignType::Any)
                value = 0;
            else
                this->value = atof(value);
    }
    bool match(const char* bytes)
    {
        if(bytes[0] != this->type)
            return false;
        ByteType<float> byte_value(bytes + 1);
        switch(this->sign)
        {
            case SignType::LessThen:
                return byte_value.get_value() < this->value;
            case SignType::LessThenEqual:
                return byte_value.get_value() <= this->value;
            case SignType::Equal:
                return byte_value.get_value() == this->value;
            case SignType::GreatherThen:
                return byte_value.get_value() > this->value;
            case SignType::GreatherThenEqual:
                return byte_value.get_value() >= this->value; 
            case SignType::Any:
                return true;
        }
    }
};
template<> class TemplatePatternElement<std::string> : public PatternElement
{
protected:
    std::string value;
    
public:
    TemplatePatternElement(SignType sign, const char * value) 
        : PatternElement(sign, 's', strlen(value) + 2)
    {
        this->value = std::string(value);
    }
    bool match(const char* value)
    {
        std::string temp = value + 1;
        switch(this->sign)
        {
            case SignType::Equal:;
                return temp == this->value;
            case SignType::Any:
            {
                this->size = (int) strlen(value) + 1;
                return true;
            }
        }
    }
};


#endif	/* PATTERN_ELEMENT_H */

