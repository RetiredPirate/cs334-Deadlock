
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <iostream>

using namespace std;


#define NUM_CUSTOMERS 5
#define NUM_RESOURCES 3

int available[NUM_RESOURCES];
int maximum[NUM_CUSTOMERS][NUM_RESOURCES];
int allocation[NUM_CUSTOMERS][NUM_RESOURCES];
int need[NUM_CUSTOMERS][NUM_RESOURCES];

pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t resource_mutex = PTHREAD_MUTEX_INITIALIZER;


void* customer_func(void* ID) {
    int id = * (int*) ID;
    std::cout << "Hello from thread number "<< id << std::endl;
    pthread_exit(NULL);
}


int resource_request(int customer_num, int request[]) {
    pthread_mutex_lock(&resource_mutex);


    pthread_mutex_unlock(&resource_mutex);
    return 0;
}

int resource_release(int customer_num, int release[]) {
    pthread_mutex_lock(&resource_mutex);


    pthread_mutex_unlock(&resource_mutex);
    return 0;
}


int main(int argc, char* argv[]) {

    if (argc != NUM_RESOURCES+1) {
        //wrong number of arguments
        printf("Program requires %d arguments.\n", NUM_RESOURCES);
        exit(1);
    }

    // initialize resouce array
    for(int i = 0; i < NUM_RESOURCES; i++) {
        available[i] = atoi(argv[i+1]);
    }

    // initialize other arrays
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        for (int j = 0; j < NUM_RESOURCES; j++) {
            allocation[i][j] = 0;
            int max = rand() % 5 + 1; // rand 1 through 5
            maximum[i][j] = max;
            need[i][j] = max;
        }
    }

    // make threads
    pthread_t threads[NUM_CUSTOMERS];
    int ids[NUM_CUSTOMERS]; // customer id's

    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, customer_func, &ids[i]);
    }

    pthread_exit(NULL);
}
