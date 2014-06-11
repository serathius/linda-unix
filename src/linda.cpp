#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <iostream>
#include "../include/linda.h"
#include "../include/tuple_pattern.h"
#include "../include/tuple.h"



int Linda::init(key_t shm_key)
{

	// try to create new shared memory segment
	shmId = shmget(shm_key, 8 + MAX_TUPLES*(TUPLE_MAX_SIZE + 1), IPC_CREAT | IPC_EXCL | 0777);

	if (shmId < 0 && errno == EEXIST) // someone already created shm segment
	{
		// connect to existing shm segment
		shmId = shmget(shm_key, 8 + MAX_TUPLES*(TUPLE_MAX_SIZE + 1), IPC_CREAT | 0777);
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
		std::cout << "semKey=" << shm->semKey;
		if ((semId = semget(shm->semKey, 3, IPC_CREAT | 0777)) == -1)
		{
			std::cerr << "[Linda] Error obtaining existing semaphores. Errno = " << errno << std::endl;
			return 6;
		}
		std::cout << "semId="<<semId;
		processCounter = ++(shm->processCount);
		std::cout << "processCounter="<<processCounter;
	
		struct sembuf getCriticalSection[1] = 
		{
			SEM_READ, 1, 0,
		};
		if (semop(semId, getCriticalSection, sizeof(getCriticalSection)/sizeof(sembuf)) < 0)
		{
			std::cerr << "[Linda input] Error refreshing readers' semaphore limit. Errno = " << errno << std::endl;
			return 7;
		}
	
		return 0;
	}
	else if (shmId == -1)
	{
		std::cerr << "[Linda] Error creating new shared memory segment. Errno = " << errno << std::endl;
		return 3;
	}

	// we created new shared memory segment, so fill it with empty environment data
	
	if ((long)(shm = (SharedMemory*)shmat(shmId, 0, 0)) < 0)
	{
		std::cerr << "[Linda] Error mapping shared memory segment. Errno = " << errno << std::endl;
		return 4;
	}
	
	shm->processCount = 1;
	int key = 1234;
	
	if ((semId = semget(key, 3, IPC_CREAT | IPC_EXCL | 0777))  == -1)
	{
		std::cerr << "[Linda] Error creating semaphore set. Errno = " << errno << std::endl;
		return 5;
	}
	std::cout << "semId" << semId;
	shm->semKey = key;
	
	for (int i = 0; i < MAX_TUPLES; i++) shm->tupleArray[i].valid = TUPLE_INVALID;
	
	struct sembuf increment[1] = 
	{
		SEM_READ, 1, 0,
		//SEM_WAIT, 1, 0
	};
	if (semop(semId, increment, sizeof(increment)/sizeof(sembuf)) < 0)
	{
		std::cerr << "[Linda input] Error refreshing readers' semaphore limit. Errno = " << errno << std::endl;
		return 7;
	}
	std::cout<<"Incremented sem_read"<<std::endl;
	return 0;
}


Linda::~Linda()
{
	std::cout<<"Destroying..."<<std::endl;
	if (shm != nullptr)
	{
		struct sembuf getCriticalSection[1] = 
		{
			SEM_READ, -1, 0
		};
		if (semop(semId, getCriticalSection, 1) < 0)
		{
			std::cerr << "[Linda input] Error refreshing readers' semaphore limit. Errno = " << errno << std::endl;
		}

		struct shmid_ds desc;
		shmctl(shmId, IPC_STAT, &desc);
		
		if (desc.shm_nattch == 1)
		{
			shmdt(shm);
			shmctl(shmId, IPC_RMID, NULL); // delete shared memory segment
			semctl(semId, 3, IPC_RMID, NULL); // delete semaphores set
		}
		else
			shmdt(shm);
	}
}


