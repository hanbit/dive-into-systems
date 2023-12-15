/*
 * Copyright (c) 2020, Dive into Systems, LLC
 *
 * https://diveintosystems.org/
 */

/*
 * An example program that uses pthreads condition variables.
 * This is a simplified producer-consumer problem, where some
 * number of chickens each lay a number of eggs, and the same
 * number of farmers take eggs from chickens when they are
 * available.  The farmer will take a nap if there are no eggs
 * to collect.  A chicken will wake up a napping farmer after
 * it lays and egg.
 *
 * To compile: gcc -g -o layeggs layeggs.c -lpthread
 *
 * To run: ./layeggs <n> <t>   # n: num eggs, t: num chicken & farmer threads
 *
 *   # 4 chickens each lay 10 eggs, and 4 farmers each collect 10 eggs:
 *   ./layeggs 10 4
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define EGGTIME 100000   /* the number of microseconds it takes to lay an egg */

/* Error handling function: prints out error message */
int print_error(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(2);
}

/* thread args struct that contains pointers to the synchronization
 * variables for egg producers and egg consumers
 * This is also an example of using pointer fields to refer to
 * a common value shared by all threads (vs. using globals for these)*/
struct t_arg {
    int id;         // thread id
    int total_eggs; // total number of eggs to lay or collect

    // These fields point to variables shared by all threads:
    int *num_eggs;           // number of eggs currently ready to collect
    pthread_mutex_t *mutex;  // mutex associated with cond variable
    pthread_cond_t  *eggs;   // farmer blocks if no eggs ready to collect
};

void *chicken(void* args);  // main loop for chicken threads
void *farmer(void* args);   // main loop for farmer threads

int main(int argc, char **argv) {
    int total_eggs, nthreads, ret, i;
    pthread_t *thread_array; // thread id array
    struct t_arg * thread_args; // thread args struct array

    // these will be shared by all threads via pointer fields in t_args
    int num_eggs;           // number of eggs ready to collect
    pthread_mutex_t mutex;  // mutex associated with cond variable
    pthread_cond_t  eggs;   // used to block/wake-up farmer waiting for eggs

    if (argc != 3) {
        fprintf(stderr, "usage: %s <n> <t>\n", argv[0]);
        fprintf(stderr, "where <n> is the number of eggs to create/collect\n");
        fprintf(stderr, "and <t> is the number of threads\n");
        return 1;
    }
    total_eggs = atoi(argv[1]);
    nthreads = atoi(argv[2]);

    // check for bad values and set to something okay if bad
    if (total_eggs <= 0) {  total_eggs = 10; }
    if (nthreads <= 0) { nthreads = 3; }

    num_eggs = 0; // number of eggs ready to collect
    ret = pthread_mutex_init(&mutex, NULL); //initialize the mutex
    if (ret) print_error("ERROR pthread_mutex_init failed");

    pthread_cond_init(&eggs, NULL); //initialize the mutex
    if (ret) print_error("ERROR pthread_cond_init failed");

    // will create nthreads chicken threads and nthreads farmer threads
    thread_array = malloc(2 * nthreads * sizeof(pthread_t));
    thread_args = malloc(2 * nthreads * sizeof(struct t_arg));
    if ((!thread_array) || (!thread_args)) print_error("ERROR malloc failed");

    // create some chicken and farmer threads
    for (i = 0; i < (2 * nthreads); i++) {
        thread_args[i].id = i;
        thread_args[i].total_eggs = total_eggs;
        // NOTE: these fields point to shared state
        thread_args[i].num_eggs = &num_eggs;
        thread_args[i].mutex = &mutex;
        thread_args[i].eggs = &eggs;
        if ( (i % 2) == 0 ) {
            ret = pthread_create( &thread_array[i], NULL, chicken, &thread_args[i] );
        }
        else {
            ret = pthread_create( &thread_array[i], NULL, farmer, &thread_args[i] );
        }
        if (ret) print_error("ERROR: pthread create failed");
    }

    // wait for chicken and farmer threads to exit
    for (i = 0; i < (2 * nthreads); i++) {
        ret = pthread_join(thread_array[i], NULL);
        if (ret) print_error("ERROR: pthread join failed");
    }

    // clean-up program state
    pthread_mutex_destroy(&mutex); //destroy the mutex
    pthread_cond_destroy(&eggs);   //destroy the cond var

    free(thread_array);
    free(thread_args);

    return 0;
}

/***********************************************************/
/* Thread main loop for chickens: create some eggs,
 * notifying the farmer after each egg created
 *
 *   args: pointer to t_arg struct for this thread
 *   returns: NULL
 */
void *chicken(void *args){
    struct t_arg *myargs = (struct t_arg *)args;
    int *num_eggs, i, num;

    num_eggs = myargs->num_eggs;
    i = 0;

    // lay some eggs
    for (i = 0; i < myargs->total_eggs; i++) {
        usleep(EGGTIME); // chicken sleeps

        pthread_mutex_lock(myargs->mutex);
        *num_eggs = *num_eggs + 1;  // update number of eggs
        num = *num_eggs;
        pthread_cond_signal(myargs->eggs); // wake up a sleeping farmer (squawk)
        pthread_mutex_unlock(myargs->mutex);

        printf("chicken %d created egg %d available %d\n", myargs->id, i, num);
        fflush(stdout);
    }
    return NULL;
}
/***********************************************************/
/* Thread main loop for farmers: take some eggs from chickens 
 *
 *   args: pointer to t_arg struct for this thread
 *   returns: NULL
 */
void *farmer(void *args ){
    struct t_arg *myargs = (struct t_arg *)args;
    int *num_eggs, i, num;

    num_eggs = myargs->num_eggs;

    i = 0;

    for (i = 0; i < myargs->total_eggs; i++) {
        pthread_mutex_lock(myargs->mutex);
        while (*num_eggs == 0 ) { // no eggs to collect
            // wait for a chicken to lay an egg
            pthread_cond_wait(myargs->eggs, myargs->mutex);
        }

        // we hold mutex lock here and num_eggs > 0
        num = *num_eggs;
        *num_eggs = *num_eggs - 1;
        pthread_mutex_unlock(myargs->mutex);

        printf("farmer %d gathered egg %d available %d\n", myargs->id, i, num);
        fflush(stdout);
    }
    return NULL;
}

