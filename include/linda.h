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

#define E_UNINITIALIZED 1

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
        int semKey;
        ShmTuple tupleArray[MAX_TUPLES];
    };

    int semId, shmId;
    SharedMemory *shm; 
    bool debug;
	
public:
	
    Linda() : semId(0), shm(nullptr), debug(false) {}
    ~Linda();
    void setDebugMode(bool debug) { this->debug = debug; }
    int init(key_t shm_key);
    template <typename... Elements>
    int output(Tuple<Elements...> &tuple);
    template <typename... Elements>
    Tuple<Elements...>* input(TuplePattern<Elements...> pattern, int timeout);
    template <typename... Elements>
    Tuple<Elements...>* read(TuplePattern<Elements...> pattern, int timeout);
};

/**
 * Puts tuple into shared memory
 * 
 * args:
 * tuple - reference to tuple to be put
 * 
 * returns:
 * 0 if everything was ok and tuple was put into shared memory,
 * E_UNINITIALIZED if there is no correct shared memory segment initialized
 * unix's errno - when system functions returned error. This is useful combined
 * with debug mode which gives feedback, what logical part didn't succeeded
 */
template <typename... Elements>
int Linda::output(Tuple<Elements...> &tuple)
{
    if (!shm) 
        return E_UNINITIALIZED;
    if (debug) 
        std::cout << "[Linda] output" << std::endl;

    struct shmid_ds desc;

    // read number of attached processes
    shmctl(shmId, IPC_STAT, &desc);

    struct sembuf getCriticalSection[3] = 
    {
        SEM_WRITE, 0, 0, 				// wait for sem_write to be 0
        SEM_WRITE, 1, SEM_UNDO,                         // then add 1 (get critical section)
        SEM_READ, (short)-(desc.shm_nattch), SEM_UNDO 	// wait for sem_read to be >= processCount (no reader in critical section)
                                                        // and reduce its value to 0 (get critical section exclusively)
    };

    if (semop(semId, getCriticalSection, sizeof(getCriticalSection)/sizeof(sembuf)) < 0)
    {
        int err = errno;
        if (debug) std::cerr << "[Linda output] Error getting critical section." << std::endl;
        return err;	
    }

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
    int semval = semctl(semId, SEM_WAIT, GETVAL);

    if (!semval) // if not, don't decrement semaphore value (it is already 0)
    {
        // get current processes count (could have changed during searching)
        shmctl(shmId, IPC_STAT, &desc);

        struct sembuf releaseCriticalSection[2] = 
        {
            SEM_READ, (short)(desc.shm_nattch), SEM_UNDO,	// release critical section for readers
            SEM_WRITE, -1, SEM_UNDO			// release critical section for writers
        };

        if (semop(semId, releaseCriticalSection, sizeof(releaseCriticalSection)/sizeof(sembuf)) < 0)
        {
            int err = errno;
            if (debug) std::cerr << "[Linda output] Error releasing critical section." << std::endl;
            return err;	
        }
    }
    else
    {
        // get current processes count (could have changed during searching)
        shmctl(shmId, IPC_STAT, &desc);

        struct sembuf releaseCriticalSection[3] = 
        {
            SEM_READ, (short)(desc.shm_nattch), SEM_UNDO,	// release critical section for readers
            SEM_WAIT, (short)-semval, SEM_UNDO,		// wake up readers waiting for new data
            SEM_WRITE, -1, SEM_UNDO			// release critical section for writers
        };

        if (semop(semId, releaseCriticalSection, sizeof(releaseCriticalSection)/sizeof(sembuf)) < 0)
        {
            int err = errno;
            if (debug) std::cerr << "[Linda output] Error releasing critical section. Errno = " << errno << std::endl;
            return err;	
        }
    }
    return 0;
}

/**
 * Gets tuple from shared memory and removes it from there
 * 
 * args:
 * pattern - TuplePattern object representing what kind of tuple is wanted
 * timeout - amount of seconds, after which function returns. If tuple wasn't
 * found in that time, nullptr is returned; timeout cannot be null
 * 
 * returns:
 * pointer to new copy of tuple wanted if everything succeeded
 * nullptr if:
 * - there is no correct shared memory segment
 * - given timeout is 0
 * - there was error with system functions. Enabling debug option will provide
 * more information about which part caused error
 */
