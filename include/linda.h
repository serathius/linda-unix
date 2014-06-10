#ifndef LINDA_H
#define LINDA_H

#include <string>
#include <sys/types.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <iostream>
#include "../include/linda.h"
#include "../include/tuple_pattern.h"
#include "../include/tuple.h"

#define SEM_WRITE 0
#define SEM_READ 1
#define SEM_WAIT 2

class Linda
{
public:	
	static const int MAX_TUPLES = 100;
	static const int TUPLE_INVALID = 1;
	static const int TUPLE_VALID = 0;

private:
	struct ShmTuple
	{
		char valid;
		GenericTuple tuple;
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
	
	~Linda();
	
	int init(key_t shm_key);
        template <typename... Elements>
	void output(Tuple<Elements...> &tuple);
        template <typename... Elements>
	Tuple<Elements...>* input(std::string pattern, int timeout);
        template <typename... Elements>
	Tuple<Elements...>* read(std::string pattern, int timeout);
};


template <typename... Elements>
void Linda::output(Tuple<Elements...> &tuple)
{
	struct sembuf getCriticalSection[3] = 
	{
		SEM_WRITE, 0, 0, 				// wait for sem_write to be 0
		SEM_WRITE, 1, SEM_UNDO, 		// then add 1 (get critical section)
		SEM_READ, (short)-(shm->processCount), SEM_UNDO 	// wait for sem_read to be >= processCount (no reader in critical section)
								// and reduce its value to 0 (get critical section exclusively)
	};

	std::cout << "write="<<semctl(semId, 0, GETVAL)<<std::flush;
	std::cout << "read="<<semctl(semId, 1, GETVAL)<<std::flush;
	std::cout << "pC="<<(shm->processCount)<<std::flush;
	if (semop(semId, getCriticalSection, 3) < 0)
	{
		int err = errno;
		std::cerr << "[Linda output] Error getting critical section. Errno = " << err << std::endl;
		return;	
	}
	
	
	for (int i = 0; i < MAX_TUPLES; ++i)
	{
		if (shm->tupleArray[i].valid == TUPLE_INVALID)
		{
			// good place to put new tuple
			shm->tupleArray[i].tuple = tuple; // copy tuple to shm
			shm->tupleArray[i].valid = TUPLE_VALID;
			std::cout << "Tuple put" << std::endl;
			break;
		}
	}
	
	// check if anyone is waiting for new data to arrive
	int semval = semctl(semId, 2, GETVAL);
	
	if (!semval) // if not, don't decrement semaphore value (it is already 0)
	{
		struct sembuf releaseCriticalSection[2] = 
		{
			SEM_READ, processCounter, SEM_UNDO,	// release critical section for readers
			SEM_WRITE, -1, SEM_UNDO			// release critical section for writers
		};
		
		if (semop(semId, releaseCriticalSection, 2) < 0)
		{
			std::cerr << "[Linda output] Error releasing critical section. Errno = " << errno << std::endl;
			return;	
		}
	}
	else
	{
		struct sembuf releaseCriticalSection[3] = 
		{
			SEM_READ, processCounter, SEM_UNDO,	// release critical section for readers
			SEM_WAIT, -1, SEM_UNDO,		// wake up readers waiting for new data
			SEM_WRITE, -1, SEM_UNDO			// release critical section for writers
		};
		
		if (semop(semId, releaseCriticalSection, 3) < 0)
		{
			std::cerr << "[Linda output] Error releasing critical section. Errno = " << errno << std::endl;
			return;	
		}
	}
	
}

template <typename... Elements>
Tuple<Elements...>* Linda::input(std::string pattern, int timeout)
{
	// TODO decreasing timeout after each semaphore operation
	
	bool got = false; // indicates if tuple is found and downloaded
	
	struct sembuf getCriticalSection[3] = 
	{
		(unsigned short)0, (short)0, (short)0,  				// wait for sem_write to be 0
		(unsigned short)0, (short)1, SEM_UNDO, 		// then add 1 (get critical section)
		(unsigned short)1, (short)-(shm->processCount), SEM_UNDO 	// wait for sem_read to be >= processCount (no reader in critical section)
								// and reduce its value to 0 (get critical section exclusively)
	};
	
	struct timespec timeout_struct;
	timeout_struct.tv_sec = (time_t)timeout;
	timeout_struct.tv_nsec = 0;
	
	TuplePattern<int, float, std::string> tuplePat("*", "*", "*");
	Tuple<int, float, std::string>* tuple;
	while (!got)
	{
		std::cout << "write="<<semctl(semId, 0, GETVAL)<<std::flush;
		std::cout << "read="<<semctl(semId, 1, GETVAL)<<std::flush;
		std::cout << "pC="<<(shm->processCount)<<std::flush;
		if (semop(semId, getCriticalSection, 3/*, &timeout_struct*/) < 0)
		{
			std::cerr << "[Linda input] Error getting critical section. Errno = " << errno << std::endl;
			return nullptr;	
		}
		
		
		for (int i = 0; i < MAX_TUPLES; ++i)
		{
			std::cout<<shm->tupleArray[i].tuple.get<int>(0);
			if (shm->tupleArray[i].valid == TUPLE_VALID)
			{
				if (tuplePat.match(shm->tupleArray[i].tuple))
				{
					//int val = shm->tupleArray[i].tuple.get<int>(0);
					//float val2 = shm->tupleArray[i].tuple.get<float>(2);
					//std::string val3 = shm->tupleArray[i].tuple.get<std::string>(3);
					//tuple = new Tuple<int, float, std::string>(4, 8.0, "ala");
					got = true;
				}
					
			}
		}		
		
		if (!got)
		{
			struct sembuf releaseCriticalSection[3] = 
			{
				SEM_WAIT, 0, 0,		// wait for sem_wait to be 0
				SEM_READ, (short)shm->processCount, SEM_UNDO,	// release critical section for readers
				SEM_WRITE, -1, SEM_UNDO	// release critical section for writers
			};
			
			if (semop(semId, releaseCriticalSection, 3) < 0)
			{
				std::cerr << "[Linda input] Error releasing critical section. Errno = " << errno << std::endl;
				return nullptr;	
			}
		}
		else
		{
			struct sembuf releaseCriticalSection[2] = 
			{
				SEM_READ, (short)shm->processCount, SEM_UNDO,	// release critical section for readers
				SEM_WRITE, -1, SEM_UNDO	// release critical section for writers
			};
			
			if (semop(semId, releaseCriticalSection, 2) < 0)
			{
				std::cerr << "[Linda input] Error releasing critical section. Errno = " << errno << std::endl;
				return nullptr;	
			}
			
			return tuple;
		}
	} // while
	
}

template <typename... Elements>
Tuple<Elements...>* Linda::read(std::string pattern, int timeout)
{
	// TODO decreasing timeout after each semaphore operation
	
	bool got = false; // indicates if tuple is found and downloaded
	
	struct sembuf getCriticalSection[1] = 
	{
		SEM_READ, -1, SEM_UNDO 	// wait for sem_read to be >= 1 (at least one more reader can get in)
								// and decrease its value
	};
	
	struct timespec timeout_struct;
	timeout_struct.tv_sec = (time_t)timeout;
	timeout_struct.tv_nsec = 0;
	
	while (!got)
	{
		if (semtimedop(semId, getCriticalSection, 1, &timeout_struct) < 0)
		{
			std::cerr << "[Linda read] Error getting critical section. Errno = " << errno << std::endl;
			return nullptr;	
		}
		
		
		// TODO getting tuple
		
		
		if (!got)
		{
			struct sembuf releaseCriticalSection[2] = 
			{
				SEM_WAIT, 0, 0,		// wait for sem_wait to be 0
				SEM_READ, 1, SEM_UNDO,	// release critical section for another reader
			};
			
			if (semop(semId, releaseCriticalSection, 2) < 0)
			{
				std::cerr << "[Linda input] Error releasing critical section. Errno = " << errno << std::endl;
				return nullptr;	
			}
		}
		else
		{
			struct sembuf releaseCriticalSection[1] = 
			{
				SEM_READ, 1, SEM_UNDO,	// release critical section for another reader
			};
			
			if (semop(semId, releaseCriticalSection, 1) < 0)
			{
				std::cerr << "[Linda input] Error releasing critical section. Errno = " << errno << std::endl;
				return nullptr;	
			}
		}
	} // while
}

#endif /* LINDA_H */