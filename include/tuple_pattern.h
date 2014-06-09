#ifndef TUPLEPATTERN_H
#define	TUPLEPATTERN_H

#include <list>
#include <memory>

#include "tuple.h"
#include "pattern_element.h"

template<typename... Types> class TuplePattern;

template<>
class TuplePattern<>
{
protected:
    std::list<std::shared_ptr<PatternElement>> patterns;
    TuplePattern()
    {
        
    }
    
public:
    template<typename... Elements>
    bool match(const Tuple<Elements...>& tuple)
    {
        const char * actual = tuple.bytes;
        for(auto pattern: this->patterns)
        {
            if(!pattern->match(actual))
                return false;
            actual += pattern->get_size();
        }
        return true;
    }
};

template<typename... Tail>
class TuplePattern<int, Tail...> : public TuplePattern<Tail...>
{
public:
    template<typename... Patterns>
    TuplePattern(const int& value, Patterns... tail) 
        : TuplePattern<Tail...>(tail...)
    {
        this->patterns.push_front(std::shared_ptr<PatternElement>(
            new TemplatePatternElement<int>(value)));
    }
    
    template<typename... Patterns>
    TuplePattern(const char * pattern, Patterns... tail) 
        : TuplePattern<Tail...>(tail...)
    {
        switch(pattern[0])
        {
            case '*':
                this->patterns.push_front(std::shared_ptr<PatternElement>(
                        new TemplatePatternElement<int>(SignType::Any, 
                                                        pattern + 1)));
                break;
            case '<':
                if(pattern[1] == '=')
                {
                    this->patterns.push_front(std::shared_ptr<PatternElement>(
                        new TemplatePatternElement<int>(SignType::LessThenEqual, 
                                                        pattern + 2)));
                }
                else
                {
                    this->patterns.push_front(std::shared_ptr<PatternElement>(
                        new TemplatePatternElement<int>(SignType::LessThen, 
                                                        pattern + 1)));
                }
                break;
            case '>':
                if(pattern[1] == '=')
                {
                    this->patterns.push_front(std::shared_ptr<PatternElement>(
                        new TemplatePatternElement<int>(
                            SignType::GreatherThenEqual, pattern + 2)));
                }
                else
                {
                    this->patterns.push_front(std::shared_ptr<PatternElement>(
                        new TemplatePatternElement<int>(SignType::GreatherThen, 
                                                        pattern + 1)));
                }
                break;    
                
            default:
                this->patterns.push_front(std::shared_ptr<PatternElement>(
                        new TemplatePatternElement<int>(SignType::Equal, 
                                                        pattern)));
                break;
        }
    }
};

template<typename... Tail>
class TuplePattern<float, Tail...> : public TuplePattern<Tail...>
{
public:
    template<typename... Patterns>
    TuplePattern(const float& value, Patterns... tail) 
        : TuplePattern<Tail...>(tail...)
    {
        this->patterns.push_front(std::shared_ptr<PatternElement>(
            new TemplatePatternElement<float>(value)));
    }
    
    template<typename... Patterns>
    TuplePattern(const char * pattern, Patterns... tail) 
        : TuplePattern<Tail...>(tail...)
    {
        switch(pattern[0])
        {
            case '*':
                this->patterns.push_front(std::shared_ptr<PatternElement>(
                        new TemplatePatternElement<float>(SignType::Any, 
                                                        pattern + 1)));
                break;
            case '<':
                if(pattern[1] == '=')
                {
                    this->patterns.push_front(std::shared_ptr<PatternElement>(
                        new TemplatePatternElement<float>(SignType::LessThenEqual, 
                                                        pattern + 2)));
                }
                else
                {
                    this->patterns.push_front(std::shared_ptr<PatternElement>(
                        new TemplatePatternElement<float>(SignType::LessThen, 
                                                        pattern + 1)));
                }
                break;
            case '>':
                if(pattern[1] == '=')
                {
                    this->patterns.push_front(std::shared_ptr<PatternElement>(
                        new TemplatePatternElement<float>(
                            SignType::GreatherThenEqual, pattern + 2)));
                }
                else
                {
                    this->patterns.push_front(std::shared_ptr<PatternElement>(
                        new TemplatePatternElement<float>(SignType::GreatherThen, 
                                                        pattern + 1)));
                }
                break;    
                
            default:
                this->patterns.push_front(std::shared_ptr<PatternElement>(
                    new TemplatePatternElement<float>(SignType::Equal, 
                                                    pattern)));
                break;
        }
    }
};

template<typename... Tail>
class TuplePattern<std::string, Tail...> : public TuplePattern<Tail...>
{
public:
    template<typename... Patterns>
    TuplePattern(const char * pattern, Patterns... tail) 
        : TuplePattern<Tail...>(tail...)
    {
        if(strlen(pattern) == 1 && pattern[0] == '*')
        {
            this->patterns.push_front(std::shared_ptr<PatternElement>(
                new TemplatePatternElement<std::string>(SignType::Any, 
                                                        pattern)));
        }
        else
        {
            this->patterns.push_front(std::shared_ptr<PatternElement>(
                new TemplatePatternElement<std::string>(SignType::Equal, 
                                                        pattern)));
        }
    }
};

#endif	/* TUPLEPATTERN_H */