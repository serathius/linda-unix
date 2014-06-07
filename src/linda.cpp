#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <iostream>
#include "../include/linda.h"


int Linda::init(key_t shm_key)
{
	// try to create new shared memory segment
	int shmId = shmget(shm_key, 8 + MAX_TUPLES*(TUPLE_MAX_SIZE + 1), IPC_CREAT | IPC_EXCL);
	
	if (shmId == -1 && errno == EEXIST) // someone already created shm segment
	{
		// connect to existing shm segment
		shmId = shmget(shm_key, 8 + MAX_TUPLES*(TUPLE_MAX_SIZE + 1), IPC_CREAT);
		if(shmId == -1)
		{
			std::cerr << "[Linda] Error connecting to shared memory segment. Errno = " << errno << std::endl;
			return 1;
		}
		
		// read basic environment data (process counter and semaphores' table id)
		
		if ((shm = (SharedMemory*)shmat(shmId, 0, 0)) < 0)
		{
			std::cerr << "[Linda] Error mapping shared memory segment. Errno = " << errno << std::endl;
			return 2;
		}
		
		if (semId = semget(shm->semKey, 3, IPC_CREAT) == -1)
		{
			std::cerr << "[Linda] Error obtaining existing semaphores. Errno = " << errno << std::endl;
			return 6;
		}			
		
		processCounter = shm->processCount;
		
	}
	else if (shmId == -1)
	{
		std::cerr << "[Linda] Error creating new shared memory segment. Errno = " << errno << std::endl;
		return 3;
	}
	
	// we created new shared memory segment, so fill it with empty environment data
	
	if ((shm = (SharedMemory*)shmat(shmId, 0, 0)) < 0)
	{
		std::cerr << "[Linda] Error mapping shared memory segment. Errno = " << errno << std::endl;
		return 4;
	}
	
	shm->processCount = 1;
	
	int key = 1234; // should probably generate it randomly in loop
	
	if (semId = semget(key, 3, IPC_CREAT | IPC_EXCL)  == -1)
	{
		std::cerr << "[Linda] Error creating semaphore set. Errno = " << errno << std::endl;
		return 5;
	}
	
	shm->semKey = key;
	
}

void Linda::output(Tuple &tuple)
{
	
}

Tuple& Linda::input(std::string pattern, int timeout)
{
	
}

Tuple& Linda::read(std::string pattern, int timeout)
{
	
}