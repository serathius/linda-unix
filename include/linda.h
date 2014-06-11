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
	static const char TUPLE_INVALID = 1;
	static const char TUPLE_VALID = 0;

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
	Tuple<Elements...>* input(TuplePattern<Elements...> pattern, int timeout);
        template <typename... Elements>
	Tuple<Elements...>* read(TuplePattern<Elements...> pattern, int timeout);
};


template <typename... Elements>
void Linda::output(Tuple<Elements...> &tuple)
{
	std::cout << "LindaOutput"<< std::endl << std::flush;
struct sembuf getCriticalSection[2] = 
	{
		SEM_WRITE, 0, 0,//, 				// wait for sem_write to be 0
		SEM_WRITE, 1, SEM_UNDO, 		// then add 1 (get critical section)
		//SEM_READ, (short)-(shm->processCount), SEM_UNDO 	// wait for sem_read to be >= processCount (no reader in critical section)
								// and reduce its value to 0 (get critical section exclusively)
	};

	std::cout << "write="<<semctl(semId, 0, GETVAL)<<std::flush;
	std::cout << "read="<<semctl(semId, 1, GETVAL)<<std::flush;
	std::cout << "pC="<<(shm->processCount)<<std::flush;
	if (semop(semId, getCriticalSection, 2) < 0)
	{
		int err = errno;
		std::cerr << "[Linda output] Error getting critical section. Errno = " << err << std::endl;
		return;	
	}
	
	std::cout << "Entered critical section"<<std::endl<<std::flush;
	for (int i = 0; i < MAX_TUPLES; ++i)
	{
		if (shm->tupleArray[i].valid == TUPLE_INVALID)
		{
			// good place to put new tuple
			shm->tupleArray[i].tuple = tuple; // copy tuple to shm
			shm->tupleArray[i].valid = TUPLE_VALID;
			break;
		}
	}	
	
	// check if anyone is waiting for new data to arrive
	int semval = semctl(semId, 2, GETVAL);
	
	if (!semval) // if not, don't decrement semaphore value (it is already 0)
	{
		struct sembuf releaseCriticalSection[1] = 
		{
			//SEM_READ, processCounter, SEM_UNDO,	// release critical section for readers
			SEM_WRITE, -1, SEM_UNDO			// release critical section for writers
		};
		
		if (semop(semId, releaseCriticalSection, 1) < 0)
		{
			std::cerr << "[Linda output] Error releasing critical section. Errno = " << errno << std::endl;
			return;	
		}
		std::cout << "Section released"<<std::endl<<std::flush;
	}
	else
	{
		struct sembuf releaseCriticalSection[1] = 
		{
			//SEM_READ, processCounter, SEM_UNDO,	// release critical section for readers
			//SEM_WAIT, -1, SEM_UNDO,		// wake up readers waiting for new data
			SEM_WRITE, -1, SEM_UNDO			// release critical section for writers
		};
		
		if (semop(semId, releaseCriticalSection, 1) < 0)
		{
			std::cerr << "[Linda output] Error releasing critical section. Errno = " << errno << std::endl;
			return;	
		}
		
		std::cout << "Section released"<<std::endl<<std::flush;

	}
	
}

template <typename... Elements>
Tuple<Elements...>* Linda::input(TuplePattern<Elements...> pattern, int timeout)
{
	// TODO decreasing timeout after each semaphore operation
	
	bool got = false; // indicates if tuple is found and downloaded
	
	struct sembuf getCriticalSection[2] = 
	{
		(unsigned short)0, (short)0, (short)0,  				// wait for sem_write to be 0
		(unsigned short)0, (short)1, SEM_UNDO, 		// then add 1 (get critical section)
		//(unsigned short)1, (short)-(shm->processCount), SEM_UNDO 	// wait for sem_read to be >= processCount (no reader in critical section)
								// and reduce its value to 0 (get critical section exclusively)
	};
	
	struct timespec timeout_struct;
	timeout_struct.tv_sec = (time_t)timeout;
	timeout_struct.tv_nsec = 0;
	
	Tuple<Elements...>* tuple;
	while (!got)
	{
		//std::cout << "write="<<semctl(semId, 0, GETVAL)<<std::flush;
		//std::cout << "read="<<semctl(semId, 1, GETVAL)<<std::flush;
		//std::cout << "pC="<<(shm->processCount)<<std::flush;
		if (semop(semId, getCriticalSection, 2/*, &timeout_struct*/) < 0)
		{
			std::cerr << "[Linda input] Error getting critical section. Errno = " << errno << std::endl;
			return nullptr;	
		}
		
		//std::cout << "Got critical section" << std::endl << std::flush;
		
		for (int i = 0; i < MAX_TUPLES; ++i)
		{
			//std::cout << i << " " << (int)(shm->tupleArray[i].valid) << std::endl<<std::flush;
			//std::cout<<(shm->tupleArray[i].valid)<<std::flush;
			if (shm->tupleArray[i].valid == TUPLE_VALID)
			{
				
				if (pattern.match(shm->tupleArray[i].tuple))
				{
					std::cout << "Matching tuple found" << std::endl<<std::flush;
					tuple = new Tuple<Elements...>((Tuple<Elements...>&)shm->tupleArray[i].tuple);
					shm->tupleArray[i].valid = TUPLE_INVALID;
					got = true;
					break;
				}
					
			}
		}		

		
		if (!got)
		{
			struct sembuf releaseCriticalSection[1] = 
			{
				//SEM_WAIT, 0, 0,		// wait for sem_wait to be 0
				//SEM_READ, (short)shm->processCount, SEM_UNDO,	// release critical section for readers
				SEM_WRITE, -1, SEM_UNDO	// release critical section for writers
			};
			
			if (semop(semId, releaseCriticalSection, 1) < 0)
			{
				std::cerr << "[Linda input] Error releasing critical section. Errno = " << errno << std::endl;
				return nullptr;	
			}
		}
		else
		{
			struct sembuf releaseCriticalSection[1] = 
			{
				//SEM_READ, (short)shm->processCount, SEM_UNDO,	// release critical section for readers
				SEM_WRITE, -1, SEM_UNDO	// release critical section for writers
			};
			
			if (semop(semId, releaseCriticalSection, 1) < 0)
			{
				std::cerr << "[Linda input] Error releasing critical section. Errno = " << errno << std::endl;
				return nullptr;	
			}
			
			return tuple;
		}
	} // while
	
}

template <typename... Elements>
Tuple<Elements...>* Linda::read(TuplePattern<Elements...> pattern, int timeout)
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
	
	Tuple<Elements...>* tuple;
	while (!got)
	{
		if (semop(semId, getCriticalSection, 1/*, &timeout_struct*/) < 0)
		{
			std::cerr << "[Linda read] Error getting critical section. Errno = " << errno << std::endl;
			return nullptr;	
		}
		

		for (int i = 0; i < MAX_TUPLES; ++i)
		{
			//std::cout << i << " " << (int)(shm->tupleArray[i].valid) << std::endl<<std::flush;
			//std::cout<<(shm->tupleArray[i].valid)<<std::flush;
			if (shm->tupleArray[i].valid == TUPLE_VALID)
			{
				if (pattern.match(shm->tupleArray[i].tuple))
				{
					std::cout << "Matching tuple found" << std::endl<<std::flush;
					tuple = new Tuple<Elements...>((Tuple<Elements...>&)shm->tupleArray[i].tuple);
					got = true;
					break;
				}
					
			}
		}
		
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
			
			return tuple;
		}
	} // while
}

#endif /* LINDA_H */