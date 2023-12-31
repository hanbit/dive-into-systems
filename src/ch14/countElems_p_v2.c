#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define MAX 10

/*error handling function: prints out error message*/
int print_error(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(2);
}

/* helper function: genRandomArray
 * fills an input array of specified length (length) with random 
   values from 0 to MAX-1
*/
void genRandomArray(int *array, long length) {
   int i;
   for (i = 0; i < length; i++) {
       array[i] = rand() % MAX;
   }
}

/* helper function: printArray
 * prints out all the values in the input array separated by spaces
 */
void printArray(int *array, long length) {
    int i;
    for (i = 0; i < length; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

/*helper function: printCounts
 * prints out all the values in the counts array, separated by spaces
*/
void printCounts(long *counts) {
    int i;
    for (i = 0; i < MAX; i++) {
        printf("%ld ", counts[i]);
    }
    printf("\n");
}

struct t_arg {
    long id; //thread id
    long numthreads; //number of threads
    int *ap; //pointer to array to be sorted
    int length; //length of  array to be sorted
    long *countp; //pointer to count array
};


pthread_mutex_t mutex; //global declaration of mutex, initialized in main()

/*parallel version of step 1 of CountSort algorithm (attempt 1 with mutexes):
 * extracts arguments from args value
 * calculates the portion of the array that thread is responsible for counting
 * computes the frequency of all the elements in assigned component and stores
 * the associated counts of each element in counts array
*/
void *countElems(void *args) {
    //extract arguments
    struct t_arg *myargs = (struct t_arg *)args;
    long myid = myargs->id;
    long nthreads = myargs->numthreads;
    long length = myargs->length;
    int *array = myargs->ap;
    long *counts = myargs->countp;

    //assign work to the thread
    long chunk = length / nthreads; //nominal chunk size
    long start = myid * chunk;
    long end = (myid + 1) * chunk;
    long val;
    if (myid == nthreads - 1) {
        end = length;
    }

    long i;

    //heart of the program
    pthread_mutex_lock(&mutex); //acquire the mutex lock
    for (i = start; i < end; i++) {
        val = array[i];
        counts[val] = counts[val] + 1;
    }
    pthread_mutex_unlock(&mutex); //release the mutex lock

    return NULL;
}



int main(int argc, char **argv) {

    if (argc != 4) {
        fprintf(stderr, "usage: %s <n> <p?> <t>\n", argv[0]);
        fprintf(stderr, "where <n> is the length of the array\n");
        fprintf(stderr, "and <p?> is the print option (0/1)\n");
        fprintf(stderr, "and <t> is the number of threads\n");
        return 1;
    }

    srand(10);

    long t;
    long length = strtol(argv[1], NULL, 10);
    int verbose = atoi(argv[2]);
    long nthreads = strtol(argv[3], NULL, 10);
    if (nthreads < 1) print_error("ERROR: nthreads must be greater than 0");
    if (length < nthreads) print_error("ERROR: length must be greater than nthreads");
    int ret; //for error handling

    //generate random array of elements of specified length
    int *array = malloc(length * sizeof(int));
    if (!array) print_error("ERROR: malloc failed!");
    genRandomArray(array, length);

    //start timer
    struct timeval tstart, tend;
    gettimeofday(&tstart, NULL);

    //specify counts array and initialize all elements to zero
    long counts[MAX] = {0};

    //allocate threads and args array
    pthread_t *thread_array; //pointer to future thread array
    thread_array = malloc(nthreads * sizeof(pthread_t)); //allocate the array
    struct t_arg *thread_args = malloc(nthreads * sizeof(struct t_arg));
    if (!thread_array || !thread_args) print_error("ERROR: malloc failed!");

    //fill thread array with parameters
    for (t = 0; t < nthreads; t++) {
        thread_args[t].id = t;
        thread_args[t].numthreads = nthreads;
        thread_args[t].ap = array; //pointer to array
        thread_args[t].length = length;
        thread_args[t].countp = counts; //pointer to counts array
    }

    ret = pthread_mutex_init(&mutex, NULL); //initialize the mutex
    if (ret) print_error("ERROR: pthread_mutex_init failed");

    for (t = 0; t < nthreads; t++) {
        ret = pthread_create( &thread_array[t], NULL, countElems, &thread_args[t] );
        if (ret) print_error("ERROR: pthread_create failed");
    }

    for (t = 0; t < nthreads; t++) {
        pthread_join(thread_array[t], NULL);
        if (ret) print_error("ERROR: pthread_create failed");
    }
    pthread_mutex_destroy(&mutex); //destroy (free) the mutex

    free(thread_array);
    free(array);
    gettimeofday(&tend, NULL);

    if (verbose) {
        printf("Counts array:\n");
        printCounts(counts);
    }
    else {
        double time = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;
        printf("Time for Step 1 is %g s\n", time);
    }
    return 0;
}
