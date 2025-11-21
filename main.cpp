//
// Multi-thread synchronization assignment
// Problem 1: No-starve Readers/Writers
// Problem 2: Writer-Priority Readers/Writers
//

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <iostream>
using namespace std;

// ---------------------- SEMAPHORE CLASS ------------------------

class Semaphore {
public:
    Semaphore(int initialValue)
    {
        sem_init(&mSemaphore, 0, initialValue);
    }
    ~Semaphore()
    {
        sem_destroy(&mSemaphore);
    }
    void wait()
    {
        sem_wait(&mSemaphore);
    }
    void signal()
    {
        sem_post(&mSemaphore);
    }

private:
    sem_t mSemaphore;
};

// ============================
// PROBLEM 1 — NO-STARVE READERS/WRITERS
// ============================

Semaphore turnstile1(1);
Semaphore roomEmpty1(1);
Semaphore mutex_r1(1);

int readerCount1 = 0;
int sharedData1 = 0;

void* reader1(void* arg)
{
    long id = (long)arg;

    turnstile1.wait();
    turnstile1.signal();

    mutex_r1.wait();
    readerCount1++;
    if (readerCount1 == 1)
        roomEmpty1.wait();
    mutex_r1.signal();

    printf("[P1] Reader %ld reading value %d\n", id, sharedData1);
    sleep(1);

    mutex_r1.wait();
    readerCount1--;
    if (readerCount1 == 0)
        roomEmpty1.signal();
    mutex_r1.signal();

    return NULL;
}

void* writer1(void* arg)
{
    long id = (long)arg;

    turnstile1.wait();
    roomEmpty1.wait();

    sharedData1++;
    printf("[P1] Writer %ld writing new value %d\n", id, sharedData1);
    sleep(1);

    roomEmpty1.signal();
    turnstile1.signal();

    return NULL;
}

// ============================
// PROBLEM 2 — WRITER PRIORITY READERS/WRITERS
// ============================

Semaphore writerGate(1);    
Semaphore noWriters(1);     
Semaphore rmutex(1);        

int readerCount2 = 0;
int sharedData2 = 0;

void* reader2(void* arg)
{
    long id = (long)arg;

    writerGate.wait();   
    writerGate.signal(); 

    rmutex.wait();
    readerCount2++;
    if (readerCount2 == 1)
        noWriters.wait();  
    rmutex.signal();

    printf("[P2] Reader %ld reading value %d\n", id, sharedData2);
    sleep(1);

    rmutex.wait();
    readerCount2--;
    if (readerCount2 == 0)
        noWriters.signal(); 
    rmutex.signal();

    return NULL;
}

void* writer2(void* arg)
{
    long id = (long)arg;

    writerGate.wait();    
    noWriters.wait();     

    sharedData2++;
    printf("[P2] Writer %ld writing new value %d\n", id, sharedData2);
    sleep(1);

    noWriters.signal();    
    writerGate.signal();   

    return NULL;
}

// ============================
// MAIN — RUNS SELECTED PROBLEM
// ============================

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <problem number: 1 or 2>\n", argv[0]);
        return 1;
    }

    int problem = atoi(argv[1]);

    // --------- PROBLEM 1 ----------
    if (problem == 1) {
        printf("Running Problem 1: No-starve Readers/Writers\n");

        pthread_t readers[5];
        pthread_t writers[5];

        for (long i = 0; i < 5; i++) {
            pthread_create(&readers[i], NULL, reader1, (void*)i);
            pthread_create(&writers[i], NULL, writer1, (void*)i);
        }

        for (int i = 0; i < 5; i++) {
            pthread_join(readers[i], NULL);
            pthread_join(writers[i], NULL);
        }
        return 0;
    }

    // --------- PROBLEM 2 ----------
    if (problem == 2) {
        printf("Running Problem 2: Writer-Priority Readers/Writers\n");

        pthread_t readers[5];
        pthread_t writers[5];

        for (long i = 0; i < 5; i++) {
            pthread_create(&readers[i], NULL, reader2, (void*)i);
            pthread_create(&writers[i], NULL, writer2, (void*)i);
        }

        for (int i = 0; i < 5; i++) {
            pthread_join(readers[i], NULL);
            pthread_join(writers[i], NULL);
        }
        return 0;
    }

    printf("Problem %d not implemented yet.\n", problem);
    return 0;
}
