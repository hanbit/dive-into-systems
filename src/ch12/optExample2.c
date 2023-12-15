#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

//helper function: computes wall clock time
double getTime(struct timeval ts, struct timeval te){
    double time = te.tv_sec - ts.tv_sec + (te.tv_usec - ts.tv_usec)/1.e6;
    return time;
}

//helper function: allocates an array of a specified length and returns a pointer
int * allocateArray(int len) {
    int * result = malloc(len * sizeof(int));
    return result;
}

//helper function: prints out elements of array separated by spaces
void printArray(int * arr, int len) {
    int i;
    for (i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

//helper function: checks to see if a number is prime
int isPrime(int x) {
    int i;
    int max = sqrt(x)+1;
    for (i = 2; i < max; i++) { //no prime number is less than 2
        if (x % i == 0) { //if the number is divisible by i
            return 0; //it is not prime
        }
    }
    return 1; //otherwise it is prime
}

// finds the next prime
int getNextPrime(int prev) {
    int next = prev + 1;
    while (!isPrime(next)) { //while the number is not prime
        next++; //increment and check again
    }
    return next;
}

// generates a sequence of primes
int genPrimeSequence(int * array, int limit) {
    int i;
    int len = limit;
    if (len == 0) return 0;
    array[0]=2; //initialize the first number to 2
    for (i = 1; i < len; i++) {
        array[i] = getNextPrime(array[i-1]); //fill in the array
        if (array[i] > limit){
            len = i;
            return len;
        }
    }
    return len;
}

int main(int argc, char ** argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <num>\n", argv[0]);
        printf("where <num> is upper limit of the desired range of primes\n");
        return 1;
    }

    struct timeval tstart, tend;
    int limit = strtol(argv[1], NULL, 10);
    gettimeofday(&tstart, NULL);
    int * array = allocateArray(limit); //array can't be longer than the limit size
    gettimeofday(&tend, NULL);
    printf("Time to allocate: %g\n", getTime(tstart, tend));

    gettimeofday(&tstart, NULL);
    int length = genPrimeSequence(array, limit);
    gettimeofday(&tend, NULL);
    printf("Time to generate primes: %g\n", getTime(tstart, tend));
    printf("%d primes found.\n", length);
    //printf("The first %ld prime numbers are:\n", limit);
    //printArray(array, length);
    return 0;
}