template <typename... Elements>
Tuple<Elements...>* Linda::input(TuplePattern<Elements...> pattern, int timeout)
{	
    if (!shm || !timeout) 
        return nullptr;

    if (debug) 
        std::cout << "[Linda] input" << std::endl;

    bool got = false; // indicates if tuple is found and downloaded

    time_t begin, end;

    struct shmid_ds desc;

    // get current attached processes count
    shmctl(shmId, IPC_STAT, &desc);

    struct sembuf getCriticalSection[3] = 
    {
        SEM_WRITE, 0, 0,  				// wait for sem_write to be 0
        SEM_WRITE, 1, SEM_UNDO,                     // then add 1 (get critical section)
        SEM_READ, (short)-(desc.shm_nattch), SEM_UNDO 	// wait for sem_read to be >= processCount (no reader in critical section)
                                                        // and reduce its value to 0 (get critical section exclusively)
    };

    struct timespec timeout_struct;
    timeout_struct.tv_sec = (time_t)timeout;
    timeout_struct.tv_nsec = 0;

    Tuple<Elements...>* tuple;
    while (!got)
    {
        begin = time(NULL);
        if (semtimedop(semId, getCriticalSection, sizeof(getCriticalSection)/sizeof(sembuf), &timeout_struct) < 0)
        {
                int res = errno;
                if (errno != EAGAIN)
                        if (debug) std::cerr << "[Linda input] Error getting critical section. Errno = " << errno << std::endl;
                return nullptr;	
        }

        for (int i = 0; i < MAX_TUPLES; ++i)
        {
            if (shm->tupleArray[i].valid == TUPLE_VALID)
            {
                if (pattern.match(shm->tupleArray[i].tuple))
                {
                    tuple = new Tuple<Elements...>((Tuple<Elements...>&)shm->tupleArray[i].tuple);
                    shm->tupleArray[i].valid = TUPLE_INVALID;
                    got = true;
                    break;
                }
            }
        }		

        if (!got)
        {
            end = time(NULL);
            timeout_struct.tv_sec = timeout_struct.tv_sec - (begin - end);
            if (timeout_struct.tv_sec <= 0)
            {
                if (debug) std::cerr << "[Linda input] Timeout" << std::endl;
                return nullptr;
            }

            // get current processes count (could have changed during searching)
            shmctl(shmId, IPC_STAT, &desc);

            struct sembuf releaseCriticalSection[3] = 
            {
                SEM_READ, (short)(desc.shm_nattch), SEM_UNDO,	// release critical section for readers
                SEM_WRITE, -1, SEM_UNDO,	// release critical section for writers
                SEM_WAIT, 1, SEM_UNDO		// set sem_wait to let some writer decrement it 
            };

            struct sembuf wait[1] = { SEM_WAIT, 0, 0 };		// wait for sem_wait to be 0

            if ((semop(semId, releaseCriticalSection, sizeof(releaseCriticalSection)/sizeof(sembuf)) < 0)
                    || semtimedop(semId, wait, sizeof(wait)/sizeof(sembuf), &timeout_struct) < 0)
            {
                int err = errno;
                if (errno == EAGAIN)
                        if (debug) std::cerr << "[Linda input] Timeout" << std::endl;
                else if (debug) std::cerr << "[Linda input] Error releasing critical section. Errno = " << errno << std::endl;
                return nullptr;	
            }
        }
        else
        {
            // get current processes count (could have changed during searching)
            shmctl(shmId, IPC_STAT, &desc);

            struct sembuf releaseCriticalSection[2] = 
            {
                SEM_READ, (short)(desc.shm_nattch), SEM_UNDO,	// release critical section for readers
                SEM_WRITE, -1, SEM_UNDO	// release critical section for writers
            };

            if (semop(semId, releaseCriticalSection, sizeof(releaseCriticalSection)/sizeof(sembuf)) < 0)
            {
                if (debug) std::cerr << "[Linda input] Error releasing critical section. Errno = " << errno << std::endl;
                return nullptr;	
            }

            return tuple;
        }
    }
}

