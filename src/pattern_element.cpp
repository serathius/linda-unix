#include "../include/pattern_element.h"


PatternElement::PatternElement(SignType sign, char type, unsigned short size) 
{
    this->sign = sign;
    this->type = type;
    this->size = size;
}

char PatternElement::get_type() 
{
    return this->type;
}

unsigned short PatternElement::get_size()
{
    return this->size;
}