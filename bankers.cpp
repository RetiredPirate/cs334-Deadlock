
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



// determine whether system is in a safe state
bool safeState(int avail[], int allo[][NUM_RESOURCES], int ned[][NUM_RESOURCES]) {

    //safety algorithm
    int work[NUM_RESOURCES] = {0};
    int finish[NUM_CUSTOMERS] = {0};

    //work = available
    for (int i=0; i<NUM_RESOURCES; i++) {
        work[i] = avail[i];
    }

    //find index
    for (int rec=0; rec < NUM_CUSTOMERS; rec++) {
        for (int i=0; i<NUM_CUSTOMERS; i++) {
            bool isLessThan = true;
            for (int j=0; j<NUM_RESOURCES; j++) {
                isLessThan = isLessThan && (ned[i][j] <= work[j]);
            }
            if (finish[i] == 0 && isLessThan) {
                //finish[i] is true
                finish[i] = 1;
                //work += allocation
                for (int j=0; j<NUM_RESOURCES; j++) {
                    work[j] += allo[i][j];
                }
                break;
            }
        }
        bool done = true;
        for (int i=0; i<NUM_CUSTOMERS; i++) {
            done = done && (finish[i] == 1);
        }
        if (done) {
            return true;
        }
    }

    return false;
}


int resource_request(int customer_num, int request[]) {
    pthread_mutex_lock(&resource_mutex);

    bool ltNeed = true;
    for (int i = 0; i < NUM_RESOURCES; i++) {
        ltNeed = ltNeed && (request[i] <= need[customer_num][i]);
    }
    if (!ltNeed) {
        pthread_mutex_unlock(&resource_mutex);
        return -1;
    }

    bool ltAvail = true;
    for (int i = 0; i < NUM_RESOURCES; i++) {
        ltAvail = ltAvail && (request[i] <= available[i]);
    }
    if (!ltAvail) {
        pthread_mutex_unlock(&resource_mutex);
        return -1;
    }

    // temp arrays to test state safety
    int avail[NUM_RESOURCES];
    int alloc[NUM_CUSTOMERS][NUM_RESOURCES];
    int ned[NUM_CUSTOMERS][NUM_RESOURCES];

    for (int i = 0; i < NUM_RESOURCES; i++) {
        avail[i] = available[i]-request[i];
        alloc[customer_num][i] = allocation[customer_num][i] + request[i];
        ned[customer_num][i] = need[customer_num][i] - request[i];
    }

    if (!safeState(avail, alloc, ned)) {
        pthread_mutex_unlock(&resource_mutex);
        return -1;
    }

    for (int i = 0; i < NUM_RESOURCES; i++) {
        available[i] = avail[i];
        allocation[customer_num][i] = alloc[customer_num][i];
        need[customer_num][i] = ned[customer_num][i];
    }

    pthread_mutex_unlock(&resource_mutex);
    return 0;
}

int resource_release(int customer_num, int release[]) {
    pthread_mutex_lock(&resource_mutex);

    // temp arrays to test state safety
    int avail[NUM_RESOURCES];
    int alloc[NUM_CUSTOMERS][NUM_RESOURCES];
    int ned[NUM_CUSTOMERS][NUM_RESOURCES];

    for (int i = 0; i < NUM_RESOURCES; i++) {
        avail[i] = available[i]+release[i];
        alloc[customer_num][i] = allocation[customer_num][i] - release[i];
        ned[customer_num][i] = need[customer_num][i] + release[i];
    }

    if (!safeState(avail, alloc, ned)) {
        pthread_mutex_unlock(&resource_mutex);
        return -1;
    }

    for (int i = 0; i < NUM_RESOURCES; i++) {
        available[i] = avail[i];
        allocation[customer_num][i] = alloc[customer_num][i];
        need[customer_num][i] = ned[customer_num][i];
    }

    pthread_mutex_unlock(&resource_mutex);
    return 0;
}

/* void* customer_func(void* ID)
        perform customer functions, request, release resources
*/
void* customer_func(void* ID) {
    int id = * (int*) ID;
    printf("Initialized Customer Number %d\n", id);

    while (true) {
        if (rand()%2 == 0) { //request
            int resources[NUM_RESOURCES] = {0};
            for (int i=0; i<NUM_RESOURCES; i++) {
                resources[i] = rand()%(need[id][i]+1);
            }
            if (resource_request(id, resources) == 0){
                printf("Request Granted!\n");
            }
            else {
                printf("Request Denied!\n");
            }
        }
        else { //release
            int resources[NUM_RESOURCES] = {0};
            for (int i=0; i<NUM_RESOURCES; i++) {
                resources[i] = rand()%(allocation[id][i]+1);
            }
            if (resource_release(id, resources) == 0) {
                printf("Release Granted!\n");
            }
            else {
                printf("Release Denied!\n");
            }
        }
    }

    pthread_exit(NULL);
}

/* int main(int argc, char* argv[])
        initialize arrays and spin up threads for NUM_CUSTOMERS
*/
int main(int argc, char* argv[]) {

    if (argc != NUM_RESOURCES+1) {
        //wrong number of arguments
        printf("Program requires %d arguments.\n", NUM_RESOURCES);
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        if (atoi(argv[i]) < 3) {
            //argument too small
            printf("Program requires arguments to be integers greater than 2.\n");
            exit(1);
        }
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
