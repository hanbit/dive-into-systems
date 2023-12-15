#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*error handling function: prints out error message*/
int print_error(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(2);
}

/* The "thread function" passed to pthread_create.  Each thread executes this
 * function and terminates when it returns from this function. */
void *HelloWorld(void *id) {

    /* We know the argument is a pointer to a long, so we cast it from a
     * generic (void *) to a (long *). */
    long *myid = (long *)id;

    printf("Hello world! I am thread %ld\n", *myid);

    /* We don't need our threads to return anything. */
    return NULL;
}

int main(int argc, char **argv) {
    int i;
    int nthreads; //number of threads
    pthread_t *thread_array; //pointer to future thread array
    long *thread_ids;
    int ret; //useful for error detection

    /* Read the number of threads to create from the command line. */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <n>\n", argv[0]);
        fprintf(stderr, "where <n> is the number of threads\n");
        return 1;
    }
    nthreads = strtol(argv[1], NULL, 10);
    if (nthreads < 1) print_error("ERROR: enter a positive number of threads");

    /* Allocate space for thread structs and identifiers. */
    thread_array = malloc(nthreads * sizeof(pthread_t));
    thread_ids = malloc(nthreads * sizeof(long));
    if (!thread_array || !thread_ids) print_error("ERROR: malloc failed");

    /* Assign each thread an ID and create all the threads. */
    for (i = 0; i < nthreads; i++) {
        thread_ids[i] = i;
        ret = pthread_create(&thread_array[i], NULL, HelloWorld, &thread_ids[i]);
        if (ret) print_error("ERROR: pthread create failed");
    }

    /* Join all the threads. Main will pause in this loop until all threads
     * have returned from the thread function. */
    for (i = 0; i < nthreads; i++) {
        ret = pthread_join(thread_array[i], NULL);
        if (ret) print_error("ERROR: pthread join failed");
    }

    free(thread_array);
    free(thread_ids);

    return 0;
}
