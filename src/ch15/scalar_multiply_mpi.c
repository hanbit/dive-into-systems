/*
 * Copyright (c) 2020, Dive into Systems, LLC
 * https://diveintosystems.org/
 */

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define ARRAY_SIZE (40)
#define SCALAR (10)

/* In a real application, the boss process would likely read its input from a
 * data file.  This example program produces a simple array and informs the
 * caller of the size of the array through the array_size pointer parameter. */
int *build_array(int *array_size) {
    int i;
    int *result = malloc(ARRAY_SIZE * sizeof(int));

    if (result == NULL) {
        exit(1);
    }

    for (i = 0; i < ARRAY_SIZE; i++) {
        result[i] = i;
    }

    *array_size = ARRAY_SIZE;
    return result;
}

/* Print the elements of an array, given the array and its size. */
void print_array(int *array, int array_size) {
    int i;
    for (i = 0; i < array_size; i++) {
        printf("%3d ", array[i]);
    }
    printf("\n\n");
}

/* Multiply each element of an array by a scalar value. */
void scalar_multiply(int *array, int array_size, int scalar) {
    int i;
    for (i = 0; i < array_size; i++) {
        array[i] = array[i] * scalar;
    }
}

int main(int argc, char **argv) {
    int rank, process_count;
    int array_size, local_size;
    int scalar;
    int *array, *local_array;

    /* Initialize MPI */
    MPI_Init(&argc, &argv);

    /* Determine how many processes there are and which one this is. */
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Designate rank 0 to be the boss.  It sets up the problem by generating
     * the initial input array and choosing the scalar to multiply it by. */
    if (rank == 0) {
        array = build_array(&array_size);
        scalar = SCALAR;

        printf("Initial array:\n");
        print_array(array, array_size);
    }

    /* Boss sends the scalar value to every process with a broadcast.
     * Worker processes receive the scalar value by making this MPI_Bcast
     * call. */
    MPI_Bcast(&scalar, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* Boss sends the total array size to every process with a broadcast.
     * Worker processes receive the size value by making this MPI_Bcast
     * call. */
    MPI_Bcast(&array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* Determine how many array elements each process will get.
     * Assumes the array is evenly divisible by the number of processes. */
    local_size = array_size / process_count;

    /* Each process allocates space to store its portion of the array. */
    local_array = malloc(local_size * sizeof(int));
    if (local_array == NULL) {
        exit(1);
    }

    /* Boss scatters chunks of the array evenly among all the processes. */
    MPI_Scatter(array, local_size, MPI_INT, local_array, local_size, MPI_INT,
                0, MPI_COMM_WORLD);

    /* Every process (including boss) performs scalar multiplication over its
     * chunk of the array in parallel. */
    scalar_multiply(local_array, local_size, scalar);

    /* Boss gathers the chunks from all the processes and coalesces the
     * results into a final array. */
    MPI_Gather(local_array, local_size, MPI_INT, array, local_size, MPI_INT,
               0, MPI_COMM_WORLD);

    /* Boss prints the final answer. */
    if (rank == 0) {
        printf("Final array:\n");
        print_array(array, array_size);
    }

    /* Clean up. */
    if (rank == 0) {
        free(array);
    }
    free(local_array);
    MPI_Finalize();

    return 0;
}
