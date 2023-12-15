#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>

#define MAX 10 //the maximum value of an element. (10 means 0-9)
pthread_mutex_t mutex; //declare mutex

/*error handling function: prints out error message*/
int print_error(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(2);
}

/*helper function:fillString
/*fills the string pointed to by string with random digits
 *separated by spaces.
*/
void fillString(char *string, long length) {
    long i;
    for (i = 0; i < length; i += 2) {
        string[i] = (rand() % 10) + 49;
        string[i + 1] = 32;
    }
    string[length - 1] = 0;
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

struct t_arg{
    long id; //thread's id
    int *counts_ptr; //pointer to counts
    char *str_ptr; //pointer to string
    long length;
    long nthreads;
};

/* parallel version of countElemsStr (First cut):
 * computes the frequency of all the elements in the input string and stores
 * the associated counts of each element in the array called counts
*/
void *countElemsStr(void *args) {
    struct t_arg *myargs = (struct t_arg *)args;
    long myid = myargs->id;
    char *input_str = myargs->str_ptr;
    int *counts = myargs->counts_ptr;
    long total_length = myargs->length;
    long numthreads = myargs->nthreads;

    //local variables
    int val, i;
    char *token;
    int local_counts[MAX] = {0};
    char *saveptr; //for getting strtok to work

    //compute local start and end values and chunk size:
    long chunk = total_length / numthreads;
    long start = myid * chunk;
    long end = (myid + 1) * chunk;
    if (input_str[start] <= '0') {
        start++; //ensure we start at a number
    }
    if (input_str[end] >= '0') {
        end--; //ensure we end on a space
    }
    input_str[end] = 0; //ending point for parsing

    //printf("[thread: %ld]: start: %ld = %c\n", myid, start, input_str[start]);
    //printf("[thread: %ld]: end: %ld = %c\n", myid, end, input_str[end]);

    //tokenize values
    token = strtok_r(input_str+start, " ", &saveptr);
    while (token != NULL) {
        val = atoi(token); //convert to an int
	local_counts[val] = local_counts[val] + 1; //update associated counts
	token = strtok_r(NULL, " ", &saveptr);
    }

    pthread_mutex_lock(&mutex);
    for (i =0; i < MAX; i++) {
        counts[i] += local_counts[i];
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}


/* main function:
 * calls countElemsStr on a static string and counts up all the digits in
 * that string.
*/
int main(int argc, char **argv) {

    if (argc != 4) { //checks to ensure we have the correct number of args
        fprintf(stderr, "usage: %s <n> <p?> <t>\n", argv[0]);
        fprintf(stderr, "where <n> is the number of digits in the string and\n");
        fprintf(stderr, "<p?> is a binary value (0/1) indicating if counts");
        fprintf(stderr, " should be printed and\n");
        fprintf(stderr, "<t> is the number of threads\n");
        return 1;
    }

    long nthreads = strtol(argv[3], NULL, 10);
    if (nthreads < 1 ) print_error("ERROR: nthreads must be a postive number");

    long length = strtol(argv[1], NULL, 10);
    if (length < nthreads) print_error("ERROR: length must be greater than number of threads");
    int verbose = atoi(argv[2]);
    int ret; //used for error checking
    srand(10); //seed random number generator with static seed
    ret = pthread_mutex_init(&mutex, NULL);
    if (ret) print_error("ERROR: pthread_mutex_init failed");

    //allocate and fill string
    char *inputString = malloc(length * 2 * sizeof(char));
    fillString(inputString, length * 2);

    //allocate counts array.
    int counts[MAX] = {0};
    pthread_t *thread_array = malloc(nthreads * sizeof(pthread_t));
    struct t_arg *thread_args = malloc(nthreads * sizeof(struct t_arg));
    if (!thread_array || !thread_args) print_error("ERROR: malloc failed");

    long t;
    //fill t_args
    for (t = 0; t < nthreads; t++) {
        thread_args[t].id = t;
        thread_args[t].counts_ptr = counts;
        thread_args[t].str_ptr = inputString;
        thread_args[t].length = length * 2;
        thread_args[t].nthreads = nthreads;
    }

    //print out array before sort
    if (verbose == 2) {
        printf("original string:\n");
        printf("%s\n", inputString);
    }

    for (t = 0; t < nthreads; t++) {
        ret = pthread_create( &thread_array[t], NULL, countElemsStr, &thread_args[t] );
        if (ret) print_error("ERROR: pthread_create failed");
    }
    for (t = 0; t < nthreads; t++) {
        ret = pthread_join(thread_array[t], NULL);
        if (ret) print_error("ERROR: pthread_create failed");
    }
    //print out array after sort
    if (verbose) {
        printf("contents of counts array:\n");
        printCounts(counts);
    }

    //clean up
    pthread_mutex_destroy(&mutex);
    //free(thread_array);
    //free(thread_args);
    //free(inputString);

    return 0;
}
