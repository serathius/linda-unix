#ifndef LINDA_H
#define LINDA_H

#include <string>
#include <sys/types.h>
#include "tuple.h"

class Linda
{
public:	
	static const int MAX_TUPLES = 100;

private:
	struct ShmTuple
	{
		char valid;
		char tupleData[TUPLE_MAX_SIZE];
	};
	
	struct SharedMemory
	{
		int processCount;
		int semKey;
		ShmTuple tupleArray[MAX_TUPLES];
	};
	
	int semId;
	short processCounter;
	SharedMemory *shm; 
	
	
public:
	
	Linda() : processCounter(0), semId(0), shm(nullptr) {}
	
	int init(key_t shm_key);
        template <typename... Elements>
	void output(Tuple<Elements...> &tuple);
        template <typename... Elements>
	Tuple<Elements...>* input(std::string pattern, int timeout);
        template <typename... Elements>
	Tuple<Elements...>* read(std::string pattern, int timeout);
};

#endif /* LINDA_H */