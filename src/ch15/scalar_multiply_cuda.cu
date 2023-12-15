/*
 * Copyright (c) 2020, Dive into Systems, LLC
 * https://diveintosystems.org/
 *
 * Example CUDA program that does scalar vector multiply
 *
 * to compile: 
 *  nvcc -g -G -o scalar_multiply_cuda  scalar_multiply_cuda.cu -lstdc++
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cuda.h>

/* define these so that N is evenly divided by BLOCK_SIZE */
#define BLOCK_SIZE       64    /* threads per block */ 
#define N              10240   /* vector size */ 
#define DEFAULT_VALUE  3       /* default scalar value */

/******** prototypes ************/
/* host-side: vector initialization function */
void init_array(int *vector, int size, int step);
/* host-size: print out some vector values */
void print_some(int *vector, int size, int step);
/* cuda scalar multiply kernel */
__global__ void scalar_multiply(int *array, int scalar); 


/***************************************************/
// host-side main:
int main(int argc, char **argv) {

  int *vector, *dev_vector, scalar;

  scalar = DEFAULT_VALUE;
  if(argc == 2) {
    scalar = atoi(argv[1]);
  }

  // allocate host memory space for the vector
  vector = (int *)malloc(sizeof(int)*N);
  if (vector == NULL) {
    printf("Error: malloc failed\n");
    exit(1);
  }
  // initialize vector in host memory
  init_array(vector, N, 7);

  // allocate GPU device memory for vector
  if (cudaMalloc(&dev_vector, sizeof(int)*N) != cudaSuccess) {
    printf("Error: cudaMalloc failed\n");
    exit(1);
  }

  // copy host vector to device memory
  if (cudaMemcpy(dev_vector, vector, sizeof(int)*N, cudaMemcpyHostToDevice)
      != cudaSuccess) 
  {
    printf("Error: cudaMemcpy host to dev failed\n");
    exit(1);
  }

  // call the CUDA scalar_multiply kernel
  // specify the 1D blocks/grid and 1D threads/block layout in the call
  scalar_multiply<<<(N/BLOCK_SIZE), BLOCK_SIZE>>>(dev_vector, scalar);

  // copy device vector to host memory
  if (cudaMemcpy(vector, dev_vector, sizeof(int)*N, cudaMemcpyDeviceToHost)
      != cudaSuccess) 
  {
    printf("Error: cudaMemcpy dev to host failed\n");
    exit(1);
  }

  // print out some of the result to check
  print_some(vector, N, 117);

  // free space
  cudaFree(dev_vector);
  free(vector);

  return 0;
}
/***************************************************/
/*
 * CUDA kernel function that performs scalar multiply 
 * of a vector on the GPU device
 *
 * This assumes that there are enough threads to associated
 * each array[i] element with a signal thread
 * (in general, each thread would be responsible for a set of data elements)
 */
__global__ void scalar_multiply(int *array, int scalar) {

  int index;

  // get the thread's index value based on its 
  // position in the enclosing block and grid
  index = blockIdx.x * blockDim.x + threadIdx.x;

  array[index] = array[index] * scalar;
}
/***************************************************/
/* host-side: vector initialization function */
void init_array(int *vector, int size, int step) {

  int i;

  for(i=0; i < size; i++) { 
    vector[i] = i % step; 
  }

}
/* host-size: print out some vector values */
void print_some(int *vector, int size, int step) {

  int i;

  i = 0;
  while( i < size) {
    printf("v[%d] = %d\n", i, vector[i]);
    i = i + step;
  }

}



