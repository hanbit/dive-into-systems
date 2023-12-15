#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#define MAX 10 //the maximum value of an element. (10 means 0-9)

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
    string[length-1]=0;
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

/* computes the frequency of all the elements in the input string and stores
 * the associated counts of each element in the array called counts
*/
void countElemsStr(int *counts, char *input_str) {
    int val, i;
    char *token;
    token = strtok(input_str, " ");
    while (token != NULL) {
        val = atoi(token);
        counts[val] = counts[val] + 1;
        token = strtok(NULL, " ");
    }
}


/* main function:
 * calls countElemsStr on a static string and counts up all the digits in
 * that string.
*/
int main(int argc, char **argv) {

    if (argc != 3) { //checks to ensure we have the correct number of args
        fprintf(stderr, "usage: %s <n> <p?>\n", argv[0]);
        fprintf(stderr, "where <n> is the number of digits in the string and\n");
        fprintf(stderr, "<p?> is a binary value (0/1) indicating if counts");
        fprintf(stderr, "should be printed\n");
        return 1;
    }

    long length = strtol(argv[1], NULL, 10);
    int verbose = atoi(argv[2]);
    if (length < 1) print_error("ERROR: must enter a positive length");
    //seed random number generator with static value
    srand(10);

    //fill string with n digits
    char *inputString = malloc(length * 2 * sizeof(char));
    if (!inputString) print_error("ERROR: malloc failed");
    fillString(inputString, length * 2);

    //allocate counts array.
    int counts[MAX] = {0};

    //print out array before sort
    if (verbose > 1) {
        printf("original string:\n");
        printf("%s\n", inputString);
    }
    countElemsStr(counts, inputString);

    //print out array after sort
    if (verbose) {
        printf("contents of counts array:\n");
        printCounts(counts);
    }

    return 0;
}