/**
 * Gets tuple from shared memory and leaves it there
 * 
 * args:
 * pattern - TuplePattern object representing what kind of tuple is wanted
 * timeout - amount of seconds, after which function returns. If tuple wasn't
 * found in that time, nullptr is returned; timeout cannot be null
 * 
 * returns:
 * pointer to new copy of tuple wanted if everything succeeded
 * nullptr if:
 * - there is no correct shared memory segment
 * - given timeout is 0
 * - there was error with system functions. Enabling debug option will provide
 * more information about which part caused error
 */
template <typename... Elements>
Tuple<Elements...>* Linda::read(TuplePattern<Elements...> pattern, int timeout)
{	
	if (!shm || !timeout) return nullptr;
	
	if (debug) std::cout << "[Linda] read" << std::endl;
	
	bool got = false; // indicates if tuple is found

	time_t begin, end;

	struct timespec timeout_struct;
	timeout_struct.tv_sec = (time_t)timeout;
	timeout_struct.tv_nsec = 0;
	
	Tuple<Elements...>* tuple;

	struct sembuf getCriticalSection[1] = 
	{
		SEM_READ, -1, SEM_UNDO 	// wait for sem_read to be >= 1 (at least one more reader can get in)
								// and decrease its value
	};
	
	while (!got)
	{
		begin = time(NULL);
		if (semtimedop(semId, getCriticalSection, sizeof(getCriticalSection)/sizeof(sembuf), &timeout_struct) < 0)
		{
			int err = errno;
			if (errno == EAGAIN) 
				if (debug) std::cerr << "[Linda read] Timeout" << std::endl;
			else if (debug) std::cerr << "[Linda read] Error getting critical section. Errno = " << errno << std::endl;
			return nullptr;	
		}
		
		for (int i = 0; i < MAX_TUPLES; ++i)
		{
			if (shm->tupleArray[i].valid == TUPLE_VALID)
			{
				if (pattern.match(shm->tupleArray[i].tuple))
				{
					tuple = new Tuple<Elements...>((Tuple<Elements...>&)shm->tupleArray[i].tuple);
					got = true;
					break;
				}
					
			}
		}
		
		if (!got)
		{
			end = time(NULL);
			timeout_struct.tv_sec = timeout_struct.tv_sec - (begin - end);
			if (timeout_struct.tv_sec <= 0)
			{
				if (debug) std::cerr << "[Linda input] timeout" << std::endl;
				return nullptr;
			}
			
			struct sembuf *releaseCriticalSection;
			int semval = semctl(semId, SEM_WAIT, GETVAL);
			int size = 0;
			if (semval)
			{
				struct sembuf rel[1] = 
				{
					SEM_READ, 1, SEM_UNDO	// release critical section for another reader
				};
				releaseCriticalSection = rel;
				size = 1;
			}
			else 
			{
				struct sembuf rel[2] = 
				{
					SEM_WAIT, 1, SEM_UNDO,	// set sem_wait to let some writer decrement it
					SEM_READ, 1, SEM_UNDO	// release critical section for another reader
				};
				releaseCriticalSection = rel;
				size = 2;
			}
				
			struct sembuf wait[1] = { SEM_WAIT, 0, 0 };		// wait for sem_wait to be 0
			
			if ((semop(semId, releaseCriticalSection, size) < 0)
				|| semtimedop(semId, wait, sizeof(wait)/sizeof(sembuf), &timeout_struct) < 0)
			{
				int err = errno;
				if (errno == EAGAIN)
					if (debug) std::cerr << "[Linda read] Timeout" << std::endl;
				else if (debug) std::cerr << "[Linda read] Error releasing critical section. Errno = " << errno << std::endl;
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
				if (debug) std::cerr << "[Linda input] Error releasing critical section. Errno = " << errno << std::endl;
				return nullptr;	
			}
			return tuple;
		}
	} // while
}

#endif /* LINDA_H */