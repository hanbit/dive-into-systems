#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define MAX 10 //the maximum value of an element. (10 means 0-9)

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
void printCounts(int *counts) {
    int i;
    for (i = 0; i < MAX; i++) {
        printf("%d ", counts[i]);
    }
    printf("\n");
}

/*step 1:
 * compute the frequency of all the elements in the input array and store
 * the associated counts of each element in array counts. The elements in the
 * counts array are initialized to zero prior to the call to this function.
*/
void countElems(int *counts, int *array_A, long length) {
    int val, i;
    for (i = 0; i < length; i++) {
        val = array_A[i]; //read the value at index i
        counts[val] = counts[val] + 1; //update corresponding location in counts
    }
}

/* step 2:
 * overwrite the input array (array_A) using the frequencies stored in the
 *  array counts
*/
void writeArray(int *counts, int *array_A) {
    int i, j = 0, amt;

    for (i = 0; i < MAX; i++) { //iterate over the counts array
        amt = counts[i]; //capture frequency of element i
        while (amt > 0) { //while all values aren't written
            array_A[j] = i; //replace value at index j of array_A with i
            j++; //go to next position in array_A
            amt--; //decrease the amount written by 1
        }
    }
}

/* main function:
 * gets array length from command line args, allocates a random array of that
 * size, allocates the counts array, the executes step 1 of the CountSort
 * algorithm (countsElem) followed by step 2 (writeArray).
*/
int main(int argc, char **argv) {

    if (argc != 3) { //checks to ensure we have the correct number of args
        fprintf(stderr, "usage: %s <n> <p?>\n", argv[0]);
        fprintf(stderr, "where <n> is the length of the array\n");
        fprintf(stderr, "and <p?> is a binary value (0/1) indicating if arrays");
        fprintf(stderr, "should be printed\n");
        return 1;
    }

    srand(10); //use of static seed ensures the output is the same every run
    //replace with srand(time(NULL)) if we want to have random arrays every run

    long length = strtol(argv[1], NULL, 10);
    if (length < 1) print_error("ERROR: length must be greater than 0");

    int verbose = atoi(argv[2]);

    //generate random array of elements of specified length
    int *array = malloc(length * sizeof(int));
    if (!array) print_error("ERROR: malloc failed");

    genRandomArray(array, length);

    //allocate counts array.
    int counts[MAX] = {0};

    //print out array before sort
    if (verbose) {
        printf("array before sort:\n");
        printArray(array, length);
    }
    countElems(counts, array, length); //calls step 1
    writeArray(counts, array); //calls step2

    //print out array after sort
    if (verbose) {
        printf("result after sort:\n");
        printArray(array, length);
    }
    free(array); //free memory

    return 0;
}


