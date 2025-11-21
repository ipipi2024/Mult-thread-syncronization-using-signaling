//
// Example from: http://www.amparo.net/ce155/sem-ex.c
//
// Adapted using some code from Downey's book on semaphores
//
// Compilation:
//
//       g++ main.cpp -lpthread -o main -lm
// or 
//      make
//

#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include <semaphore.h>  /* Semaphore */
#include <iostream>
using namespace std;

// ----------------- Problem 1: Readers–Writers (No-Starve) -----------------

Semaphore turnstile(1);    // Door #1
Semaphore roomEmpty(1);    // The sign
Semaphore mutex_r(1);      // Protects readerCount

int readerCount = 0;
int sharedData = 0;


class Semaphore {
public:
    // Constructor
    Semaphore(int initialValue)
    {
        sem_init(&mSemaphore, 0, initialValue);
    }
    // Destructor
    ~Semaphore()
    {
        sem_destroy(&mSemaphore); /* destroy semaphore */
    }
    
    // wait
    void wait()
    {
        sem_wait(&mSemaphore);
    }
    // signal
    void signal()
    {
        sem_post(&mSemaphore);
    }
    
    
private:
    sem_t mSemaphore;
};



void* reader(void* arg)
{
    long id = (long)arg;

    // Pass through the turnstile
    turnstile.wait();
    turnstile.signal();

    // Lightswitch: reader enters
    mutex_r.wait();
    readerCount++;
    if (readerCount == 1)
        roomEmpty.wait();   // first reader puts up the sign
    mutex_r.signal();

    // Reading
    printf("Reader %ld reading: %d\n", id, sharedData);
    fflush(stdout);
    sleep(1);

    // Lightswitch: reader leaves
    mutex_r.wait();
    readerCount--;
    if (readerCount == 0)
        roomEmpty.signal(); // last reader removes sign
    mutex_r.signal();

    return NULL;
}

void* writer(void* arg)
{
    long id = (long)arg;

    // Close door #1 (turnstile) // does not release it
    turnstile.wait();

    // Wait for room to be empty
    roomEmpty.wait();

    // Write to sharedData
    sharedData++;
    printf("Writer %ld writing new value: %d\n", id, sharedData);
    fflush(stdout);
    sleep(1);

    // Re-open everything
    roomEmpty.signal();
    turnstile.signal();

    return NULL;
}


int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <problem number>\n", argv[0]);
        return 1;
    }

    int problem = atoi(argv[1]);

    if (problem == 1) {
        printf("Running Problem 1: No-starve Readers/Writers\n");

        pthread_t readers[5];
        pthread_t writers[5];

        for (long i = 0; i < 5; i++) {
            pthread_create(&readers[i], NULL, reader, (void*)i);
            pthread_create(&writers[i], NULL, writer, (void*)i);
        }

        for (int i = 0; i < 5; i++) {
            pthread_join(readers[i], NULL);
            pthread_join(writers[i], NULL);
        }

        return 0;
    }

    printf("Problem not implemented yet.\n");
    return 0;
}





