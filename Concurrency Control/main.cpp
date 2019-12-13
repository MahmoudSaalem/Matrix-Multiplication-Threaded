//
//  main.cpp
//  semaphores
//
//  Created by Mahmoud Salem on 12/10/19.
//  Copyright © 2019 Mahmoud Salem. All rights reserved.
//

#include <iostream>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

//#ifdef __APPLE__
//#include <dispatch/dispatch.h>
//typedef dispatch_semaphore_t sem_t;
//#define sem_init(psem, shared, value)   *psem = dispatch_semaphore_create(value)
//#define sem_wait(psem)                  dispatch_semaphore_wait(*psem, DISPATCH_TIME_FOREVER)
//#define sem_trywait(psem)               dispatch_semaphore_wait(*psem, DISPATCH_TIME_FOREVER)
//#define sem_post(psem)                  dispatch_semaphore_signal(*psem)
//#define sem_destroy(psem)               dispatch_release(*psem)
//#else
//#include <semaphore.h>
//#endif

using namespace std;

// Circular buffer implementation
class CircularBuffer {
private:
    int start,
        end;
    int size;
    int *buffer;
    int allocated;

public:
    CircularBuffer() {
        start = 0;
        end = -1;
        allocated = 0;
    }

    ~CircularBuffer() {
        delete buffer;
    }

    void initializeBuffer(int size) {
        this->size = size;
        buffer = new int[size];

    }

    template <class type>
    int push(type element) {
        if (allocated < size) {
            allocated++;
            int pushed = end = (end + 1) % size;
            buffer[end] = element;
            return pushed;
        } else {
            cout << "Buffer is full!" << endl;
            return -1;
        }
    }

    int pop() {
        if (allocated) {
            allocated--;
            int popped = start;
            start = (start + 1) % size;
            return popped;
        } else {
            cout << "Buffer is empty!" << endl;
            return -1;
        }
    }
};

// GLOBAL VARIABLES
int counter = 0,    // shared variable
    timeInterval,   // time interval for monitor
    threadsCount,
    BUFFER_SIZE;
bool running;
CircularBuffer buffer;

sem_t empty,
      full;
pthread_mutex_t bufferMutex,
                counterMutex;

// FUNCTIONS
void *counterCallback(void *vargp) {
    int id = *(int*)vargp;
    int randSleepTime;
    srand((unsigned int)time(0));
    while (running) {
        randSleepTime = (rand() % (timeInterval) + 1) * 1000000;
        usleep(randSleepTime);
        printf("Counter thread %d: received a message\n", id);
        if (pthread_mutex_trylock(&counterMutex)) {
            printf("Counter thread %d: waiting to write\n", id);
            pthread_mutex_lock(&counterMutex);
        }
        counter++;
        printf("Counter thread %d: now adding to counter, counter value = %d\n", id, counter);
        pthread_mutex_unlock(&counterMutex);
    }
    return NULL;
}

void *monitorCallback(void *vargp) {
    int m, produced;
    while (running) {
        usleep(timeInterval * 1000000);
        if (pthread_mutex_trylock(&counterMutex)) {
            printf("Monitor thread: waiting to read counter\n");
            pthread_mutex_lock(&counterMutex);
        }
        m = counter;
        printf("Monitor thread: reading a count value of %d\n", m);
        counter = 0;
        pthread_mutex_unlock(&counterMutex);
        if (sem_trywait(&empty)) {
            printf("Monitor thread: buffer is full!\n");
            sem_wait(&empty);
        }
        if (pthread_mutex_trylock(&bufferMutex)) {
            printf("Monitor thread: waiting for access to buffer\n");
            pthread_mutex_lock(&bufferMutex);
        }
        produced = buffer.push(m);
        printf("Monitor thread: writing to buffer at position %d\n", produced);
        pthread_mutex_unlock(&bufferMutex);
        sem_post(&full);
    }
    return NULL;
}

void *collectorCallback(void *vargp) {
    int consumed;
    while (running) {
        usleep(3 * timeInterval * 1000000);
        if (sem_trywait(&full)) {
            printf("Collector thread: buffer is empty!\n");
            sem_wait(&full);
        }
        if (pthread_mutex_trylock(&bufferMutex)) {
            printf("Collector thread: waiting for access to buffer\n");
            pthread_mutex_lock(&bufferMutex);
        }
        consumed = buffer.pop();
        printf("Collector thread: reading from buffer at position %d\n", consumed);
        pthread_mutex_unlock(&bufferMutex);
        sem_post(&empty);
    }
    return NULL;
}

void initialize_ids(int ids[], int threadsCount) {
    for (int i = 0; i < threadsCount; i++) {
        ids[i] = i + 1;
    }
}

void runProgram() {
    running = true;

    // read threadsCount
    threadsCount = 10;
    // read timeInterval
    timeInterval = 5;
    // read BUFFER_SIZE
    BUFFER_SIZE = 4;

    buffer.initializeBuffer(BUFFER_SIZE);
    int *ids = new int[threadsCount];
    //int ids[threadsCount];
    bool success;
    initialize_ids(ids, threadsCount);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    pthread_t monitorID;
    success = pthread_create(&monitorID, NULL, monitorCallback, NULL);

    pthread_t *threadIDs = new pthread_t[threadsCount];
    //pthread_t threadIDs[threadsCount];
    for (int i = 0; i < threadsCount; i++) {
        success = pthread_create(&threadIDs[i], NULL, counterCallback, &ids[i]);
        assert(!success);
    }

    pthread_t collectorID;
    success = pthread_create(&collectorID, NULL, collectorCallback, NULL);

    pthread_join(monitorID, NULL);

    for (int i = 0; i < threadsCount; i++) {
        pthread_join(threadIDs[i], NULL);
    }

    pthread_join(collectorID, NULL);

    delete threadIDs;
    delete ids;

    sem_destroy(&empty);
    sem_destroy(&full);
}

int main(int argc, const char *argv[]) {
    runProgram();
    return 0;
}
