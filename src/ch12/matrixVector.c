#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#define DEBUG 0

//helper function: computes wall clock time
double getTime(struct timeval ts, struct timeval te){
    double time = te.tv_sec - ts.tv_sec + (te.tv_usec - ts.tv_usec)/1.e6;
    return time;
}

//helper function: allocates an array of a specified length
int * allocateArray(int len) {
    int * result = malloc(len * sizeof(int));
    return result;
}

//helper function: fills array with elements
void fillArrayRandom(int * array, int len) {
    int i;
    for (i = 0; i < len; i++) {
        array[i] = 1+rand()%100;
    }
}

//helper function: fills an array with zeros
void fillArrayZeros(int * array, int len) {
    int i;
    for (i = 0; i < len; i++) {
        array[i] = 0;
    }
}

//helper function: prints out elements of array separated by spaces
void printArray(int * arr, int len) {
    long i;
    for (i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

//prints out the elements of a matrix, with each row on a separate line
void printMatrix(int ** mat, long rows, long cols) {
    long i;
    for (i = 0; i < rows; i++) {
        printArray(mat[i], cols);
    }
}

void matrixVectorMultiply(int ** mat, int * vec, int ** res, int row, int col){
    int i, j;
    for (j = 0; j < col; j++){
        for (i = 0; i < row; i++){
            res[i][j] = mat[i][j] * vec[j];
        }
    }
}

int main(int argc, char ** argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <n> <m>\n", argv[0]);
        printf("where <n> is the number of rows and <m> is the number of cols\n");
        printf("program will allocate a random nxm matrix and a vector of size m\n");
        printf("and perform matrix-vector multiplication on them.\n");
        return 1;
    }

    struct timeval tstart, tend;
    int rows = strtol(argv[1], NULL, 10);
    int cols = strtol(argv[2], NULL, 10);
    int i;
    //srand(time(NULL));
    srand(4);
    //if (DEBUG) srand(4); //set static seed if debugging

    //declare, allocate and fill input and output matrices
    gettimeofday(&tstart, NULL);
    int ** matrix = malloc(rows*sizeof(int *));
    int ** result = malloc(rows*sizeof(int *));

    //allocate matrices
    for (i = 0; i < rows; i++){
        matrix[i] = allocateArray(cols);
        result[i] = allocateArray(cols);
    }

    //fill matrices
    for (i = 0; i < rows; i++){
        fillArrayRandom(matrix[i], cols);
        fillArrayZeros(result[i], cols);
    }
    gettimeofday(&tend, NULL);
    printf("Time to allocate and fill matrices: %g\n", getTime(tstart, tend));

    //allocate and fill vector
    gettimeofday(&tstart, NULL);
    int * vector = allocateArray(cols);
    fillArrayRandom(vector, cols);
    gettimeofday(&tend, NULL);
    printf("Time to allocate vector: %g\n", getTime(tstart, tend));

    //perform matrix-vector multiplication
    gettimeofday(&tstart, NULL);
    matrixVectorMultiply(matrix, vector, result, rows, cols);
    gettimeofday(&tend, NULL);
    printf("Time to matrix-vector multiply: %g\n", getTime(tstart, tend));

    //print out matrix and result if the debug flag is on
    if (DEBUG) {
        printf("Matrix:\n");
        printMatrix(matrix, rows, cols);

        printf("\nVector:\n");
        printArray(vector, cols);

        printf("\nResult:\n");
        printMatrix(result, rows, cols); 
    }

    return 0;
}
