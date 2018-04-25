#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#define FARMERS 4

/*
 * CS 334 - Programming Assignment 3 Part 2
 * Problem 1: Using POSIX Synchronization
 * Authors: Andrew Williams, Kurtis Davidson
 * April 24, 2018
 */

int numNorthRipley = 0;
int numSouthRipley = 0;
int famerIsCrossing = 0;
int northLastToCross = 0;
int i;

// Initialize mutex and condition
pthread_cond_t proceedToCross = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Farmer struct
struct farmer{
    pthread_t ID;
    int farmerID;
};

/*********************************************************************
 * crossBridgeInitialNorth:
 *    Method to cross bridge for each farmer that is on the North side
 *    Param: farmerThread
 *********************************************************************/
void crossBridgeNorth(void * farmerThread){

    // Create a reference to the farmerThread
    struct farmer * ref = farmerThread;
    // Mutex Lock
    pthread_mutex_lock(&mutex);
    // Record one more North farmer
    numNorthRipley++;

    // Checks if the farmer can cross with respect to other farmers currently crossing or if the last farmer to cross was a North farmer
    while((numSouthRipley > 0 && northLastToCross == 1) || famerIsCrossing == 1){
        pthread_cond_wait(&proceedToCross, &mutex);
    }

    // Set farmer to occupy bridge
    famerIsCrossing = 1;
    // Last farmer to cross was now North (true)
    northLastToCross = 1;
    // Decreases number of farmers from North
    numNorthRipley--;

    printf("Farmer %d is crossing from the north side.\n", ref->farmerID);

    // Generate random number to be used for sleeping
    unsigned int time;
    rand_r(&time);
    int randNum = rand() % 3;

    // Sleep for random number of time while farmer is crossing
    sleep(randNum);

    printf("Farmer %d has crossed from north to south.\n", ref->farmerID);

    // Farmer has crossed
    famerIsCrossing = 0;
    // Unlock mutex
    pthread_mutex_unlock(&mutex);
    // Signal to others that crossing can continue again
    pthread_cond_signal(&proceedToCross);

}

/*********************************************************************
 * crossBridgeInitSouth:
 *    Method to cross bridge for each farmer that is on the South side
 *    Param: farmerThread
 *********************************************************************/
void crossBridgeSouth(void * farmerThread){

    // Create a reference to the farmerThread
    struct farmer * ref = farmerThread;
    // Mutex Lock
    pthread_mutex_lock(&mutex);
    // Record one more South farmer
    numSouthRipley++;

    // Checks if the farmer can cross with respect to other farmers currently crossing or if the last farmer to cross was a North farmer
    while((numNorthRipley > 0 && northLastToCross == 0) || famerIsCrossing == 1){
        pthread_cond_wait(&proceedToCross, &mutex);
    }

    // Set farmer to occupy bridge
    famerIsCrossing = 1;
    // Last farmer to cross was now South
    northLastToCross = 0;
    // Decreases number of farmers from South
    numSouthRipley--;

    printf("Farmer %d is crossing from the south side.\n", ref->farmerID);

    // Generate random number to be used for sleeping
    unsigned int time;
    rand_r(&time);
    int randNum = rand() % 3;

    // Sleep for random number of time while farmer is crossing
    sleep(randNum);

    printf("Farmer %d has crossed from south to north.\n", ref->farmerID);

    // Farmer has crossed
    famerIsCrossing = 0;
    // Unlock mutex
    pthread_mutex_unlock(&mutex);
    // Signal to others that crossing can continue again
    pthread_cond_signal(&proceedToCross);
}

int main(int argc, char * argv[]){
    // Allocates memory for farmers
    struct farmer * arrayOfFarmers = malloc(FARMERS * sizeof(struct farmer));
    // Creates a thread for each farmer with a max of FARMERS
    for (i = 0; i < FARMERS; i++) {
        // Assigns even numbers as North farmers
        if (i % 2 != 0) {
            printf("North farmer created.\n");
            arrayOfFarmers[i].farmerID = i;
            pthread_create(&(arrayOfFarmers[i].ID), NULL, crossBridgeNorth, &(arrayOfFarmers[i]));
        // Assigns odd numbers as South farmers
        } else {
            printf("South farmer created.\n");
            arrayOfFarmers[i].farmerID = i;
            pthread_create(&(arrayOfFarmers[i].ID), NULL, crossBridgeSouth, &(arrayOfFarmers[i]));
        }
    }

    //Joins all threads together
    for (i = 0; i < FARMERS; i++) {
        pthread_join(arrayOfFarmers[i].ID, NULL);
    }

    return 0;
}
