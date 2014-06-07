#ifndef LINDA_H
#define LINDA_H

#include <string>
#include <sys/types.h>
#include "tuple.h"

class Linda
{
	
	int processCounter, semId;
	SharedMemory *shm; 
	
	struct ShmTuple
	{
		char valid;
		char [TUPLE_MAX_SIZE] tupleData;
	}
	
	struct SharedMemory
	{
		int processCount;
		int semKey;
		ShmTuple[MAX_TUPLES] tupleArray;
	}
	
public:
	const int TUPLE_MAX_SIZE = 255; /* in bytes */
	const int MAX_TUPLES = 100;
	
	Linda() : processCounter(0), semId(0), shm(nullptr) {}
	
	int init(key_t shm_key);
	void output(Tuple &tuple);
	Tuple& input(std::string pattern, int timeout);
	Tuple& read(std::string pattern, int timeout);
}

#endif /* LINDA_H */