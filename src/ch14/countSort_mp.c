#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <sys/time.h>

#define MAX 10

/*error handling function: prints out error message*/
int print_error(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(2);
}

void genRandomArray(int *array, long length) {
    int i;
    for (i = 0; i < length; i++) {
        array[i] = rand() % MAX;
    }
}

void printArray(int *array, long length) {
    int i;
    for (i = 0; i < length; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

void printCounts(int *counts) {
    int i;
    for (i = 0; i < MAX; i++) {
        printf("%d ", counts[i]);
    }
    printf("\n");
}


void countElems(int *counts, int *array, long length) {

    #pragma omp parallel default(none) shared(counts, array, length)
    {
        int val, i, local[MAX] = {0};
        #pragma omp for
        for (i = 0; i < length; i++){
            val = array[i];
            local[val]++;
        }

       #pragma omp critical
       {
           for (i = 0; i < MAX; i++){
               counts[i] += local[i];
           }
       }
   }
}

void writeArray(int *counts, int *array) {
    int i;

    //assumed the number of threads is no more than MAX
    #pragma omp parallel for schedule(dynamic)
    for (i = 0; i < MAX; i++) {
        int j = 0, amt, start = 0;
        for (j = 0; j < i; j++) { //calculate the "true" start position
            start += counts[j];
        }

        amt = counts[i]; //the number of array positions to fill

        //overwrite amt elements with value i, starting at position start
        for (j = start; j < start + amt; j++){
            array[j] = i;
        }
    }
}

int main(int argc, char **argv) {

    if (argc != 3){
        fprintf(stderr, "usage: %s <n> <t>\n", argv[0]);
        fprintf(stderr, "where <n> is the length of the array\n");
        fprintf(stderr, "and <t> is the number of elements in the array\n");
        return 1;
    }

    srand(10);

    long length = strtol(argv[1], NULL, 10);
    if (length < 1) print_error("ERROR: length must be greater than 0");

    int *array = malloc(length * sizeof(int));
    if (!array) print_error("ERROR: malloc failed");

    genRandomArray(array, length); //randomize elements
    int counts[MAX] = {0};
    long nthreads = strtol(argv[2], NULL, 10);
    omp_set_num_threads(nthreads);

    //printArray(array, length);

    struct timeval tstart, tend;
    gettimeofday(&tstart, NULL);
    countElems(counts, array, length);
    gettimeofday(&tend, NULL);
    double time = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;
    printf("Run Time for Phase 1 is %g\n", time);

    gettimeofday(&tstart, NULL);
    writeArray(counts, array);
    gettimeofday(&tend, NULL);
    time = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;
    printf("Run Time for Phase 2 is %g\n", time);

    //printArray(array, length);

    //printCounts(counts);


    free(array);

    return 0;
}


